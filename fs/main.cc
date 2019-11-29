#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include "fs.h"

int main()
{
	Message msg(all_processes::FS);
	struct _context con;
	msg.reset_message(dr::IDEN, con);
	// printf("fs will send.\n");
	msg.send(all_processes::DR);
	// printf("fs end sending.\n");

	con.con_1 = 3;
	msg.reset_message(kr::SLEEP, con);
	msg.send_then_recv(all_processes::KR);
	printf("fs wake up.\n");

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

