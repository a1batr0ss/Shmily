#include <global.h>
#include <stdio.h>
#include <string.h>
#include <all_syscall.h>
#include "file.h"
#include "inode.h"
#include "dir.h"
#include "fs.h"
#include "super_block.h"

struct file* file_desc_tbl[64];

void free_inode_sectors(struct inode inode);
int get_fdt_free_slot();
void free_fdt_slot(unsigned int slot);

void mkfile(char *path)
{
	/* Create a dir entry in the parent directory. */
	char parent[64] = {0};
	char *file_name = split_path_2parts(path, parent);
	unsigned int parent_ino = dir_is_exists(parent);

	if (-1 == parent_ino)
		return;

	struct inode parent_inode = ino2inode(parent_ino);
	struct inode file_inode;
	memset((char*)&file_inode, 0, sizeof(struct inode));
	create_inode(&file_inode);
	sync_inode(&file_inode);
	sync_inode_bitmap();

	struct dir_entry file_entry;
	memset((char*)&file_entry, 0, sizeof(struct dir_entry));
	strcpy(file_entry.name, file_name);
	file_entry.type = FILE;
	file_entry.inode_no = file_inode.inode_no;

	create_dir_entry(parent_inode, file_entry);

	return;
}

void rmfile(char *path)
{
	char parent[64] = {0};
	char *file_del = split_path_2parts(path, parent);
	unsigned int parent_ino = dir_is_exists(parent);
	unsigned int file_del_ino = dir_is_exists(path);

	if (-1 == file_del_ino) {
		printf("file is not exists.\n");
		return;
	}

	struct inode inode_parent = ino2inode(parent_ino);
	struct inode inode_del = ino2inode(file_del_ino);

	remove_dir_entry(inode_parent, file_del);

	/* Free the inode content's sectors. */
	free_inode_sectors(inode_del);

	free_inode(file_del_ino);
	sync_inode_bitmap();
	return;
}

void free_inode_sectors(struct inode inode)
{
	for (int i=0; i<9; i++) {
		if (0 == inode.sectors[i])
			continue;

		unsigned int block_no = inode.sectors[i] - cur_part->sb->data_start;
		free_block(block_no);
	}
	sync_block_bitmap();
	return;
}

int open_file(char *path)
{
	unsigned int ino = dir_is_exists(path);
	if (-1 == ino) {
		printf("file is not exists.\n");
		return -1;
	}

	/* The inode will exist in file_desc_table, so it couldn't be a local variable. */
	struct inode *inode = (struct inode*)malloc(sizeof(struct inode));
	*inode = ino2inode(ino);
	struct file *file = (struct file*)malloc(sizeof(struct file));
	file->inode = inode;
	file->offset = 0;

	int slot = get_fdt_free_slot();
	if (-1 == slot)
		return -1;

	file_desc_tbl[slot] = file;
	return slot;
}

void close_file(unsigned int fd)
{
	struct file *file = file_desc_tbl[fd];
	if (NULL == file)
		return;

	free(file->inode);
	free(file);

	free_fdt_slot(fd);
}

/* fdt: file descriptor table. */
int get_fdt_free_slot()
{
	for (int i=0; i<64; i++) {
		if (NULL == file_desc_tbl[i])
			return i;
	}
	return -1;
}

void free_fdt_slot(unsigned int slot)
{
	if (slot > 63)
		return;
	file_desc_tbl[slot] = NULL;
}

