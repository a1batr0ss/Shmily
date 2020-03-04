#ifndef __FS_DIR_H__
#define __FS_DIR_H__

#include "inode.h"

enum dir_entry_type {
	DIR, FILE
};

struct dir_entry {
	char name[64];
	unsigned int inode_no;
	enum dir_entry_type type;
};

extern char cur_dir[64];

void sys_mkdir(char *path);
void sys_rmdir(char *path);
char* split_path_2parts(char *path, char *parent);
int dir_is_exists(char *path);
void create_dir_entry(struct inode inode, struct dir_entry child);
void remove_dir_entry(struct inode inode, char *child);

void pwd();
bool cd(char *path);
void ls(char *path);

#endif

