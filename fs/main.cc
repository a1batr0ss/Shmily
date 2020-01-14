#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include "fs.h"

int main()
{
	unsigned int ret = malloc(10);
	printf("malloc memory space is %x.\n", ret);
	ret = malloc(10);
	printf("malloc memory space is %x.\n", ret);
	// Message msg(all_processes::FS);
	Message msg;
	struct _context con;
	con.con_1 = 1;
	const char *str = "See how much I love you.";
	con.con_2 = 0;
	con.con_3 = (unsigned int)str;
	con.con_4 = 1;

	msg.reset_message(dr::WRITE, con);
	msg.send(all_processes::DR);

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

