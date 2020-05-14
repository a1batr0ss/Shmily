#include "arp.h"
#include "ip.h"
#include "byte_order.h"
#include "net.h"

unsigned short IPv4Factory::identifier = 0;

unsigned short generate_checksum(unsigned short *pkt, unsigned int len);

void IPv4Factory::format_packet(unsigned char *data, unsigned char *ip_addr, unsigned char *target_ip, unsigned short ident, unsigned char next_proto, unsigned short total_len)
{
	struct ip_packet *pkt = (struct ip_packet*)data;

	pkt->version = 0x4;
	pkt->header_len = 0x5;
	pkt->diff_services = 0x0;
	pkt->total_len = swap_word(total_len);
	pkt->identification = swap_word(0x4455);
	identifier++;

	pkt->flags = swap_word(ip::no_fragment);
	// pkt->ttl = ip::default_ttl;
	pkt->ttl = 64;
	pkt->next_protocol = next_proto;
	pkt->header_checksum = 0;

	for (int i=0; i<4; i++) {
		pkt->src_ip[i] = ip_addr[i];
		pkt->dst_ip[i] = target_ip[i];
	}

	pkt->header_checksum = generate_checksum((unsigned short*)data, pkt->header_len*4);
}

unsigned short generate_checksum(unsigned short *pkt, unsigned int len)
{
	unsigned int checksum = 0;
	while (len > 1)	{
		checksum += *pkt++;
		len -= sizeof(unsigned short);
	}

	if (len)
		checksum += *pkt;

	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum += (checksum >> 16);
	return (unsigned short)(~checksum);
}

