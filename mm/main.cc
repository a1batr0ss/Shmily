#include <stdio.h>
#include <syscall.h>
#include "memory.h"

int main()
{
	init_mempool();

	UserMessage msg(0x91000);
	while (1) {
		msg.recv(0);
		printf("mm received message is %d\n", msg.get_context());
	}

	return 0;
}

