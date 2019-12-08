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
	msg.send(all_processes::DR);

	con.con_1 = 3;
	msg.reset_message(kr::SLEEP, con);
	msg.send_then_recv(all_processes::KR);
	printf("fs wake up.\n");

	msg.reset_message(mm::MALLOC, con);
	msg.send_then_recv(all_processes::MM);
	/* 
	 * If we want't to get the result, we couldn't use con.con_1, but msg.get_context().con_1. \
	 * As we use value passed in Message's contructor. 
	 */
	printf("malloc memory is at %x.\n", msg.get_context().con_1);

	con.con_1 = msg.get_context().con_1;
	msg.reset_message(mm::FREE, con);
	msg.send_then_recv(all_processes::MM);  /* There is a bug maybe, we couldn't use send simply. */	

	con.con_1 = 3;
	msg.reset_message(mm::MALLOC, con);
	msg.send_then_recv(all_processes::MM);	
	printf("malloc memory is at %x.\n", msg.get_context().con_1);

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

