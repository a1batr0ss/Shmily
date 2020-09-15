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
	Message msg(all_processes::NET);

	NetworkManager nm;

	NetworkInterface& net_iface = nm.get_cur_ifaces();

	IndirectRingBuffer *net_buf = (IndirectRingBuffer*)get_net_buffer();

	unsigned char ip_addr[4] = {172, 17, 0, 1};
	ArpFactory::request(net_iface.get_mac_addr(), net_iface.get_ip_addr(), ip_addr);

	ProtocolStack pro_stk(&net_iface);

	TcpFactory *tcp_factory = pro_stk.get_tcp_factory();

	while (1) {
		msg.receive(all_processes::ANY);
		struct _context con = msg.get_context();

		switch (msg.get_type()) {
		case intr::INTR:
		{
			char *data = net_buf->get_first_used_buffer();  /* I think there is a bug, it should wait the next line process end, then increase itself. */

			unsigned short packet_len = *(unsigned short*)data;
			pro_stk.handle_packet((unsigned char*)(data+2), packet_len);

			break;
		}
		case net::SHOW_ARPTBL:
		{
			net_iface.get_arp_table().print_all();

			msg.reply();

			break;
		}
		case net::PING:
		{
			unsigned char ip_addr[4] = {0};
			ip_addr[0] = (con.con_1 & 0xff000000) >> 24;
			ip_addr[1] = (con.con_1 & 0x00ff0000) >> 16;
			ip_addr[2] = (con.con_1 & 0x0000ff00) >> 8;
			ip_addr[3] = con.con_1 & 0x000000ff;

			ICMPFactory::request(net_iface.get_mac_addr(), net_iface.get_ip_addr(), ip_addr, net_iface.get_default_gateway_mac());
			msg.reply();
			break;
		}
		case net::SHOW_IFACES:
		{
			nm.show_all_ifaces();

			msg.reply();

			break;
		}
		case net::CONF_NET:
		{
			/* NetworkManager::get_cur_ifaces() return the reference of current interface. */
			unsigned int ip_uint = con.con_1;
			unsigned int gateway_uint = con.con_2;  /* Unused tentatively. */
			NetworkInterface& net_iface = nm.get_cur_ifaces();
			net_iface.config_iface(ip_uint, gateway_uint);

			// ArpFactory::reply(net_iface.get_mac_addr(), net_iface.get_ip_addr(), g_mac, g_mac, g_ip);  /* The point-to-point is invalid, we should make a broadcast. */
			ArpFactory::reply(net_iface.get_mac_addr(), net_iface.get_ip_addr(), arp::mac_broadcast, arp::mac_broadcast, arp::mac_zero);

			msg.reply();

			break;
		}
		case net::LISTEN:
		{
			unsigned short port = (unsigned short)(con.con_1);
			TcpSocket *socket = tcp_factory->listen(port);

			msg.reply();

			break;
		}
		case net::ACCEPT:
		{
			unsigned short port = (unsigned short)(con.con_1);
			unsigned int bind_proc = msg.get_destination();
			bool res = tcp_factory->socket_accept(port, bind_proc);

			/* Source process will block to wait, the reply will send in protocol stack when expected packet arrived. */

			break;
		}
		case net::SEND:
		{
			unsigned short port = (unsigned short)(con.con_1);
			char *data = (char*)(con.con_2);
			unsigned int len = con.con_3;

			tcp_factory->send_socket(port, data, len);
			msg.reply();

			break;
		}
		case net::RECV:
		{
			unsigned short port = (unsigned short)(con.con_1);
			char *buf = (char*)(con.con_2);
			unsigned int len = con.con_3;

			tcp_factory->recv(port, buf, len);
			msg.reply();

			break;
		}
		case net::CLOSE:
		{
			unsigned short port = (unsigned short)(con.con_1);

			tcp_factory->close_socket(port);
			msg.reply();

			break;
		}
		default:
		{
			printf("Net received unknown message's type. %x from %x\n", msg.get_type(), msg.get_destination());
		}
		}

	}


	while (1) {
		if (net_buf->is_empty())
			continue;

		char *data = net_buf->get_first_used_buffer();  /* I think there is a bug, it should wait the next line process end, then increase itself. */

		unsigned short packet_len = *(unsigned short*)data;
		pro_stk.handle_packet((unsigned char*)(data+2), packet_len);
	}

	return 0;
}

