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
#include "protocol_stack.h"
#include "network_interface.h"
#include "network_manager.h"
#include "tcp.h"

int main()
{
	NetworkManager nm;

	NetworkInterface net_iface = nm.get_cur_ifaces();

	IndirectRingBuffer *net_buf = (IndirectRingBuffer*)get_net_buffer();

	unsigned char ip_addr[4] = {172, 17, 0, 1};
	ArpFactory::request(net_iface.get_mac_addr(), net_iface.get_ip_addr(), ip_addr);

	unsigned char target_mac[6] = {0x02, 0x42, 0xaf, 0xdd, 0xf5, 0x4a};
	ProtocolStack pro_stk(&net_iface);

	TcpFactory *tcp_factory = pro_stk.get_tcp_factory();

	TcpSocket *socket = tcp_factory->listen(80);

	while (1) {
		if (net_buf->is_empty())
			continue;

		char *data = net_buf->get_first_used_buffer();  /* I think there is a bug, it should wait the next line process end, then increase itself. */
		pro_stk.handle_packet((unsigned char*)data);
	}

	return 0;
}

