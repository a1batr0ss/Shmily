#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include "fs.h"

int main()
{
	Message msg(0x92000, 1, 2);
	msg.send(0x91000);
	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context());
    while (1);

	return 0;
}

