#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include "fs.h"

int main()
{
	Message msg(0x92000);
	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
    while (1);

	return 0;
}

