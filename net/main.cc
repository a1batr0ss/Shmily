#include <all_syscall.h>
#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include "arp.h"
#include "net.h"
#include "icmp.h"
#include "udp.h"
#include "builtin_cmd.h"

int main()
{
	init_net();

	unsigned char req_ip[4] = {192, 168, 11, 99};
	unsigned char target_mac_addr[6] = {11, 11, 11, 22, 22, 22};
	// icmp_reply(req_ip, target_mac_addr);
	send_udp(53, 0, target_mac_addr, req_ip);

	Message msg(all_processes::NET);

	while (1) {
		msg.receive(all_processes::ANY);
		struct _context con = msg.get_context();

		switch (msg.get_type()) {
		case net::PKT_ARRIVED:
		{
			unsigned char *data = (unsigned char*)(con.con_1);

			resolve_packet(data);

			break;
		}
		default:
		{
			printf("net received unknown message.\n");
		}
		}
	}


	return 0;
}

