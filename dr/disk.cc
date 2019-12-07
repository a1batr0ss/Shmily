#include <global.h>
#include <ipc_glo.h>
#include <io.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include "disk.h"

void disk_handler();
void disk_identify();

void disk_handler()
{
	inb(disk::status_reg);  /* Pretend this interrupt is handled. */

	Message msg(all_processes::INTERRUPT);
	msg.send(all_processes::INTERRUPT);
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
	Message msg(all_processes::DR);
	struct _context con;
	con.con_1 = 3;
	unsigned max_wait_seconds = 30;
	while (max_wait_seconds-=1 > 0) {
		if (inb(disk::status_reg & disk::busy))
			return inb(disk::status_reg) & disk::ready;
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

	insw(disk::data_reg, buf, bytes_cnt/2);
}

void disk_identify()
{
	outb(disk::cmd_reg, disk::identify);

	Message msg(all_processes::INTERRUPT);
	msg.send(all_processes::INTERRUPT);
	
	if (!wait_disk())
		printf("Not ready.\n");

	char info_buf[512];
	read_disk(info_buf, 1);

	char buf[64];
	memset(buf, 0, 64);
	swap_pairs_bytes(&info_buf[20], buf, 20);	
	printf("SN is %s\n", buf);
}

