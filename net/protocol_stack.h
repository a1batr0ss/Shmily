#ifndef __NET_PROTOCOL_STACK_H__
#define __NET_PROTOCOL_STACK_H__

#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "tcp.h"

class NetworkInterface;

class ProtocolStack {
  private:
	/*
	 * Unused, as these protocol factory just has static methods.
	 * Defined here just for showing what protocols we support in protocol stack.
	 * except tcp.
     */
	EthernetFactory eth_protocol;
	ArpFactory arp_protocol;
	IPv4Factory ipv4_protocol;
	ICMPFactory icmp_protocol;
	TcpFactory tcp_protocol;

	NetworkInterface *cur_net_if;

  public:
	ProtocolStack(NetworkInterface *net_iface);

	TcpFactory* get_tcp_factory();

	void handle_packet(unsigned char *data, unsigned short len);
};

#endif

