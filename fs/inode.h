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

namespace {
	namespace inode_mode {
		const unsigned char DEFAULT_PRIVILEGE = 0x3d;  /* RWX for owner. RX for others. Include file and directory. */
		/* Directory or file. Don't have group tentatively. */
		const unsigned char READ_OWNER = 0x20;
		const unsigned char WRITE_OWNER = 0x10;
		const unsigned char EXEC_OWNER = 0x08;
		const unsigned char READ_OTHER = 0x04;
		const unsigned char WRITE_OTHER = 0x02;
		const unsigned char EXEC_OTHER = 0x01;
	};
}

void create_inode(struct inode *inode);
void free_inode(unsigned int ino);
void sync_inode(struct inode *inode);
void sync_block(unsigned int block_no, char *buf);
void sync_inode_bitmap();
void sync_block_bitmap();
struct inode ino2inode(unsigned int ino);
unsigned int allocate_block();
void read_block(unsigned int block_no, char *buf);
void free_block(unsigned int block_no);

#endif

