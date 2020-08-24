#ifndef __NET_UDP_H__
#define __NET_UDP_H__

struct udp_pseudo_header {
	unsigned char src_ip[4];
	unsigned char dst_ip[4];
	unsigned char reserve;  /* Always set to zero. */
	unsigned char protocol;
	unsigned short total_len;
} __attribute__((packed));

struct udp_header {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned short length;
	unsigned short checksum;
} __attribute__((packed));

void init_udp_header(unsigned char *data, unsigned short dst_port, unsigned short length);
void send_udp(unsigned char *mac_addr, unsigned char *ip_addr, unsigned short dst_port, unsigned short length, unsigned char *target_mac_addr, unsigned char *targe_ip);

class UdpFactory;

class UdpSocket {
  public:
	unsigned short src_port;
	unsigned char src_ip[4];
	unsigned short dst_port;
	unsigned char dst_ip[4];

	UdpFactory *udp_factory;
};

class NetworkInterface;

class UdpFactory {
  private:
	UdpSocket *sockets[128];
	NetworkInterface *net_if;

  public:
	UdpFactory(NetworkInterface *net_if_);
	void format_packet(unsigned char *data, UdpSocket *socket, unsigned char *paylaod_data, unsigned int size);
	UdpSocket* connect(unsigned char *target_ip, unsigned short target_port);
	void send(UdpSocket *socket, unsigned char *data, unsigned int size);
};

#endif

