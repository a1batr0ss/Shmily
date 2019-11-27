#include <syscall.h>
#include <stdio.h>
#include "keyboard.h"
#include "disk.h"

int main()
{
	Message msg(0x94000);	
	struct _context con;

	/* Register the keyboard handler. */
	con.con_1 = 0x21;
	con.con_2 = (unsigned int)keyboard_handler;
	msg.reset_message(0, con);
	msg.send(0x93000);

	/* Register the disk handler. */
	con.con_1 = 0x2e;
	con.con_2 = (unsigned int)disk_handler;
	msg.reset_message(0, con);
	msg.send(0x93000);

	/* Although the message is printed, the kernel could not register the interrupt. */

	while (1) {
		msg.receive(0);
		
		switch (msg.get_type()) {
		case 1:
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

