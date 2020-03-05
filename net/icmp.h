#ifndef __NET_ICMP_H__
#define __NET_ICMP_H__

struct icmp_packet {
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short identifier;
	unsigned short sequence_num;
	unsigned char timestamp[8];
	unsigned char data[48];
} __attribute__((packed));

namespace {
	namespace icmp {
		const unsigned char ICMP_REQUEST = 0x08;
		const unsigned char ICMP_REPLY = 0x0;
	};
};

void icmp_request(unsigned char *target_ip, unsigned char *target_mac_addr);
void icmp_reply(unsigned char *target_ip, unsigned char *target_mac_addr);

#endif

