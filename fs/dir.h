#ifndef __FS_DIR_H__
#define __FS_DIR_H__

#include "inode.h"

struct dir_entry {
	char name[64];
	unsigned int inode_no;
	unsigned char type;
};

void mkdir(char *path);

#endif

