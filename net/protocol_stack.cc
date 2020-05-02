#include <stdio.h>
#include "protocol_stack.h"
#include "net.h"
#include "byte_order.h"
#include "network_interface.h"

/* These codes should be re-arrangement. */
void ProtocolStack::handle_packet(NetworkInterface *net_iface, unsigned char *data)
{
	struct eth_packet *pkt_eth = (struct eth_packet*)(data);

	switch (swap_word(pkt_eth->next_protocol)) {
	case _net::ARP:
	{
		struct arp_packet *pkt_arp = (struct arp_packet*)(data + sizeof(struct eth_packet));

		/* No test! */
		/* An arp request */
		if (arp::req_opcode == swap_word(pkt_arp->opcode)) {
			const unsigned char *res_mac_addr = net_iface->get_arp_table().get_ip_addr(pkt_arp->target_ip);

			if (nullptr != res_mac_addr)
				ArpFactory::reply(net_iface->get_mac_addr(), net_iface->get_ip_addr(), pkt_arp->src_mac_addr, (unsigned char*)res_mac_addr, pkt_arp->target_ip);
		} else {
			/* An arp response. */
			/* Without any secure verify. */
			net_iface->get_arp_table().append(pkt_arp->src_mac_addr, pkt_arp->src_ip_addr);
		}
		net_iface->get_arp_table().print_all();
		break;
	}
	case _net::IP:
	{
		struct ip_packet *pkt_ip = (struct ip_packet*)(data + sizeof(eth_packet));

		/* IP layer judge the destination is our host? */
		if (_net::ICMP == pkt_ip->next_protocol) {
			ICMPFactory::resolve_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet));
		}

		break;
	}
	default:
	{
		printf("unknown protocol, type is %x.\n", swap_word(pkt_eth->next_protocol));
	}
	}
}

