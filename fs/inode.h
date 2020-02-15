#ifndef __FS_INODE_H__
#define __FS_INODE_H__

struct inode {
	unsigned int inode_no;
	unsigned int size;

	unsigned long long last_modify_time;
	unsigned int link_cnt;

	unsigned short uid;
	unsigned short gid;
	unsigned int mode;

	unsigned int sectors[9];
} __attribute__((packed));

void create_inode(struct inode *inode);
void sync_inode(struct inode *inode);
void sync_block(unsigned int block_no, char *buf);
void sync_inode_bitmap();
void sync_block_bitmap();
struct inode ino2inode(unsigned int ino);
unsigned int allocate_block();
void read_block(unsigned int block_no, char *buf);

#endif

