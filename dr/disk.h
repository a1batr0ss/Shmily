#ifndef __DR_DISK_H__
#define __DR_DISK_H__

#include <bitmap.h>

namespace hd {
    const unsigned int disk_intr_nr = 0x2e;
    const unsigned short channel_port_base = 0x1f0;
    const unsigned short data_reg = 0x1f0;
    const unsigned short error_reg = 0x1f1;
    const unsigned short sect_cnt_reg = 0x1f2;
    const unsigned short lba_l_reg = 0x1f3;
    const unsigned short lba_m_reg = 0x1f4;
    const unsigned short lba_h_reg = 0x1f5;
    const unsigned short dev_reg = 0x1f6;
    const unsigned short status_reg = 0x1f7;
    const unsigned short cmd_reg = 0x1f7;

    const unsigned char identify = 0xec;
    const unsigned char read = 0x20;
    const unsigned char write = 0x30;

    const unsigned char stat_busy = 0x80;
    const unsigned char stat_unready = 0x40;
    const unsigned char stat_ready = 0x08;
	const unsigned char dev_mbs = 0xa0;
	const unsigned char dev_lba = 0x40;
	const unsigned char dev_dev = 0x10;
};

enum partition_type {
	PRIMARY, EXTEND, LOGIC
};

struct partition {
	char name[16];
	unsigned int sector_cnt;
	unsigned int start_lba;
	enum partition_type type;
	struct disk *disk;
	struct super_block *sb;  /* super block data in fs, just a pointer, same size. */

	struct bitmap block_bmap;
	struct bitmap inode_bmap;
};

struct disk {
	char name[16];
	struct channel *channel;
	bool is_slave;
	struct partition primary[4];
	struct partition logic[4];
};

struct channel {
	char name[16];
	unsigned int port;
	unsigned int irq_nr;
	struct disk disks[2];
};

struct partition_table_entry_mbr {
	char boot_signature;
	char start_head;
	char start_sector;
	char start_cyliner;
	char sys_signature;
	char end_head;
	char end_sector;
	char end_cyliner;
	unsigned int sector_nr_before;  /* lba */
	unsigned int sector_nr_in;
} __attribute__((packed));

struct partition_table_mbr {
	struct partition_table_entry_mbr entries[4];
};

extern struct disk disks[2];

void disk_identify(unsigned char disk_nr);
void init_disk();
void read_sector(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt);
void write_sector(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt);
void print_disk_partition_info(unsigned int disk_nr);

#endif

