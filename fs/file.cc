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

void sys_mkfile(char *path)
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

void sys_rmfile(char *path)
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

/* Take a start from head. */
void write_file(unsigned int fd, char *str, unsigned int count)
{
	struct file *file = file_desc_tbl[fd];
	if (NULL == file)
		return;

	if (strlen(str) < count)
		count = strlen(str) + 1;  /* The '/0'. */

	unsigned int sector_cnts = count / _fs::sector_size;
	unsigned int last_sector_bytes = count % _fs::sector_size;
	struct inode *inode = file->inode;

	/* Allocate block for the inode. */
	for (int i=0; i<sector_cnts+1; i++) {
		if (0 == inode->sectors[i]) {
			unsigned int block_no = allocate_block();
			inode->sectors[i] = cur_part->sb->data_start + block_no;
		}
	}
	sync_inode(inode);
	sync_block_bitmap();

	unsigned int disk_nr = 1;
	/* Write to disk. Not serial, we must write to disk singly. */
	int i = 0;
	for (; i<sector_cnts; i++) {
		write_disk(disk_nr, inode->sectors[i], str + (i*512), 1);
	}

	/* The last sector.(padding 0) */
	char *buf = (char*)malloc(_fs::sector_size);
	memcpy(buf, str + (i*512), last_sector_bytes);
	write_disk(disk_nr, inode->sectors[i], buf, 1);

	bool has_surplus = false;
	/* Free the remainder sectors previously. */
	for(int i=sector_cnts+1; i<9; i++) {
		if (0 == inode->sectors[i])
			continue;

		unsigned int block_no = inode->sectors[i] - cur_part->sb->data_start;
		free_block(block_no);
		inode->sectors[i] = 0;
		has_surplus = true;
	}
	inode->size = count;
	sync_inode(inode);

	if (has_surplus)
		sync_block_bitmap();

	free(buf);
}

int read_file(unsigned int fd, char *buf, unsigned int count)
{
	struct file *file = file_desc_tbl[fd];

	if (NULL == file)
		return -1;

	struct inode *inode = file->inode;
	if (count > inode->size)
		count = inode->size;
	unsigned int start_sector = file->offset / _fs::sector_size;
	unsigned int start_offset_first = file->offset % _fs::sector_size;
	unsigned int bytes_first_sector = count > 512 ? _fs::sector_size - file->offset : count;
	unsigned int sector_cnts = count > 512 ? (count-bytes_first_sector) / _fs::sector_size : 0;
	unsigned int last_sector_bytes = (count-bytes_first_sector) % _fs::sector_size;
	unsigned int disk_nr = 1;

	char *buf_temp = (char*)malloc(_fs::sector_size);
	/* The first sector. */
	read_disk(disk_nr, inode->sectors[start_sector], buf_temp, 1);
	memcpy(buf, buf_temp + start_offset_first, bytes_first_sector);
	buf += bytes_first_sector;

	int i = start_sector + 1;
	for (; i<sector_cnts; i++)
		read_disk(disk_nr, inode->sectors[i], buf + (i*512), 1);

	if (count > 512) {
		/* The last sector. */
		read_disk(disk_nr, inode->sectors[i], buf_temp, 1);
		memcpy(buf + (i*512), buf_temp, last_sector_bytes);
	}

	free(buf_temp);
	file->offset += count;

	return count;
}

void lseek_file(unsigned int fd, unsigned int offset)
{
	struct file *file = file_desc_tbl[fd];
	if (offset > file->inode->size)
		offset = file->inode->size;

	file->offset = offset;
	return;
}

bool is_eof(unsigned int fd)
{
	struct file *file = file_desc_tbl[fd];
	return file->offset >= file->inode->size;
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

