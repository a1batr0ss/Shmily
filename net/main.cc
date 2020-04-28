#include <all_syscall.h>
#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include <indirect_ring_buffer.h>
#include "arp.h"
#include "net.h"
#include "icmp.h"
#include "udp.h"
#include "dns.h"
#include "route_table.h"

int main()
{
	init_net();
	IndirectRingBuffer *net_buf = (IndirectRingBuffer*)get_net_buffer();

	unsigned char ip_addr[4] = {192, 168, 10, 1};
	arp_request(ip_addr);
	arp_request(ip_addr);

	RouteTable route_tbl;
	route_tbl.set_default_gateway(ip_addr);
	route_tbl.print_all();

	while (1) {
		if (net_buf->is_empty())
			continue;

		char *data = net_buf->get_first_used_buffer();
		resolve_packet((unsigned char*)data);
	}

	return 0;
}

