#include <stdio.h>
#include <io.h>
#include "ne2k.h"

void out_mac_addr();

void init_ne2k()
{
	printf("Init ne2k.\n");

	outb(ne2k::iobase + 0x1f, inb(ne2k::iobase + 0x1f));
	while ((inb(ne2k::iobase + 0x07) & 0x80 ) == 0);

	outb(ne2k::iobase, (1<<5) | 1);
	outb(ne2k::iobase + 0x0e, 0x49);
	outb(ne2k::iobase + 0x0a, 0x0);
	outb(ne2k::iobase + 0x0b, 0x0);
	outb(ne2k::iobase + 0x0f, 0x0);
	outb(ne2k::iobase + 0x07, 0xff);
	outb(ne2k::iobase + 0x0c, 0x20);
	outb(ne2k::iobase + 0x0d, 0x02);
	outb(ne2k::iobase + 0x0a, 32);
	outb(ne2k::iobase + 0x0b, 0x0);
	outb(ne2k::iobase + 0x08, 0x0);
	outb(ne2k::iobase + 0x09, 0x0);
	outb(ne2k::iobase, 0x0a);

	printf("End ne2k.\n");
	out_mac_addr();
}

void out_mac_addr()
{
	char prom[32];

	for (int i=0; i<32; i++)
		prom[i] = inb(ne2k::iobase + 0x10);

	for (int i=0; i<6; i++) {
		printf("%x", prom[i] & 0xff);  /* Output fffffxxx, There might be problem in print(asm). */
		if (5 != i)
			printf(":");
	}
}	

