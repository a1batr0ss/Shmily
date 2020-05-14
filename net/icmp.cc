#include <all_syscall.h>
#include <stdio.h>
#include "icmp.h"
#include "ip.h"
#include "ethernet.h"

bool ICMPFactory::need_output = false;

void ICMPFactory::format_packet(unsigned char *data, unsigned char type)
{
	struct icmp_packet *pkt = (struct icmp_packet*)data;
	pkt->type = type;
	pkt->code = 0;
	pkt->checksum = 0;

	/* Set to zero tentatively. */
	pkt->identifier = 0;
	pkt->sequence_num = 0;
	for (int i=0; i<8; i++)
		pkt->timestamp[i] = 0;

	pkt->data[0] = 0x33;
	pkt->data[1] = 0x2b;
	pkt->data[2] = 0x04;
	pkt->data[3] = 0;
	pkt->data[4] = 0;
	pkt->data[5] = 0;
	pkt->data[6] = 0;
	pkt->data[7] = 0;
	for (int i=0x10; i<0x37; i++)
		pkt->data[i-8] = i;

	pkt->checksum = generate_checksum((unsigned short*)data, sizeof(struct icmp_packet));
}

void ICMPFactory::resolve_packet(unsigned char *data)
{
	struct icmp_packet *icmp_pkt = (struct icmp_packet*)data;

	if (icmp::ICMP_REQUEST == icmp_pkt->type) {
		/* No test. */
	} else {
		if (need_output)
			print_reply(icmp_pkt);
	}
}

void ICMPFactory::print_reply(struct icmp_packet *pkt)
{
	printf("ICMP reply seq=%x\n", pkt->sequence_num);
}

void ICMPFactory::request(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip, unsigned char *target_mac_addr)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct icmp_packet));

	EthernetFactory::format_packet(data, mac_addr, target_mac_addr, frame::next_is_ip);
	IPv4Factory::format_packet(data + sizeof(struct eth_packet), ip_addr, target_ip, 0, 0x1, sizeof(struct ip_packet) + sizeof(struct icmp_packet));
	format_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), 0x08);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct icmp_packet);
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);

	need_output = true;
}

void ICMPFactory::reply(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_ip, unsigned char *target_mac_addr)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct icmp_packet));

	EthernetFactory::format_packet(data, mac_addr, target_mac_addr, frame::next_is_ip);
	IPv4Factory::format_packet(data + sizeof(struct eth_packet), ip_addr, target_ip, 0, 0x1, sizeof(struct ip_packet) + sizeof(struct icmp_packet));
	format_packet(data + sizeof(struct eth_packet) + sizeof(ip_packet), 0x0);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct icmp_packet);
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

