#include <all_syscall.h>
#include <stdio.h>
#include <string.h>
#include "tcp.h"
#include "ethernet.h"
#include "ip.h"
#include "network_interface.h"
#include "byte_order.h"

TcpFactory::TcpFactory(NetworkInterface *net_if_)
{
	for (int i=0; i<128; i++)
		this->sockets[i] = nullptr;

	this->net_if = net_if_;
}

/* Server */
TcpSocket* TcpFactory::listen(unsigned short port)
{
	/* Be occupied. */
	if (nullptr != this->sockets[port])
		return nullptr;

	TcpSocket *socket = (TcpSocket*)malloc(sizeof(TcpSocket));

	if (nullptr == socket)
		return nullptr;

	memcpy((char*)(socket->src_ip), (char*)(this->net_if->get_ip_addr()), 4);
	socket->src_port = port;
	socket->state = LISTENING;

	this->sockets[port] = socket;
}

/* Client */
TcpSocket* TcpFactory::connect(unsigned char *dst_ip, unsigned short dst_port)
{
	TcpSocket *socket = (TcpSocket*)malloc(sizeof(TcpSocket));

	if (nullptr == socket)
		return nullptr;

	unsigned char *src_ip = this->net_if->get_ip_addr();

	int src_port = get_free_port();
	if (-1 == src_port)
		return nullptr;

	this->sockets[src_port] = socket;

	memcpy((char*)(socket->src_ip), (char*)src_ip, 4);
	socket->src_port = src_port;
	memcpy((char*)(socket->dst_ip), (char*)dst_ip, 4);
	socket->dst_port = dst_port;
	socket->state = SYN_SENT;
	socket->seq_num = 0x0;
	socket->ack_num = 0x0;

	send(socket, 0, 0, SYN);

	return socket;
}

void TcpFactory::disconnect(TcpSocket *socket)
{
	socket->state = FIN_WAIT1;
	send(socket, 0, 0, FIN | ACK);
	socket->seq_num++;
}

void TcpFactory::send(TcpSocket *socket, unsigned char *payload_data, unsigned int size, unsigned short flags)
{
	unsigned char *mac_addr = this->net_if->get_mac_addr();
	unsigned char *ip_addr = this->net_if->get_ip_addr();
	/* Temporary. */
	unsigned char target_mac_addr[6] = {0x02, 0x42, 0x82, 0x84, 0x25, 0x95};  /* Must change every time. */

	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct tcp_packet) + size);

	EthernetFactory::format_packet(data, mac_addr, target_mac_addr, frame::next_is_ip);
	IPv4Factory::format_packet(data + sizeof(struct eth_packet), ip_addr, socket->dst_ip, 0, 0x6, sizeof(struct ip_packet) + sizeof(struct tcp_packet) + size);
	format_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), socket, payload_data, size, flags);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct tcp_packet) + size;
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
}

void TcpFactory::format_packet(unsigned char *data, TcpSocket *socket, unsigned char *payload_data, unsigned int payload_data_size, unsigned char flags)
{
	unsigned char *check_sum_use = (unsigned char*)malloc(sizeof(struct tcp_pseudo_header) + sizeof(struct tcp_packet) + payload_data_size);
	struct tcp_packet *tcp_header = (struct tcp_packet*)data;

	tcp_header->src_port = swap_word(socket->src_port);
	tcp_header->dst_port = swap_word(socket->dst_port);
	tcp_header->seq_num = swap_double_word(socket->seq_num);
	tcp_header->ack_num = swap_double_word(socket->ack_num);  /* Tentatively. */
	tcp_header->head_size = sizeof(struct tcp_packet) / 4;
	tcp_header->reserved = 0;
	tcp_header->flags = flags;
	tcp_header->window_size = 0xffff;
	tcp_header->check_sum = 0;   /* Tentatively. */
	tcp_header->urgent_pointer = 0;
	tcp_header->options = (flags & SYN) ? 0xb4050402 : 0;

	/* Calculate the checksum. */
	struct tcp_pseudo_header pseudo_header;
	memcpy((char*)(pseudo_header.src_ip), (char*)(socket->src_ip), 4);
	memcpy((char*)(pseudo_header.dst_ip), (char*)(socket->dst_ip), 4);
	pseudo_header.protocol = 0x0600;  /* TCP */
	pseudo_header.total_len = swap_word(sizeof(struct tcp_packet) + payload_data_size);

	memcpy((char*)(check_sum_use), (char*)(&pseudo_header), sizeof(struct tcp_pseudo_header));
	memcpy((char*)(check_sum_use + sizeof(struct tcp_pseudo_header)), (char*)(data), sizeof(struct tcp_packet) + payload_data_size);

	tcp_header->check_sum = generate_checksum((unsigned short*)check_sum_use, sizeof(struct tcp_pseudo_header) + sizeof(struct tcp_packet) + payload_data_size);

	/* Copy the payload. */
	memcpy((char*)(data + sizeof(struct tcp_packet)), (char*)payload_data, payload_data_size);

	socket->seq_num += payload_data_size;

	free(check_sum_use);
}

short int TcpFactory::get_free_port()
{
	for (short int i=12; i<128; i++) {
		if (!(this->sockets[i]))
			return i;
	}
	return -1;
}

void TcpFactory::resolve_packet(unsigned char *data, unsigned char *remote_ip)
{
	struct tcp_packet *tcp_header = (struct tcp_packet*)data;

	unsigned short dst_port = swap_word(tcp_header->dst_port);
	unsigned short src_port = swap_word(tcp_header->src_port);

	/* Don't have any sockets listening the port.(tcp_header->dst_port) */
	if (nullptr == this->sockets[dst_port])
		return;

	TcpSocket *socket = this->sockets[dst_port];

	switch (tcp_header->flags & (SYN | ACK | FIN)) {
	case SYN:
	{
		if (LISTENING != socket->state)
			return;

		socket->state = SYN_RECEIVED;
		socket->dst_port = src_port;
		memcpy((char*)(socket->dst_ip), (char*)remote_ip, 4);
		socket->ack_num = swap_double_word(tcp_header->seq_num) + 1;
		socket->seq_num = 0;

		send(socket, 0, 0, SYN | ACK);

		socket->seq_num++;

		break;
	}
	case SYN | ACK:
	{
		if (SYN_SENT != socket->state)
			return;

		socket->state = ESTABLISHED;

		socket->ack_num = swap_double_word(tcp_header->seq_num) + 1;

		socket->seq_num++;

		send(socket, 0, 0, ACK);

		break;
	}
	default:
	{
		printf("default %x\n", tcp_header->flags);
	}
	}
}

