#ifndef __INCLUDE_NET_H__
#define __INCLUDE_NET_H__

/* Just a type definition, without any actions, even the constructor, elements is directly assigned. */
class TcpSocket {
  public:
	unsigned short src_port;
	// unsigned char src_ip[4];
	unsigned int src_ip;
	unsigned short dst_port;
	// unsigned char dst_ip[4];
	unsigned int dst_ip;
};

#endif

