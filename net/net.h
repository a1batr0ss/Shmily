#ifndef __NET_NET_H__
#define __NET_NET_H__

#include "ethernet.h"

namespace _net {
	const unsigned short IP = 0x0800;
	const unsigned short ARP = 0x0806;
};

void init_net();
void resolve_packet(unsigned char *data);

#endif

