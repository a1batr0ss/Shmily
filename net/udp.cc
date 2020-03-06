#include <all_syscall.h>
#include "udp.h"
#include "ip.h"
#include "ethernet.h"
#include "byte_order.h"

unsigned short allocate_port()
{
	static unsigned short port = 49151;
	port++;

	return port;
}

void init_udp_header(unsigned char *data, unsigned short dst_port, unsigned short length)
{
	struct udp_header *pkt = (struct udp_header*)data;
	pkt->src_port = swap_word(allocate_port());
	pkt->dst_port = swap_word(dst_port);
	pkt->length = swap_word(length);
	pkt->checksum = 0;
	pkt->checksum = generate_checksum((unsigned short*)data, length);
}

void send_udp(unsigned short dst_port, unsigned short length, unsigned char *target_mac_addr, unsigned char *target_ip)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header));

	init_ethernet_packet(data, target_mac_addr, frame::next_is_ip);
	init_ipv4_header(data + sizeof(struct eth_packet), target_ip, 0, 0x11, sizeof(struct ip_packet) + sizeof(udp_header));
	init_udp_header(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), dst_port, length);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header);
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

