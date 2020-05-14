#include <all_syscall.h>
#include <string.h>
#include <stdio.h>
#include "udp.h"
#include "ip.h"
#include "ethernet.h"
#include "byte_order.h"
#include "network_interface.h"

UdpFactory::UdpFactory(NetworkInterface *net_if_) : net_if(net_if_) {}

UdpSocket* UdpFactory::connect(unsigned char *target_ip, unsigned short target_port)
{
	UdpSocket *socket = (UdpSocket*)malloc(sizeof(UdpSocket));

	socket->udp_factory = this;

	/* Test. */
	unsigned short src_port = 0x7970;
	socket->src_port = src_port;
	socket->dst_port = target_port;
	memcpy((char*)(socket->src_ip), (char*)(this->net_if->get_ip_addr()), 4);
	memcpy((char*)(socket->dst_ip), (char*)(target_ip), 4);

	// this->sockets[src_port] = socket;

	return socket;
}

void UdpFactory::send(UdpSocket *socket, unsigned char *payload_data, unsigned int size
)
{
	unsigned char *mac_addr = this->net_if->get_mac_addr();
	unsigned char *ip_addr = this->net_if->get_ip_addr();
	unsigned char *gateway_mac_addr = this->net_if->get_default_gateway_mac();

	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header) + size);

	EthernetFactory::format_packet(data, mac_addr, gateway_mac_addr, frame::next_is_ip);
	IPv4Factory::format_packet(data + sizeof(struct eth_packet), ip_addr, socket->dst_ip, 0, 0x11, sizeof(struct ip_packet) + sizeof(udp_header) + size);
	format_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), socket, payload_data, size);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct udp_header) + size;
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

void UdpFactory::format_packet(unsigned char *data, UdpSocket *socket, unsigned char *payload_data, unsigned int size)
{
	unsigned char *check_sum_use = (unsigned char*)malloc(sizeof(struct udp_pseudo_header) + sizeof(struct udp_header) + size);
	struct udp_header *pkt = (struct udp_header*)data;

	pkt->src_port = swap_word(socket->src_port);
	pkt->dst_port = swap_word(socket->dst_port);
	pkt->length = swap_word(sizeof(struct udp_header) + size);
	pkt->checksum = 0;

	struct udp_pseudo_header pseudo_header;
	memcpy((char*)(pseudo_header.src_ip), (char*)(socket->src_ip), 4);
	memcpy((char*)(pseudo_header.dst_ip), (char*)(socket->dst_ip), 4);
	pseudo_header.reserve = 0;
	pseudo_header.protocol = 0x11;  /* 17: UDP */
	pseudo_header.total_len = swap_word(sizeof(struct udp_header) + size);

	memcpy((char*)(check_sum_use), (char*)&pseudo_header, sizeof(struct udp_pseudo_header));
	memcpy((char*)(check_sum_use + sizeof(struct udp_pseudo_header)), (char*)data, sizeof(struct udp_header) + size);

	/* Don't forget copy the data to generate checksum. */
	// pkt->checksum = generate_checksum((unsigned short*)check_sum_use, sizeof(struct udp_pseudo_header) + sizeof(struct udp_header) + size);

	for (int i=0; i<4; i++)
		printf("%d-", socket->src_ip[i]);
	printf("\n");
	for (int i=0; i<4; i++)
		printf("%d-", socket->dst_ip[i]);

	/* Copy the payload. */
	memcpy((char*)(data + sizeof(struct udp_header)), (char*)payload_data, size);

	free(check_sum_use);
}

