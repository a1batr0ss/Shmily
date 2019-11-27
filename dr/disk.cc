#include "global.h"
#include <io.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include "../include/other_module.h"
#include "disk.h"

void disk_handler();
void disk_identify();

void disk_handler()
{
    /* TODO: Read a byte from register and inform the target. */
    printf("disk interrupt occurred.\n");

	inb(disk::status_reg);  /* Pretend this interrupt is handled. */

	printf("disk interrupt ended.\n");
}

/* Just sleep for a moment. */
static void sleep_seconds_demo(unsigned int sec)
{
	int i, j, k;
	for (i=0; i<sec; i++)
		for (j=0; j<1000; j++)
			for (k=0; k<1000; k++) ;
}

static bool wait_disk()
{
	unsigned max_wait_seconds = 30;
	while (max_wait_seconds-=1 > 0) {
		if (inb(disk::status_reg & disk::busy))
			return inb(disk::status_reg) & disk::ready;
		else
			sleep_seconds_demo(1);
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

void disk_identify()
{
    printf("disk identify.\n");
	outb(disk::cmd_reg, disk::identify);

	/* TODO: transmit information with interrupt. Don't have a appropriate method. */
	
	if (!wait_disk())
		printf("Not ready.\n");
	else
		printf("Ready.\n");

	char info_buf[512];
	read_disk(info_buf, 1);

	char buf[64];
	memset(buf, 0, 64);
	swap_pairs_bytes(&info_buf[20], buf, 20);	
	printf("SN is %s\n", buf);
}

