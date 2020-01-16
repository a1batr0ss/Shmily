#include <ipc_glo.h>
#include <syscall.h>
#include <stdio.h>
#include "all_syscall.h"

void* malloc(unsigned int cnt)
{
	Message msg;
	struct _context con;
	con.con_1 = cnt;
	msg.reset_message(mm::MALLOC, con);
	msg.send_then_recv(all_processes::MM);

	return (void*)(msg.get_context().con_1);
}

void free(void *addr)
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)addr;
	msg.reset_message(mm::FREE, con);
	msg.send_then_recv(all_processes::MM);
	return;	
}

