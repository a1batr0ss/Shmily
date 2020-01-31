#ifndef __NET_ARP_H__
#define __NET_ARP_H__

struct arp_packet {
	unsigned short hw_type;
	unsigned short req_protocol;
	unsigned char hw_addr_size;
	unsigned char protocol_size;
	unsigned short opcode;
	unsigned char src_mac_addr[6];
    unsigned char src_ip_addr[4];
	unsigned char dst_mac_addr[6];	
	unsigned char target_ip[4];
};

namespace {
	namespace arp {
		unsigned char mac_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		unsigned char mac_zero[6] = {0, 0, 0, 0, 0, 0};
		unsigned short hw_type = 0x0001;
		unsigned short ipv4 = 0x0800;
		unsigned char hw_addr_size = 0x06;
		unsigned char proto_size = 0x04;
		unsigned short req_opcode = 0x0001;
		unsigned short rsp_opcode = 0x0002;
	};
}

void arp_request(unsigned char *target_ip);

#endif

