#include <ipc_glo.h>
#include <syscall.h>
#include <stdio.h>
#include "keyboard.h"
#include "disk.h"

int main()
{
	init_keyboard();
	init_disk();
	Message msg(all_processes::DR);	

	/* Although the message is printed, the kernel could not register the interrupt. */

	while (1) {
		msg.receive(all_processes::ANY);
		struct _context con = msg.get_context();
		
		switch (msg.get_type()) {
		case dr::IDEN:
		{
			unsigned char disk_nr = con.con_1;
			disk_identify(disk_nr);
			break;
		}
		default:
		{
			printf("dr received default.\n");
		}
		}
	}

	return 0;
}

