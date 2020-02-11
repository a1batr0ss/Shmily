#include <global.h>
#include <stdio.h>
#include <string.h>
#include <all_syscall.h>
#include <bitmap.h>
#include "fs.h"
#include "super_block.h"
#include "inode.h"
#include "dir.h"

#define DIV_ROUND_UP(x, y) ((x + y - 1) / (y))

enum partition_type {
	PRIMARY, EXTEND, LOGIC
};

struct partition {
	char name[16];
	unsigned int sector_cnt;
	unsigned int start_lba;
	enum partition_type type;
	struct disk *disk;
	struct super_block *sb;

	struct bitmap block_bmap;
	struct bitmap inode_bmap;
};

struct disk {
	char name[16];
	struct channel *channel;
	bool is_slave;
	struct partition primary[4];
	struct partition logic[8];
};

struct channel {
	char name[16];
	unsigned int port;
	unsigned int irq_nr;
	struct disk disks[2];
};

struct partition *cur_part;

void partition_install_fs(struct partition *part, unsigned int disk_nr);
void mount_partition(char *part_name);

/* Just for the slave disk. */
void init_fs()
{
	unsigned int disk_nr = 1;
	struct super_block *buf = (struct super_block*)malloc(_fs::sector_size);
	struct disk *disk = (struct disk*)get_disk(disk_nr);

	if (NULL == buf)
		return;

	/* Process primary partitions. */
	for (int i=0; i<4; i++) {
		struct partition *prim_part = &disk->primary[i];

		if (0 == prim_part->sector_cnt)
			continue;

		/* We recorded the extend partition, buf we needn't handle it(Just as offset). */
		if (EXTEND == prim_part->type)
			continue;
		
		memset((char*)buf, 0, _fs::sector_size);

		read_disk(disk_nr, prim_part->start_lba+1, (char*)buf, 1);

		if (!strcmp(buf->magic, "Shmily"))
			partition_install_fs(prim_part, disk_nr);
	}

	/* Process logic partitions. */
	for (int i=0; i<4; i++) {
		struct partition *logic_part = &disk->logic[i];

		if (0 == logic_part->sector_cnt)
			continue;

		memset((char*)buf, 0, _fs::sector_size);

		read_disk(disk_nr, logic_part->start_lba+1, (char*)buf, 1);

		if (!strcmp(buf->magic, "Shmily"))
			partition_install_fs(logic_part, disk_nr);
	}
	
	free(buf);

	mount_partition("sda0");
}

void partition_install_fs(struct partition *part, unsigned int disk_nr)
{
	struct super_block *sb = (struct super_block*)malloc(_fs::sector_size);
	printf("%s is installing file system.\n", part->name);
	memset((char*)sb, 0, _fs::sector_size);

	unsigned int i_bmap_sectors = DIV_ROUND_UP(_fs::max_file_nr, 8 * _fs::sector_size);
	unsigned int i_tbl_sectors = DIV_ROUND_UP(sizeof(struct inode) * _fs::max_file_nr, _fs::sector_size);

	if (part->sector_cnt > 10000)
		sb->is_sufficient = true;
	else {
		sb->is_sufficient = false;
		i_bmap_sectors = DIV_ROUND_UP(_fs::min_file_nr, _fs::sector_size);
		i_tbl_sectors = DIV_ROUND_UP(sizeof(struct inode) * _fs::min_file_nr, _fs::sector_size);
	}

	unsigned int free_sectors = part->sector_cnt - i_bmap_sectors - i_tbl_sectors - 2;
	unsigned int block_bmap_sectors = DIV_ROUND_UP(free_sectors, 8 * _fs::sector_size);

	strcpy(sb->magic, "Shmily");
	sb->sectors_nr = part->sector_cnt;
	sb->start_lba = part->start_lba;

	sb->block_bitmap_lba = sb->start_lba + 2;
	sb->block_bitmap_sectors = DIV_ROUND_UP(free_sectors-block_bmap_sectors, 8 * _fs::sector_size);

	sb->inode_bitmap_lba = sb->block_bitmap_lba + sb->block_bitmap_sectors;
	sb->inode_bitmap_sectors = i_bmap_sectors;

	sb->inode_table_lba = sb->inode_bitmap_lba + sb->inode_bitmap_sectors;
	sb->inode_table_sectors = i_tbl_sectors;

	sb->data_start = sb->inode_table_lba + sb->inode_table_sectors;
	sb->root_inode_no = 0;
	sb->dir_entry_size = sizeof(struct dir_entry);

	write_disk(disk_nr, part->start_lba+1, (char*)sb, 1);

	/* block bitmap */
	unsigned int buf_size = (sb->block_bitmap_sectors > sb->inode_table_sectors ? sb->block_bitmap_sectors : sb->inode_table_sectors) * _fs::sector_size;
	unsigned char *buf = (unsigned char*)malloc(buf_size);

	buf[0] |= 0x1;
	write_disk(disk_nr, sb->block_bitmap_lba, (char*)buf, sb->block_bitmap_sectors);

	// inode bitmap
	memset((char*)buf, 0, buf_size);
	buf[0] |= 0x1;
	write_disk(disk_nr, sb->inode_bitmap_lba, (char*)buf, sb->inode_bitmap_sectors);

	// inode table
	memset((char*)buf, 0, buf_size);
	struct inode *inode = (struct inode*)buf;
	inode->size = sizeof(struct dir_entry) * 2;
	inode->inode_no = 0;
	inode->sectors[0] = sb->data_start;
	write_disk(disk_nr, sb->inode_table_lba, (char*)buf, sb->inode_table_sectors);

	// root entry
	memset((char*)buf, 0, buf_size);
	struct dir_entry *e = (struct dir_entry*)buf;

	memcpy(e->name, ".", 1);
	e->inode_no = 0;
	e++;
	memcpy(e->name, "..", 2);
	e->inode_no = 0;
	write_disk(disk_nr, sb->data_start, (char*)buf, 1);

	free(sb);
	free(buf);
}

void mount_partition(char *part_name)
{
	unsigned int disk_nr = 1;
	struct disk *disk = (struct disk*)get_disk(disk_nr);
	unsigned int i = 0;

	while (i < 8) {
		struct partition *part = &disk->primary[i];
		if (i > 3)
			part = &disk->logic[i-4];
		
		if (strcmp(part_name, part->name))
			cur_part = part;  /* Cross process. */

		i++;
	}

	printf("cur_part %s.\n", cur_part);
	/* Load this partition's infomation from disk. */
	struct super_block *sb = (struct super_block*)malloc(_fs::sector_size);
	read_disk(disk_nr, cur_part->start_lba+1, (char*)sb, 1);
	
	cur_part->sb = sb;

	unsigned char *block_bmap = (unsigned char*)malloc(sb->block_bitmap_sectors * _fs::sector_size);
	read_disk(disk_nr, sb->block_bitmap_lba, (char*)block_bmap, sb->block_bitmap_sectors);

	cur_part->block_bmap.base = block_bmap;
	cur_part->block_bmap.bytes_len = sb->block_bitmap_sectors * _fs::sector_size;

	unsigned char *inode_bmap = (unsigned char*)malloc(sb->inode_bitmap_sectors * _fs::sector_size);
	read_disk(disk_nr, sb->inode_bitmap_lba, (char*)inode_bmap, sb->inode_bitmap_sectors);
	cur_part->inode_bmap.base = inode_bmap;
	cur_part->inode_bmap.bytes_len = sb->inode_bitmap_sectors * _fs::sector_size;

	return;
}
