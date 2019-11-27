#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include "fs.h"

int main()
{
	Message msg(0x92000);
	struct _context con;
	msg.reset_message(1, con);
	// printf("fs will send.\n");
	msg.send(0x94000);
	// printf("fs end sending.\n");
	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
    while (1);

	return 0;
}

