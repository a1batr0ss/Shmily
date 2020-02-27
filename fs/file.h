#ifndef __FS_FILE_H__
#define __FS_FILE_H__

struct file {
	unsigned int offset;
	struct inode *inode;
};

extern struct file* file_desc_tbl[64];

void sys_mkfile(char *path);
void sys_rmfile(char *path);
int open_file(char *path);
void close_file(unsigned int fd);
void write_file(unsigned int fd, char *str, unsigned int count);
int read_file(unsigned int fd, char *buf, unsigned int count);
void lseek_file(unsigned int fd, unsigned int offset);
bool is_eof(unsigned int fd);

#endif

