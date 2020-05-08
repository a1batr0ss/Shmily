#ifndef __NET_NET_H__
#define __NET_NET_H__

#include "ethernet.h"

namespace _net {
	const unsigned short IP = 0x0800;
	const unsigned short ARP = 0x0806;

	const unsigned short ICMP = 0x01;
	const unsigned short TCP = 0x06;
};

extern char mac_addr[6];

#endif

