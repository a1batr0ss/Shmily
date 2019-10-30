#include <io.h>
#include <string.h>
#include "../include/ipc.h"
#include "../include/intr.h"
#include "../include/timer.h"
#include "../include/other_module.h"
#include "disk.h"
#include "print.h"

void disk_handler();
static void disk_identify();
void init_disk();

void disk_driver()
{
	init_disk();

	Message msg(0x92000);
	while (1) {
		msg.receive(0);

		switch (msg.get_type()) {
		case 1:
			disk_identify();
			break;
		}
	}
}

void init_disk()
{
    unsigned char nr_disk =  *(unsigned char*)0x475;
    register_intr_handler(disk::disk_intr_nr, (intr_handler)disk_handler);
}

void disk_handler()
{
	inb(disk::status_reg);  /* Pretend this interrupt is handled. */
}

static bool wait_disk()
{
	unsigned max_wait_seconds = 30;
	while (max_wait_seconds-=1 > 0) {
		if (inb(disk::status_reg & disk::busy))
			return inb(disk::status_reg) & disk::ready;
		else
			sleep_seconds(1);
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

	insw(disk::data_reg, buf, bytes_cnt/2);
}

static void disk_identify()
{
    putstring("disk identify.\n");
	outb(disk::cmd_reg, disk::identify);

	/* TODO: transmit information with interrupt. Don't have a appropriate method. */
	
	if (!wait_disk())
		putstring("Not ready.\n");
	else
		putstring("Ready.\n");

	char info_buf[512];
	read_disk(info_buf, 1);

	char buf[64];
	memset(buf, 0, 64);
	swap_pairs_bytes(&info_buf[20], buf, 20);	
	putstring("SN is ");
	putstring(buf);
}

