#ifndef __FS_FILE_H__
#define __FS_FILE_H__

struct file {
	unsigned int offset;
	struct inode *inode;
};

extern struct file* file_desc_tbl[64];

void mkfile(char *path);
void rmfile(char *path);
int open_file(char *path);
void close_file(unsigned int fd);

#endif

