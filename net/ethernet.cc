#include "ethernet.h"
#include "arp.h"
#include "net.h"
#include "byte_order.h"

void EthernetFactory::format_packet(unsigned char *data, unsigned char *mac_addr, unsigned char *target_mac_addr, unsigned short next_proto)
{
	struct eth_packet *pkt = (struct eth_packet*)data;

	pkt->next_protocol = swap_word(next_proto);
	for (int i=0; i<6; i++) {
		pkt->dst_mac_addr[i] = target_mac_addr[i];
		pkt->src_mac_addr[i] = mac_addr[i];
	}
}

