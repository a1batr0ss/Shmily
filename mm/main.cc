#include <stdio.h>
#include <syscall.h>
#include "memory.h"

int main()
{
	init_mempool();

	Message msg(0x91000);
	while (1) {
		msg.receive(0);
		printf("mm received message is %d\n", msg.get_context());
		
		switch (msg.get_type()) {
		case 1:
		{
			struct _context con;
			unsigned int cnt = con.con_1;
			unsigned int addr = (unsigned int)malloc_page(cnt);
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

