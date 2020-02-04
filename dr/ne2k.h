#ifndef __NET_NE2K_H__
#define __NET_NE2K_H__

namespace {
	namespace ne2k {
		const unsigned int NE2K_IRQ = 0x2a;

		const unsigned short IOBASE = 0x280;
		const unsigned short CMD = 0x0;
		const unsigned short PAGE_START = 0x1;
		const unsigned short PAGE_STOP = 0x2;
		const unsigned short BOUNDARY = 0x3;
		const unsigned short TRANS_STAT = 0x4;
		const unsigned short TRANS_PAGE = 0x4;
		const unsigned short NCR = 5;
		const unsigned short TRANS_CNT_LOW = 0x5;
		const unsigned short TRANS_CNT_HIGH = 0x6;
		const unsigned short INTR_STAT = 0x7;
		const unsigned short CURRENT = 0x7;
		const unsigned short REMOTE_ADDR_LOW = 0x8;
		const unsigned short CRDMA_LOW = 0x8;
		const unsigned short REMOTE_ADDR_HIGH = 0x9;
		const unsigned short DRDMA_HIGH = 0x9;
		const unsigned short REMOTE_CNT_LOW = 0xa;
		const unsigned short REMOTE_CNT_HIGH = 0xb;
		const unsigned short RECV_STAT = 0xc;
		const unsigned short RECV_CONF = 0xc;
		const unsigned short TRANS_CONF = 0xd;
		const unsigned short FAE_TALLY = 0xd;
		const unsigned short DATA_CONF = 0xe;
		const unsigned short CRC_TALLY = 0xe;
		const unsigned short INTR_MASK = 0xf;
		const unsigned short MISS_PKT_TALLY = 0xf;
		const unsigned short IOPORT = 0x10;

		const unsigned char DCR = 0x58;
		const unsigned char NE_RESET = 0x1f;
		const unsigned char NE_DATA = 0x10;
		const unsigned char TRANS_BUF = 0x40;
		const unsigned char PAGE_START_DATA = 0x46;
		const unsigned char PAGE_STOP_DATA = 0x80;

		const unsigned char START = 0x2;
		const unsigned char TRANS = 0x4;
		const unsigned char READ = 0x8;
		const unsigned char WRITE = 0x10;
		const unsigned char NODMA = 0x20;

		const unsigned char PKT_RECV = 0x01;
		const unsigned char OVERFLOW = 0x20;
		const unsigned char PKT_TRANS = 0x40;
		const unsigned char DMA_FIN = 0x02;
	};
}

extern char mac_addr[6];

void init_ne2k();
void send_packet(struct packet &p);

#endif

