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
#include "udp.h"

int main()
{
	NetworkManager nm;

	NetworkInterface net_iface = nm.get_cur_ifaces();

	IndirectRingBuffer *net_buf = (IndirectRingBuffer*)get_net_buffer();

	unsigned char ip_addr[4] = {172, 17, 0, 1};

	ArpFactory::request(net_iface.get_mac_addr(), net_iface.get_ip_addr(), ip_addr);

	ProtocolStack pro_stk(&net_iface);

	TcpFactory *tcp_factory = pro_stk.get_tcp_factory();

	unsigned char g_mac[6] = {0x0a, 0xcc, 0xd9, 0xef, 0xb3, 0xf8};
	unsigned char g_ip[4] = {172, 17, 0, 1};

	net_iface.get_arp_table().append(g_mac, g_ip);

	ArpFactory::reply(net_iface.get_mac_addr(), net_iface.get_ip_addr(), g_mac, g_mac, g_ip);
	TcpSocket *socket = tcp_factory->connect(ip_addr, 1225);

	while (1) {
		if (net_buf->is_empty())
			continue;

		char *data = net_buf->get_first_used_buffer();  /* I think there will be a bug, it should wait the next line process end, then increase itself. */

		unsigned short packet_len = *(unsigned short*)data;
		pro_stk.handle_packet((unsigned char*)(data+2), packet_len);
	}

	return 0;
}

