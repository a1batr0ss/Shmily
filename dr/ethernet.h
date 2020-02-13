#ifndef __DR_ETHERNET_H__
#define __DR_ETHERNET_H__

struct ethernet {
	unsigned char status;
	unsigned char next_page;
	unsigned int length;
	unsigned char dst_mac_addr[6];
	unsigned char src_mac_addr[6];
	unsigned int next_protocol;
	unsigned char packet[1500];
} __attribute__((packed));

struct eth_packet {
	unsigned char dst_mac_addr[6];
	unsigned char src_mac_addr[6];
	short int next_protocol;
} __attribute__((packet));

struct packet {
	unsigned int size;
	unsigned char *data;
	bool status;
	bool is_ready;
	bool is_delete;
	struct packet *next_packet;
} __attribute__((packet));

namespace {
	namespace frame {
		const unsigned int MAX_LEN = 1532;  /* 1526 */
		unsigned short next_is_arp = 0x0806;
	};
}

#endif

