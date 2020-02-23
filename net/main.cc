#include <all_syscall.h>
#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include "arp.h"
#include "net.h"
#include "builtin_cmd.h"

int main()
{
	init_net();

	// mkdir("/abc");
	// ps();
	// ls();
	// mkfile("/abc/a.txt");
	// cd("/abc");
	// ls();
	// int fd = open("/abc/a.txt");
	// write(fd, "abcdef", 6);
	// char buf[64] = {0};
	// int l = read(fd, buf, 6);
	// printf("read file %d %s\n", l, buf);
	// close(fd);

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

