#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include "memory.h"

int main()
{
	init_mem();

	Message msg(all_processes::MM);
	while (1) {
		msg.receive(all_processes::ANY);
		printf("mm received message is %d\n", msg.get_context());
		
		switch (msg.get_type()) {
		case mm::MALLOC:
		{
			struct _context con;
			unsigned int cnt = con.con_1;
			unsigned int addr = (unsigned int)malloc(cnt);
			con.con_1 = addr;
			msg.reset_message(1, con);
			msg.reply();
			break;		
		}
		default:
		{
			printf("Unknown message's type.\n");
		}
		}
	}

	return 0;
}

