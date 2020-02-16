#include <stdio.h>
#include <bitmap.h>
#include <all_syscall.h>
#include "inode.h"
#include "fs.h"
#include "super_block.h"

static unsigned int allocate_inode_no();

void create_inode(struct inode *inode)
{
	inode->inode_no = allocate_inode_no();

	/* Set to 0 tentatively. */
	inode->size = 0;
	inode->last_modify_time = 0;
	inode->link_cnt = 0;
	inode->uid = 0;
	inode->gid = 0;
	inode->mode = 0;
	for (int i=0; i<9; i++)
		inode->sectors[i] = 0;
}

static unsigned int allocate_inode_no()
{
	struct bitmap *inode_bmap = &(cur_part->inode_bmap);
	int slot = bitmap_scan(inode_bmap, 1);
	bitmap_set_bit(inode_bmap, slot, 1);

	/* Normal for positive number. */
	if (-1 != slot) 
		return slot;
}

/* Set the inode bitmap. */
void free_inode(unsigned int ino)
{
	struct bitmap *inode_bmap = &(cur_part->inode_bmap);
	bitmap_set_bit(inode_bmap, ino, 0);
	return;
}

/* Write the inode to disk. */
void sync_inode(struct inode *inode)
{
	unsigned int ino = inode->inode_no;
	unsigned int inodes_per_sector = _fs::sector_size / sizeof(struct inode);  /* Not cross two sectors. */
	unsigned int sector_offset = ino / inodes_per_sector;
	unsigned int in_sector_offset = ino % inodes_per_sector;
	unsigned int inode_tbl_lba = cur_part->sb->inode_table_lba; 
	char *buf = (char*)malloc(_fs::sector_size);
	unsigned int disk_nr = 1;

	read_disk(disk_nr, inode_tbl_lba + sector_offset, buf, 1);
	*((struct inode*)buf + in_sector_offset) = *inode;
	write_disk(disk_nr, inode_tbl_lba + sector_offset, buf, 1);

	free(buf);

	return;
}

struct inode ino2inode(unsigned int ino)
{
	unsigned int inodes_per_sector = _fs::sector_size / sizeof(struct inode);
	unsigned int sector_offset = ino / inodes_per_sector;
	unsigned int in_sector_offset = ino % inodes_per_sector;
	unsigned int inode_tbl_lba = cur_part->sb->inode_table_lba;

	char *buf = (char*)malloc(_fs::sector_size);
	unsigned int disk_nr = 1;
	
	read_disk(disk_nr, inode_tbl_lba + sector_offset, buf, 1);

	struct inode inode = *((struct inode*)buf + in_sector_offset);

	free(buf);

	return inode;
}

unsigned int allocate_block()
{
	struct bitmap *bmap = &(cur_part->block_bmap);
	int slot = bitmap_scan(bmap, 1);
	bitmap_set_bit(bmap, slot, 1);
	
	if (-1 != slot)
		return slot;
}

void free_block(unsigned int block_no)
{
	struct bitmap *bmap = &(cur_part->block_bmap);
	bitmap_set_bit(bmap, block_no, 0);
	return;
}

void sync_block(unsigned int block_no, char *buf)
{
	unsigned int disk_nr = 1;
	unsigned block_lba = cur_part->sb->data_start + block_no;
	write_disk(disk_nr, block_lba, buf, 1);
	return;
}

void sync_inode_bitmap()
{
	unsigned int disk_nr = 1;
	write_disk(disk_nr, cur_part->sb->inode_bitmap_lba, (char*)(cur_part->inode_bmap.base), cur_part->sb->inode_bitmap_sectors);
	return;
}

void sync_block_bitmap()
{
	unsigned int disk_nr = 1;
	write_disk(disk_nr, cur_part->sb->block_bitmap_lba, (char*)(cur_part->block_bmap.base), cur_part->sb->block_bitmap_sectors);
	return;
}

void read_block(unsigned int block_no, char *buf)
{
	unsigned int disk_nr = 1;
	unsigned int block_lba = cur_part->sb->data_start + block_no;
	read_disk(disk_nr, block_lba, buf, 1);
	return;
}
