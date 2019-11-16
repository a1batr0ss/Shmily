#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include "fs.h"

int main()
{
	UserMessage msg(0x92000);
	msg.set_context(5678);
	msg.send(0x91000);
    while (1);

	return 0;
}

