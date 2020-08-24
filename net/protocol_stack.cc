#include <stdio.h>
#include "protocol_stack.h"
#include "net.h"
#include "byte_order.h"
#include "network_interface.h"
#include "udp.h"

ProtocolStack::ProtocolStack(NetworkInterface *net_iface) : tcp_protocol(net_iface), cur_net_if(net_iface) {}

TcpFactory* ProtocolStack::get_tcp_factory()
{
	return &(this->tcp_protocol);
}

void ProtocolStack::handle_packet(unsigned char *data, unsigned short len)
{
	struct eth_packet *pkt_eth = (struct eth_packet*)(data);

	switch (swap_word(pkt_eth->next_protocol)) {
	case _net::ARP:
	{
		struct arp_packet *pkt_arp = (struct arp_packet*)(data + sizeof(struct eth_packet));

		/* No test! */
		/* An arp request */
		if (arp::req_opcode == swap_word(pkt_arp->opcode)) {
			const unsigned char *res_mac_addr = this->cur_net_if->get_arp_table().get_mac_addr(pkt_arp->target_ip);

			if (nullptr != res_mac_addr)
				ArpFactory::reply(this->cur_net_if->get_mac_addr(), this->cur_net_if->get_ip_addr(), pkt_arp->src_mac_addr, pkt_arp->src_mac_addr, pkt_arp->src_ip_addr);
		} else {
			/* An arp response. */
			/* Without any secure verify. */
			this->cur_net_if->get_arp_table().append(pkt_arp->src_mac_addr, pkt_arp->src_ip_addr);
		}
		break;
	}
	case _net::IP:
	{
		struct ip_packet *pkt_ip = (struct ip_packet*)(data + sizeof(struct eth_packet));

		unsigned char *remote_ip = pkt_ip->src_ip;
		/* IP layer judge the destination is our host? */
		if (_net::ICMP == pkt_ip->next_protocol) {
			ICMPFactory::resolve_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet));
		} else if (_net::TCP == pkt_ip->next_protocol) {
			unsigned short payload_len = len - sizeof(struct eth_packet) - sizeof(struct ip_packet);

			this->tcp_protocol.resolve_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), remote_ip, payload_len);
		} else {
		}

		break;
	}
	default:
	{
		// printf("unknown protocol, type is %x.\n", swap_word(pkt_eth->next_protocol));
	}
	}
}

