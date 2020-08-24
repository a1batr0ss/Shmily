#include <all_syscall.h>
#include <string.h>
#include "ethernet.h"
#include "ip.h"
#include "dns.h"
#include "udp.h"
#include "byte_order.h"

void format_url(char *buf, const char *url)
{
	unsigned char pos = 0;
	unsigned char cnt = 0;
	int i = 0;
	for (; 0!=url[i]; i++) {
		if ('.' != url[i]) {
			cnt++;
			buf[i+1] = url[i];
		}
		else {
			buf[pos] = cnt;
			pos = i + 1;
			cnt = 0;
		}
	}

	buf[i] = url[i-1];
	buf[pos] = cnt;
	buf[i+1] = 0;
}

void init_dns_header(unsigned char *data, const char *url)
{
	struct dns_header *pkt = (struct dns_header*)data;
	pkt->trans_id = 0;
	pkt->flags = swap_word(dns::STAND_QUERY);
	pkt->questions = swap_word(0x1);
	pkt->answer_rrs = swap_word(0x0);
	pkt->authority_rrs = swap_word(0x0);
	pkt->additional_rrs = swap_word(0x0);
	format_url((char*)(pkt->queries), url);

	/* Question type, question class */
	*(unsigned short*)&(pkt->queries[strlen(url)+2]) = swap_word(dns::A);
	*(unsigned short*)&(pkt->queries[strlen(url)+4]) = swap_word(dns::IN);
}

/* +5: 0 at the end of url, question type and question class. */
void send_dns_packet(unsigned char *mac_addr, unsigned char *ip_addr, unsigned char *target_mac_addr, unsigned char *target_ip, const char *url)
{
	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header) + sizeof(struct dns_header) + strlen(url) + 5);

	EthernetFactory::format_packet(data, mac_addr, target_mac_addr, frame::next_is_ip);
	IPv4Factory::format_packet(data + sizeof(struct eth_packet), ip_addr, target_ip, 0, 0x11, sizeof(struct ip_packet) + sizeof(struct udp_header) + sizeof(struct dns_header) + strlen(url) + 5);
	init_udp_header(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), dns::DNS_PORT, sizeof(struct udp_header) + sizeof(struct dns_header) + strlen(url) + 5);
	init_dns_header(data + sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header), url);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header) + sizeof(struct dns_header) + strlen(url) + 5;
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

