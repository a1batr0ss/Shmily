#include <stdio.h>
#include <io.h>
#include <string.h>
#include <all_syscall.h>
#include "ne2k.h"

char mac_addr[6];

void out_mac_addr();
void ne2k_handler();

void init_ne2k()
{
	printf("Init ne2k.\n");

	outb(ne2k::IOBASE + ne2k::NE_RESET, inb(ne2k::IOBASE + ne2k::NE_RESET));
	while ((inb(ne2k::IOBASE + ne2k::INTR_STAT) & 0x80 ) == 0);

	outb(ne2k::IOBASE + ne2k::INTR_STAT, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD, 0x21);
	outb(ne2k::IOBASE + ne2k::DATA_CONF, ne2k::DCR);
	outb(ne2k::IOBASE + ne2k::REMOTE_CNT_LOW, 0x20);
	outb(ne2k::IOBASE + ne2k::REMOTE_CNT_HIGH, 0);
	outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_LOW, 0);
	outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_HIGH, 0);
	outb(ne2k::IOBASE + ne2k::CMD, ne2k::READ | ne2k::START);
	outb(ne2k::IOBASE + ne2k::RECV_CONF, 0xe);
	outb(ne2k::IOBASE + ne2k::TRANS_CONF, 4);

	char prom[32];

	for (int i=0; i<32; i++)
		prom[i] = inb(ne2k::IOBASE + 0x10);

	/* Get mac address. */
	for (int i=0; i<6; i++)
		mac_addr[i] = prom[i*2];

	outb(ne2k::IOBASE + ne2k::TRANS_PAGE, 0x40);
	outb(ne2k::IOBASE + ne2k::PAGE_START, 0x46);
	outb(ne2k::IOBASE + ne2k::BOUNDARY, 0x46);
	outb(ne2k::IOBASE + ne2k::PAGE_STOP, 0x60);
	outb(ne2k::IOBASE + ne2k::INTR_STAT, 0x1f);
	outb(ne2k::IOBASE + ne2k::CMD, 0x61);
	outb(ne2k::IOBASE + ne2k::CMD, 0x61);
	/* Set the nic's phisical address. */
	outb(ne2k::IOBASE + ne2k::CMD + 0x1, mac_addr[0]);
	outb(ne2k::IOBASE + ne2k::CMD + 0x2, mac_addr[1]);
	outb(ne2k::IOBASE + ne2k::CMD + 0x3, mac_addr[2]);
	outb(ne2k::IOBASE + ne2k::CMD + 0x4, mac_addr[3]);
	outb(ne2k::IOBASE + ne2k::CMD + 0x5, mac_addr[4]);
	outb(ne2k::IOBASE + ne2k::CMD + 0x6, mac_addr[5]);
	outb(ne2k::IOBASE + ne2k::CMD + 0x8, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xf, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xa, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xb, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xc, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xd, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xe, 0xff);
	outb(ne2k::IOBASE + ne2k::CMD + 0xf, 0xff);

	outb(ne2k::IOBASE + ne2k::DATA_CONF, ne2k::DCR);
	outb(ne2k::IOBASE + ne2k::CURRENT, ne2k::PAGE_START_DATA + 1);
	outb(ne2k::IOBASE + ne2k::CMD, 0x22);
	outb(ne2k::IOBASE + ne2k::TRANS_CONF, 0x0);
	outb(ne2k::IOBASE + ne2k::RECV_CONF, 0xc);

	register_intr_handler(ne2k::NE2K_IRQ, (void*)ne2k_handler);
	printf("End ne2k.\n");
}

void out_mac_addr()
{

	for (int i=0; i<6; i++) {
		printf("%x", mac_addr[i] & 0xff);  // Output fffffxxx, There might be problem in print(asm).
		if (5 != i)
			printf(":");
	}
}

void ne2k_handler()
{
	printf("ne2k's handler occured.\n");
}

