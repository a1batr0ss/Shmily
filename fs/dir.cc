#include <global.h>
#include <stdio.h>
#include <string.h>
#include <all_syscall.h>
#include "dir.h"
#include "fs.h"
#include "super_block.h"

char* split_path(char *path, char *cur_path);
int find_dir_entry(struct inode &inode, char *child);

char* split_path(char *path, char *cur_path)
{
	if ('/' == path[0]) {
		cur_path[0] = '/';
		cur_path[1] = 0;
		return path+1;
	}
	if (0 == path[0]) {
		cur_path = NULL;
		return NULL;
	}

	while ('/' != (*path)) {
		*cur_path++ = *path++;
		if (0 == (*path))
			return NULL;
	}
	*cur_path = 0;
	return path+1;
}

bool dir_is_exists(char *path)
{
	if ('/' != path[0])
		return false;

	char cur_path[64] ={0};
	char *next_path = path;
	unsigned int parent_inode_no = cur_part->sb->root_inode_no;
	struct inode inode = ino2inode(parent_inode_no);
		
	while (NULL != next_path) {
		memset(cur_path, 0, 64);
		next_path = split_path(next_path, cur_path);

		if (strcmp("/", cur_path))
			continue;

		parent_inode_no = find_dir_entry(inode, cur_path);
		if (-1 == parent_inode_no)
			return false;
		
		inode = ino2inode(parent_inode_no);
	}

	return true;
}

int find_dir_entry(struct inode &inode, char *child)
{
	char *buf = (char*)malloc(_fs::sector_size);
	unsigned int disk_nr = 1;
	unsigned int entries_per_sector = _fs::sector_size / sizeof(struct dir_entry);  /* Couldn't cross sector. */

	/* Traverse the 9 sectors */
	for (int i=0; i<9; i++) {
		unsigned int sector_lba = inode.sectors[i];
		if (0 == sector_lba)
			continue;
		
		read_disk(disk_nr, sector_lba, buf, 1);
		
		struct dir_entry *p_de = (struct dir_entry*)buf;
		for (int j=0; j<entries_per_sector; j++, p_de++) {
			if (0 == p_de->name[0])
				continue;

			if (strcmp(p_de->name, child))
				return p_de->inode_no;
		}
	}

	return -1;
}

