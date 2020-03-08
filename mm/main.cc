#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include "memory.h"

int main()
{
	MemoryManager mm;

	Message msg(all_processes::MM);
	while (1) {
		msg.receive(all_processes::ANY);
		struct _context con = msg.get_context();

		switch (msg.get_type()) {
		case mm::MALLOC:
		{
			struct _context con_ret;
			unsigned int cnt = con.con_1;
			unsigned int addr = (unsigned int)mm.malloc(cnt);

			con_ret.con_1 = addr;
			msg.reset_message(1, con_ret);
			msg.reply();
			break;
		}
		case mm::FREE:
		{
			unsigned int addr = con.con_1;
			mm.free((void*)addr);
			msg.reply();

			break;
		}
		case mm::CMD_MEMINFO:
		{
			mm.print_mem_info();
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

