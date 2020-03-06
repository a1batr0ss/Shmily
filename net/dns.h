#ifndef __NET_DNS_H__
#define __NET_DNS_H__

struct dns_header {
	unsigned short trans_id;
	unsigned short flags;
	unsigned short questions;
	unsigned short answer_rrs;
	unsigned short authority_rrs;
	unsigned short additional_rrs;
	unsigned char queries[1];
} __attribute__((packed));

namespace {
	namespace dns {
		const unsigned short STAND_QUERY = 0x0100;
		const unsigned short DNS_PORT = 53;
		const unsigned short A = 0x1;
		const unsigned short IN = 0x1;
	};
};

void send_dns_packet(unsigned char *target_mac_addr, unsigned char *target_ip, const char *url);

#endif

