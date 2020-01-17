#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include "fs.h"

int main()
{
	void* ret = malloc(10);
	printf("In fs malloc memory space is %x.\n", ret);
	free(ret);
	printf("end free.\n");
	ret = malloc(10);
	printf("In fs malloc memory space is %x.\n", ret);
	// Message msg(all_processes::FS);
	Message msg;
	struct _context con;
	con.con_1 = 1;

	msg.reset_message(dr::IDEN, con);
	msg.send(all_processes::DR);

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

