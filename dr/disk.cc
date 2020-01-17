#include <global.h>
#include <ipc_glo.h>
#include <io.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include "disk.h"

struct disk disks[2];

void disk_handler();

void init_disk()
{
	unsigned char disk_nr = *(unsigned char*)0x475;

	/* TODO: Initializing the disks. Now we just have two disks. */
	disks[0].is_slave = false;
	disks[1].is_slave = true;

    Message msg(all_processes::DR); 
    struct _context con;

    /* Register the disk handler(two disks(the best way is put it to a loop. As we not sure the number of disks.)). */
    con.con_1 = 0x2e;
    con.con_2 = (unsigned int)disk_handler;
    msg.reset_message(kr::REGR_INTR, con);
    msg.send_then_recv(all_processes::KR);
    con.con_1 = 0x2f;
    con.con_2 = (unsigned int)disk_handler;
    msg.reset_message(kr::REGR_INTR, con);
    msg.send_then_recv(all_processes::KR);
}

/* Just sleep for a moment. */
static void sleep_seconds_demo(unsigned int sec)
{
	int i, j, k;
	for (i=0; i<sec; i++)
		for (j=0; j<1000; j++)
			for (k=0; k<1000; k++) ;
}

void disk_handler()
{
	inb(hd::status_reg);  /* Pretend this interrupt is handled. */

	Message msg(all_processes::INTERRUPT);
	msg.send(all_processes::DR);
}

static void choose_disk(struct disk *disk)
{
	unsigned char reg_con = 0;
	if (disk->is_slave)
		reg_con = hd::dev_mbs | hd::dev_lba | hd::dev_dev;
	else
		reg_con = hd::dev_mbs | hd::dev_lba;
	
	outb(hd::dev_reg, reg_con);
}

static void choose_sector(struct disk *disk, unsigned int lba, unsigned int cnt)
{
	outb(hd::sect_cnt_reg, cnt);
	outb(hd::lba_l_reg, lba);
	outb(hd::lba_m_reg, lba >> 8);
	outb(hd::lba_h_reg, lba >> 16);

	unsigned char reg_con = hd::dev_mbs | hd::dev_lba | (lba>>24);
	if (disk->is_slave)
		reg_con |= hd::dev_dev;

	outb(hd::dev_reg, reg_con);
}

static bool wait_disk()
{
	Message msg(all_processes::DR);
	struct _context con;
	con.con_1 = 3;
	unsigned max_wait_seconds = 30;

	while (max_wait_seconds-=1 > 0) {
		if (inb(hd::status_reg & hd::stat_busy))
			return inb(hd::status_reg) & hd::stat_ready;
		else
			msg.send_then_recv(all_processes::KR);
	}

	return false;
}

static void swap_pairs_bytes(const char *dst, char *buf, unsigned int len)
{
	unsigned char idx;
	for (idx=0; idx<len; idx+=2) {
		buf[idx+1] = *dst++;
		buf[idx] = *dst++;
	}
	buf[idx] = '\0';
}

static void read_disk(char *buf, unsigned char cnt)
{
	unsigned int bytes_cnt = cnt * 512;
	if (0 == cnt)
		bytes_cnt = 256 * 512;

	insw(hd::data_reg, buf, bytes_cnt/2);
}

static void write_disk(char *buf, unsigned char cnt)
{
	unsigned int bytes_cnt = cnt * 512;
	if (0 == cnt)
		bytes_cnt = 256 * 512;

	outsw(hd::data_reg, buf, bytes_cnt/2);
}

void write_sector(struct disk *disk, unsigned int lba, char *buf, unsigned int cnt);

void disk_identify(unsigned char disk_nr)
{
	choose_disk(&disks[disk_nr]);

	outb(hd::cmd_reg, hd::identify);

	/* TODO: transmit information with interrupt. Don't have a appropriate method. */
	Message msg(all_processes::DR);
	msg.receive(all_processes::INTERRUPT);

	if (!wait_disk()) {
		printf("Not ready.\n");
		return;
	}

	char info_buf[512];
	read_disk(info_buf, 1);

	char buf[64];
	memset(buf, 0, 64);
	swap_pairs_bytes(&info_buf[20], buf, 20);	
	printf("SN is %s\n", buf);

	// write_disk("Halou World", 1);  // Couldn't write to disk directly.	
	// write_sector(&disks[1], 0, "Halou World", 1);
}

/* TODO: Taking measures to guarantee concurrect security. */
static void _read_sector(struct disk *disk, unsigned int lba, char *buf, unsigned int cnt)
{
	choose_disk(disk);
	choose_sector(disk, lba, cnt);
	outb(hd::cmd_reg, hd::read);

	if (!wait_disk()) {
		printf("Read from sector failed.\n");
		return;
	}

	read_disk(buf, cnt);
}

/* TODO: Taking measures to guarantee concurrect security. */
static void _write_sector(struct disk *disk, unsigned int lba, char *buf, unsigned int cnt)
{
	choose_disk(disk);
	choose_sector(disk, lba, cnt);
	outb(hd::cmd_reg, hd::write);

	if (!wait_disk()) {
		printf("Write to sector failed.\n");
		return;
	};

	write_disk(buf, cnt);
}

void read_sector(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt)
{
	_read_sector(&disks[disk_nr], lba, buf, cnt);
}

void write_sector(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt)
{
	_write_sector(&disks[disk_nr], lba, buf, cnt);
}

