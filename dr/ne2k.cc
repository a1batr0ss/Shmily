#include <stdio.h>
#include <io.h>
#include <string.h>
#include <all_syscall.h>
#include <syscall.h>
#include <ipc_glo.h>
#include "ne2k.h"
#include "ethernet.h"

char mac_addr[6];
unsigned char next;
unsigned char data_recv[1600];

void out_mac_addr();
void ne2k_handler();

void init_ne2k()
{
	next = ne2k::PAGE_START_DATA + 1;

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

	outb(ne2k::IOBASE + ne2k::INTR_STAT, 0xff);
	outb(ne2k::IOBASE + ne2k::INTR_MASK, 0x0b);

	register_intr_handler(ne2k::NE2K_IRQ, (void*)ne2k_handler);
}

void out_mac_addr()
{
	for (int i=0; i<6; i++) {
		printf("%x", mac_addr[i] & 0xff);  // Output fffffxxx, There might be problem in print(asm).
		if (5 != i)
			printf(":");
	}
}

void send_packet(struct packet &p)
{
	unsigned int size = p.size;
	unsigned char *data = p.data;
	
	outb(ne2k::IOBASE + ne2k::REMOTE_CNT_LOW, size & 0xff);
	outb(ne2k::IOBASE + ne2k::REMOTE_CNT_HIGH, size >> 8);
	outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_LOW, 0);
	outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_HIGH, ne2k::TRANS_BUF);
	outb(ne2k::IOBASE + ne2k::CMD, ne2k::START | ne2k::WRITE);

	for (int i=0; i<size; i++)
		outb(ne2k::IOBASE + ne2k::NE_DATA, data[i]);

	while (0x40 != (inb(ne2k::IOBASE + ne2k::INTR_STAT)));

	outb(ne2k::IOBASE + ne2k::TRANS_PAGE, ne2k::TRANS_BUF);
	outb(ne2k::IOBASE + ne2k::TRANS_CNT_LOW, size & 0xff);
	outb(ne2k::IOBASE + ne2k::TRANS_CNT_HIGH, size >> 8);
	outb(ne2k::IOBASE + ne2k::CMD, ne2k::NODMA | ne2k::TRANS | ne2k::START);
}

void receive_packet()
{
	unsigned char recv_stat = 0;
	unsigned short len = 0;
	unsigned char cur = 0;
	unsigned char next_ = 0;

	outb(ne2k::IOBASE + ne2k::CMD, ne2k::NODMA | ne2k::START | 0x40);
	cur = inb(ne2k::IOBASE + ne2k::CURRENT);
	outb(ne2k::IOBASE + ne2k::CMD, ne2k::START | ne2k::NODMA);

	while (next != cur) {
		outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_HIGH, next);
		outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_LOW, 0x00);
		outb(ne2k::IOBASE + ne2k::REMOTE_CNT_LOW, 0x04);
		outb(ne2k::IOBASE + ne2k::REMOTE_CNT_HIGH, 0x00);

		outb(ne2k::IOBASE + ne2k::CMD, ne2k::READ | ne2k::START);
		
		recv_stat = inb(ne2k::IOBASE + ne2k::NE_DATA);
		next_ = inb(ne2k::IOBASE + ne2k::NE_DATA);
		len = inb(ne2k::IOBASE + ne2k::NE_DATA);
		len += (inb(ne2k::IOBASE + ne2k::NE_DATA) << 8);

		outb(ne2k::IOBASE + ne2k::INTR_STAT, 0x40);

		if ((1==(recv_stat&31)) && (next >= ne2k::PAGE_START_DATA) && (next <= ne2k::PAGE_STOP_DATA) && (len <= 1532)) {
			outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_HIGH, next);
			outb(ne2k::IOBASE + ne2k::REMOTE_ADDR_LOW, 0x04);
			outb(ne2k::IOBASE + ne2k::REMOTE_CNT_LOW, len & 0xff);
			outb(ne2k::IOBASE + ne2k::REMOTE_CNT_HIGH, len>>8);
			outb(ne2k::IOBASE + ne2k::CMD, ne2k::READ | ne2k::START);

			memset((char*)data_recv, 0, 1600);
			for (int i=0; i<len; i++)
				data_recv[i] = inb(ne2k::IOBASE + ne2k::NE_DATA);

			outb(ne2k::IOBASE + ne2k::INTR_STAT, 0x40);
			if (next_ == ne2k::PAGE_START_DATA)
				next = ne2k::PAGE_START_DATA;
			else
				next = next_;
		}

		if (next == ne2k::PAGE_START_DATA)
			outb(ne2k::IOBASE + ne2k::BOUNDARY, ne2k::PAGE_STOP_DATA-1);
		else
			outb(ne2k::IOBASE + ne2k::BOUNDARY, next-1);

		outb(ne2k::IOBASE + ne2k::CMD, ne2k::START | ne2k::NODMA | 0x40);
		cur = inb(ne2k::IOBASE + ne2k::CURRENT);
		outb(ne2k::IOBASE + ne2k::CMD, ne2k::START | ne2k::NODMA);
	}
}

void deal_packet()
{
	/* Notify the NET. */
	Message msg(all_processes::INTERRUPT);
	struct _context con;
	con.con_1 = (unsigned int)data_recv;
	msg.reset_message(net::PKT_ARRIVED, con);
	msg.send(all_processes::NET);
}

void ne2k_handler()
{
	unsigned char val = 0;
	
	outb(ne2k::IOBASE + ne2k::INTR_STAT, ne2k::NODMA | ne2k::START);

	while (0 != (val=inb(ne2k::IOBASE + ne2k::INTR_STAT))) {
		outb(ne2k::IOBASE + ne2k::INTR_STAT, val);

		// printf("ne2k's handler occurred. %x\n", val);
		
		if (val & ne2k::PKT_RECV) {
			/* TODO: read packet. */
			receive_packet();
			deal_packet();
		}

		if (val & ne2k::PKT_TRANS) {
			outb(ne2k::IOBASE + ne2k::CMD, 0);
		}

		if (val & ne2k::DMA_FIN) {
			// printf("DMA_FIN.\n");
			outb(ne2k::IOBASE + ne2k::CMD + 0x07, 0x40);
			outb(ne2k::IOBASE + ne2k::CMD, 0);
		}

		outb(ne2k::IOBASE + ne2k::INTR_STAT, ne2k::NODMA | ne2k::START);
	}
}
