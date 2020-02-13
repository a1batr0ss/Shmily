#ifndef __FS_FS_H__
#define __FS_FS_H__

#include <bitmap.h>

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

namespace {
	namespace _fs {
		unsigned int sector_size = 512;
		unsigned int max_file_nr = 1024;
		unsigned int min_file_nr = 64;
	};
}

extern struct partition *cur_part;

void init_fs();

#endif

