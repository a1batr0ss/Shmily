#include <global.h>
#include <all_syscall.h>
#include <stdio.h>
#include "byte_order.h"
#include "net.h"
#include "ethernet.h"
#include "arp.h"

char mac_addr[6];
ArpCacheTable arp_tbl;

void init_net()
{
	get_mac_addr(mac_addr);

	/* Initialize the arp cache table. */
	arp_tbl.set_item(0, (unsigned char*)mac_addr, ip_addr);
	arp_tbl.print_all();
}

/* Only recognize arp and ip protocol in link layer. */
void resolve_packet(unsigned char *data)
{
	struct eth_packet *pkt_eth = (struct eth_packet*)(data);

	switch (swap_word(pkt_eth->next_protocol)) {
	case _net::ARP:
	{
		// printf("An arp packet.\n");

		struct arp_packet *pkt_arp = (struct arp_packet*)(data + sizeof(struct eth_packet));

		/* No test! */
		/* An arp request */
		if (arp::req_opcode == swap_word(pkt_arp->opcode)) {
			const unsigned char *res_mac_addr = arp_tbl.get_ip_addr(pkt_arp->target_ip);

			if (NULL != res_mac_addr)
				arp_response(pkt_arp->src_mac_addr, (unsigned char*)res_mac_addr, pkt_arp->target_ip);
		} else {
			/* An arp response. */
			printf("An arp response.\n");
			/* Without any secure verify. */
			arp_tbl.append(pkt_arp->src_mac_addr, pkt_arp->src_ip_addr);
		}
		arp_tbl.print_all();
		break;
	}
	case _net::IP:
	{
		printf("A ip packet.\n");

		break;
	}
	default:
	{
		printf("unknown protocol, type is %x.\n", swap_word(pkt_eth->next_protocol));
	}
	}
}

