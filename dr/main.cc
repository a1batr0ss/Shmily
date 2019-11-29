#include <ipc_glo.h>
#include <syscall.h>
#include <stdio.h>
#include "keyboard.h"
#include "disk.h"

int main()
{
	Message msg(all_processes::DR);	
	struct _context con;

	/* Register the keyboard handler. */
	con.con_1 = 0x21;
	con.con_2 = (unsigned int)keyboard_handler;
	msg.reset_message(kr::REGR_INTR, con);
	msg.send(all_processes::KR);

	/* Register the disk handler. */
	con.con_1 = 0x2e;
	con.con_2 = (unsigned int)disk_handler;
	msg.reset_message(kr::REGR_INTR, con);
	msg.send(all_processes::KR);

	/* Although the message is printed, the kernel could not register the interrupt. */

	while (1) {
		msg.receive(all_processes::ANY);
		
		switch (msg.get_type()) {
		case dr::IDEN:
		{
			disk_identify();
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

