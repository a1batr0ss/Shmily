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

	/* Normal for positive number. */
	if (-1 != slot) 
		return slot;
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

