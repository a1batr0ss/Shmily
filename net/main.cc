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

int main()
{
	// init_net();
	// unsigned char host_ip_addr[4] = {192, 168, 10, 10};
	// arp_request(ip_addr);
	// unsigned char mac_addr[6] = {0};
	// get_mac_addr(mac_addr);

	// NetworkInterface net_iface(mac_addr, host_ip_addr);
	NetworkManager nm;

	NetworkInterface net_iface = nm.get_cur_ifaces();

	IndirectRingBuffer *net_buf = (IndirectRingBuffer*)get_net_buffer();

	// for (int i=0; i<6; i++)
		// printf("%x  ", net_iface.get_mac_addr()[i]);

	unsigned char ip_addr[4] = {192, 168, 10, 1};
	// arp_request(ip_addr);
	// arp_request(ip_addr);
	ArpFactory::request(net_iface.get_mac_addr(), net_iface.get_ip_addr(), ip_addr);

	// RouteTable route_tbl;
	// route_tbl.set_default_gateway(ip_addr);
	// route_tbl.print_all();

	unsigned char target_mac[6] = {0xb0, 0xc4, 0x20, 0x00, 0x00, 0x9a};
	// icmp_request(ip_addr, target_mac);
	ICMPFactory::request(net_iface.get_mac_addr(), net_iface.get_ip_addr(), ip_addr, target_mac);
	// ICMPFactory::request(ip_addr, target_mac);

	while (1) {
		if (net_buf->is_empty())
			continue;

		char *data = net_buf->get_first_used_buffer();
		// resolve_packet((unsigned char*)data);
		ProtocolStack::handle_packet(&net_iface, (unsigned char*)data);
	}

	return 0;
}

