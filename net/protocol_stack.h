#ifndef __NET_PROTOCOL_STACK_H__
#define __NET_PROTOCOL_STACK_H__

#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"

class NetworkInterface;

class ProtocolStack {
  private:
	/*
	 * Unused, as thesse protocol factory just has static methods.
	 * Defined here just for showing what protocols we support in protocol stack.
     */
	EthernetFactory eth_protocol;
	ArpFactory arp_protocol;
	IPv4Factory ipv4_protocol;
	ICMPFactory icmp_protocol;

  public:
	static void handle_packet(NetworkInterface *net_iface, unsigned char *data);
};

#endif

