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
			unsigned int cnt = msg.get_context();
			unsigned int addr = (unsigned int)malloc_page(cnt);
			msg.reset_message(1, addr);
			// printf("mm will reply.\n");
			msg.reply();
			// printf("mm finish replying.\n");
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

