#ifndef __INIT_DISK_H__
#define __INIT_DISK_H__

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

struct partition {
	char name[16];
	unsigned int sector_cnt;
	unsigned int start_lba;
	struct disk *disk;
	struct super_block *sb;
};

struct disk {
	char name[16];
	struct channel *channel;
	bool is_slave;
	struct partition primary[4];
	struct partition logic[8];
};

struct channel {
	char name[16];
	unsigned int port;
	unsigned int irq_nr;
	struct disk disks[2];
};

void disk_identify(unsigned char disk_nr);
void init_disk();

#endif

