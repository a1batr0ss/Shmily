#include <stdio.h>
#include <all_syscall.h>
#include "arp.h"
#include "ethernet.h"
#include "byte_order.h"
#include "ne2k.h"

extern char mac_addr[6];
unsigned char ip_addr[4] = {192, 168, 22, 44};

void arp_request(unsigned char *target_ip)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct arp_packet));

	struct eth_packet *eth = (struct eth_packet*)data;;
	struct arp_packet *pkt = (struct arp_packet*)(data + sizeof(struct eth_packet));

	eth->next_protocol = swap_word(frame::next_is_arp);

	pkt->hw_type = swap_word(arp::hw_type);
	pkt->req_protocol = swap_word(arp::ipv4);
	pkt->hw_addr_size = arp::hw_addr_size;
	pkt->protocol_size = arp::proto_size;
	pkt->opcode = swap_word(arp::req_opcode);

	for (int i=0; i<6; i++) {
		pkt->src_mac_addr[i] = mac_addr[i];
	    pkt->dst_mac_addr[i] = 0;	

		/* Process link layer at the same time. */
		eth->dst_mac_addr[i] = arp::mac_broadcast[i];
		eth->src_mac_addr[i] = mac_addr[i];
	}

	for (int i=0; i<4; i++) {
		pkt->src_ip_addr[i] = ip_addr[i];
		pkt->target_ip[i] = target_ip[i];
	}

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct arp_packet);
	p.data = data;

	send_packet(p);

	free(data);
}

