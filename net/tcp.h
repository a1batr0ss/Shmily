#ifndef __NET_TCP_H__
#define __NET_TCP_H__

enum tcp_socket_state {
	CLOSED, LISTENING, SYN_SENT, SYN_RECEIVED,
	ESTABLISHED,
	FIN_WAIT1, FIN_WAIT2, CLOSING, TIME_WAIT,
	CLOSE_WAIT,
	LAST_ACK
};

enum tcp_flag {
	FIN = 0x1,
	SYN = 0x2,
	RST = 0x4,
	PSH = 0x8,
	ACK = 0x10,
	URG = 0x20,
	ECE = 0x40,
	CWR = 0x80,
	NS = 0x100
};

struct tcp_packet {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned char reserved : 4;
	unsigned char head_size : 4;
	unsigned char flags;
	unsigned short window_size;
	unsigned short check_sum;
	unsigned short urgent_pointer;
	unsigned int options;
} __attribute__((packed));

struct tcp_pseudo_header {
	unsigned char src_ip[4];
	unsigned char dst_ip[4];
	unsigned short protocol;
	unsigned short total_len;
} __attribute__((packed));

class TcpSocket {
  public:
	unsigned short src_port;
	unsigned char src_ip[4];
	unsigned short dst_port;
	unsigned char dst_ip[4];

	unsigned int seq_num;
	unsigned int ack_num;

	enum tcp_socket_state state;
};

class NetworkInterface;

class TcpFactory {
  private:
	TcpSocket *sockets[128];  /* 65535 is so large. */
	short int get_free_port();
	NetworkInterface *net_if;

  public:
	TcpFactory(NetworkInterface *net_if_);
	TcpSocket* listen(unsigned short port);
	TcpSocket* connect(unsigned char *dst_ip, unsigned short dst_port);
	void disconnect(TcpSocket *socket);
	void format_packet(unsigned char *data, TcpSocket *socket, unsigned char *payload_data, unsigned int payload_data_size, unsigned char flags);
	void send(TcpSocket *socket, unsigned char *payload_data, unsigned int size, unsigned short flags);

	void resolve_packet(unsigned char *data, unsigned char *remote_ip);
};

#endif

