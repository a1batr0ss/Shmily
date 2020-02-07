#include <all_syscall.h>
#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include "arp.h"
#include "net.h"

int main()
{
	Message msg(all_processes::NET);

	while (1) {
		msg.receive(all_processes::ANY);
		struct _context con = msg.get_context();

		printf("net received %d.\n", con.con_1);
	}
	
	return 0;
}

