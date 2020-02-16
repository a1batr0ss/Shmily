#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include "fs.h"
#include "inode.h"
#include "dir.h"

int main()
{
	init_fs(); 

	mkdir("/abc");
	mkdir("/abc/def");
	rmdir("/abc/def");
	// rmdir("/abc");

	Message msg;

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

