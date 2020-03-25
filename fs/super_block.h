#ifndef __FS_SUPER_BLOCK_H__
#define __FS_SUPER_BLOCK_H__

struct super_block_data {
	char magic[7];  /* 'S' 'h' 'm' 'i' 'l' 'y' */
	bool is_sufficient;
	unsigned int sectors_nr;
	unsigned int inodes_nr;
	unsigned int start_lba;

	unsigned int block_bitmap_lba;
	unsigned int block_bitmap_sectors;

	unsigned int inode_bitmap_lba;
	unsigned int inode_bitmap_sectors;

	unsigned int inode_table_lba;
	unsigned int inode_table_sectors;

	unsigned int data_start;
	unsigned int root_inode_no;
	unsigned int dir_entry_size;
} __attribute__((packed));

struct super_block {
	struct super_block_data data;

	char padding[456];
} __attribute__((packed));

#endif

