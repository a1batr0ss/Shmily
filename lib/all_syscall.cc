#include <ipc_glo.h>
#include <syscall.h>
#include <stdio.h>
#include "all_syscall.h"

unsigned int malloc(unsigned int cnt)
{
	Message msg;
	struct _context con;
	con.con_1 = cnt;
	msg.reset_message(mm::MALLOC, con);
	msg.send_then_recv(all_processes::MM);

	return msg.get_context().con_1;
}

