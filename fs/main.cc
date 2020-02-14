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

	printf("/.. is %d\n", dir_is_exists("/.."));
	printf("/a is %d\n", dir_is_exists("/a"));

	Message msg;

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

