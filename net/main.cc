#include <all_syscall.h>
#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include "arp.h"
#include "net.h"

int main()
{
	init_net();

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

