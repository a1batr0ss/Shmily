#ifndef __NET_UDP_H__
#define __NET_UDP_H__

struct udp_header {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned short length;
	unsigned short checksum;
} __attribute__((packed));

void init_udp_header(unsigned char *data, unsigned short dst_port, unsigned short length);
void send_udp(unsigned char *mac_addr, unsigned char *ip_addr, unsigned short dst_port, unsigned short length, unsigned char *target_mac_addr, unsigned char *targe_ip);

#endif

