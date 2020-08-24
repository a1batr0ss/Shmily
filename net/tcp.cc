#include <all_syscall.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <indirect_ring_buffer.h>
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

	/* Initialize the socket. */
	memcpy((char*)(socket->src_ip), (char*)(this->net_if->get_ip_addr()), 4);
	socket->src_port = port;
	socket->state = LISTENING;

	/* The queue of receive data. */
	char *data_buf[10] = {0};
	for (int i=0; i<10; i++)
		data_buf[i] = (char*)malloc(1600);

	socket->recv_buffer.initialize(data_buf);

	socket->bind_proc = 0;	/* Will bind in accept call. */

	this->sockets[port] = socket;
	return socket;
}

/* Client */
TcpSocket* TcpFactory::connect(unsigned char *dst_ip, unsigned short dst_port)
{
	TcpSocket *socket = (TcpSocket*)malloc(sizeof(TcpSocket));
	socket->set_tcp_factory(this);

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
	unsigned char *gateway_mac_addr = this->net_if->get_default_gateway_mac();

	unsigned char *data = (unsigned char*)malloc(sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct tcp_packet) + size);

	EthernetFactory::format_packet(data, mac_addr, gateway_mac_addr, frame::next_is_ip);
	IPv4Factory::format_packet(data + sizeof(struct eth_packet), ip_addr, socket->dst_ip, 0, 0x6, sizeof(struct ip_packet) + sizeof(struct tcp_packet) + size);
	format_packet(data + sizeof(struct eth_packet) + sizeof(struct ip_packet), socket, payload_data, size, flags);

	struct packet p;
	p.size = sizeof(struct eth_packet) + sizeof(struct ip_packet) + sizeof(struct tcp_packet) + size;
	p.data = data;

	send_packet((unsigned int)&p);

	free(data);
	return;
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
	tcp_header->check_sum = 0;	 /* Tentatively. */
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
	memcpy((char*)(check_sum_use + sizeof(struct tcp_pseudo_header) + sizeof(struct tcp_packet)), (char*)payload_data, payload_data_size);

	tcp_header->check_sum = generate_checksum((unsigned short*)check_sum_use, sizeof(struct tcp_pseudo_header) + sizeof(struct tcp_packet) + payload_data_size);

	/* Copy the payload. */
	memcpy((char*)(data + sizeof(struct tcp_packet)), (char*)payload_data, payload_data_size);

	socket->seq_num += payload_data_size;

	free(check_sum_use);
	return;
}

short int TcpFactory::get_free_port()
{
	for (short int i=12; i<128; i++) {
		if (!(this->sockets[i]))
			return i;
	}
	return -1;
}

void TcpFactory::resolve_packet(unsigned char *data, unsigned char *remote_ip, unsigned short payload_len)
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

		socket->seq_num++;
		socket->ack_num = swap_double_word(tcp_header->seq_num) + 1;

		send(socket, 0, 0, ACK);

		break;
	}
	case FIN:
	case FIN | ACK:
	{
		if (ESTABLISHED == socket->state) {
			socket->state = CLOSE_WAIT;
			socket->ack_num++;
			send(socket, 0, 0, ACK);
			send(socket, 0, 0, FIN | ACK);
		} else if (CLOSE_WAIT == socket->state) {
			socket->state = CLOSED;
		} else if ((FIN_WAIT1 == socket->state) || (FIN_WAIT2 == socket->state)) {
			socket->state = CLOSED;
			socket->ack_num++;
			send(socket, 0, 0, ACK);
		} else {
		}

		break;
	}
	case ACK:
	{
		if (SYN_RECEIVED == socket->state) {
			socket->state = ESTABLISHED;

			/* Below code should be packaging. */
			Message msg;
			struct _context con;
			con.con_1 = swap_word(tcp_header->src_port);
			msg.reset_message(1, con);
			msg.send(socket->bind_proc);


			break;
		} else if (FIN_WAIT1 == socket->state) {
			socket->state = FIN_WAIT2;
			break;
		} else if (CLOSE_WAIT == socket->state) {
			socket->state = CLOSED;
			break;
		} else {
			if (ACK != (tcp_header->flags & ACK))
				break;

			/* Enter the default to process data which in ACK packet. So this case without break statement. */
		}

	}
	default:
	{
		unsigned char *data_start = data + tcp_header->head_size * 4;
		unsigned int real_len = 0;

		for (int i=0; i<payload_len-tcp_header->head_size*4; i++) {
			if (0 != data_start[i])
				real_len++;
		}

		if (swap_double_word(tcp_header->seq_num) == socket->ack_num) {
			/* Call application layer to handler the data. */
			/* Put the data received to buffer, prepare for process the fetch it. */
			char *data_buf = socket->recv_buffer.get_first_free_buffer();
			*(unsigned int*)data_buf = real_len;  /* The length of content. */
			data_buf += 4;
			for (int i=0; i<real_len; i++)
				data_buf[i] = data_start[i];

			socket->ack_num += real_len;
			send(socket, 0, 0, ACK);
		} else {
			/* Need to handle some unexpected situations. */
			if (socket->ack_num > swap_double_word(tcp_header->seq_num)) {
				/* Opposite side ask for retransmission. */
			}
		}
	}
	}
}

void TcpSocket::set_tcp_factory(TcpFactory *tcp_factory_)
{
	this->tcp_factory = tcp_factory_;
}

void TcpSocket::send(unsigned char *data, unsigned int size)
{
	if (nullptr == this->tcp_factory)
		return;

	this->tcp_factory->send(this, data, size, ACK);
}

bool TcpFactory::socket_accept(unsigned short port, unsigned int bind_proc)
{
	if ((nullptr != this->sockets[port]) && (LISTENING == this->sockets[port]->state)) {
		this->sockets[port]->bind_proc = bind_proc;
		return true;
	}

	return false;
}

void TcpFactory::recv(unsigned short port, char *buf, unsigned int len)
{
	/* No check of the belong to the right process. */
	char *data_buf = this->sockets[port]->recv_buffer.get_first_used_buffer();
	unsigned int content_len = *(unsigned int*)data_buf;
	data_buf += 4;

	len = len > content_len ? content_len : len;
	memcpy(buf, data_buf, len);
	return;
}

void TcpFactory::send_socket(unsigned short port, char *data, unsigned int len)
{
	TcpSocket *socket = this->sockets[port];
	if (nullptr == socket)
		return;

	send(socket, (unsigned char*)data, len, PSH|ACK);
}

void TcpFactory::close_socket(unsigned short port)
{
	/* TODO: Free the space of recv_buffer and send_buffer. */

	disconnect(this->sockets[port]);

	free(this->sockets[port]);
	this->sockets[port] = nullptr;

	return;
}

