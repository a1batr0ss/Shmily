#include <global.h>
#include <stdio.h>
#include <string.h>
#include <all_syscall.h>
#include "dir.h"
#include "fs.h"
#include "super_block.h"

int find_dir_entry(struct inode &inode, char *child);
int dir_is_exists(char *path);
char* split_path_2parts(char *path, char *parent);
void create_dir_entry(struct inode inode, struct dir_entry child);
char* split_path(char *path, char *cur_path);


/* Couldn't create directory recursively. */
void mkdir(char *path)
{
	char parent[64] = {0};
	char *child_name = split_path_2parts(path, parent);	
	unsigned int parent_ino = dir_is_exists(parent);

	if (-1 == parent_ino)
		return;

	struct inode inode = ino2inode(parent_ino);

	struct inode child_inode;
	memset((char*)&child_inode, 0, sizeof(struct inode));
	create_inode(&child_inode);
	sync_inode(&child_inode);
	sync_inode_bitmap();

	struct dir_entry child;
	memset((char*)&child, 0, sizeof(struct dir_entry));
	strcpy(child.name, child_name);
	child.inode_no = child_inode.inode_no;

	create_dir_entry(inode, child);
}

void skip_last_slash(char *str)
{
	int l = strlen(str);
	if ('/' == str[l-1])
		str[l-1] = 0;
	return;
}

char* split_path_2parts(char *path, char *parent)
{
	if ('/' != path[0])
		return NULL;

	char *parent_ = parent;
	int l = 1;
	for (char *p=path; 0!=(*p); p++) {
		if ('/' == *p)
			l++;
	}

	for (int i=0; i<l-1; parent++, path++) {
		*parent = *path;
		if ('/' == *path)
			i++;
	}

	if (0 != parent_[1]) {
		*(parent-1) = 0;  /* Replace the last '/' with 0 */
	/* else parent = "/" */
	
		skip_last_slash(parent_);
	}

	return path;
}

void create_dir_entry(struct inode inode, struct dir_entry child)
{
	unsigned int disk_nr = 1;
	char *buf = (char*)malloc(_fs::sector_size);
	unsigned int free_sector = 0;
	unsigned int free_offset = 0;

	/* Get a free slot. */
	for (int i=0; i<9; i++) {
		if (0 == inode.sectors[i])
			continue;

		memset(buf, 0, _fs::sector_size);
		read_disk(disk_nr, inode.sectors[i], buf, 1);
		
		struct dir_entry *p = (struct dir_entry*)buf;
		/* Check the sector. */
		for (int j=0; j<(_fs::sector_size / sizeof(struct dir_entry)); j++, p++) {
			if (0 == p->name[0]) {
				/* Found the free slot */
				free_sector = inode.sectors[i];
				free_offset = i;
			} else {
				if (strcmp(p->name, child.name)) {
					printf("Already exists.\n");
					free(buf);
					return;
				}
			}
		}
	}

	/* Sync block */
	if (0 != free_sector) {
		memset(buf, 0, _fs::sector_size);
		read_disk(disk_nr, free_sector, buf, 1);

		struct dir_entry *p = (struct dir_entry*)buf;
		p[free_offset] = child;
		write_disk(disk_nr, free_sector, buf, 1);
		
		free(buf);

		return;
	}

	for (int i =0; i<9; i++) {
		if (0 != inode.sectors[i])
			continue;

		/* We should allocate a new sector. */
		unsigned int block_no = allocate_block();
		read_block(block_no, buf);
		*(struct dir_entry*)buf = child;
		sync_block(block_no, buf);

		inode.sectors[i] = cur_part->sb->data_start + block_no;				
		sync_inode(&inode);

		sync_block_bitmap();

		free(buf);
		return;
	}
}

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

int dir_is_exists(char *path)
{
	if ('/' != path[0])
		return -1;

	if (0 == path[1])
		return 0;

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
			return -1;
		
		inode = ino2inode(parent_inode_no);
	}

	return parent_inode_no;
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

