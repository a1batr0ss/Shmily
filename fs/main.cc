#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include "fs.h"
#include "inode.h"
#include "dir.h"
#include "file.h"

int main()
{
	init_fs(); 

	mkdir("/abc");
	mkdir("/abc/def");
	rmdir("/abc/def");
	mkfile("/abc/a.txt");
	int desc = open_file("/abc/a.txt");
	printf("desc is %d.\n", desc);
	close_file(desc);
	desc = open_file("/abc/a.txt");
	printf("desc is %d.\n", desc);
	rmfile("/abc/a.txt");

	Message msg;

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

