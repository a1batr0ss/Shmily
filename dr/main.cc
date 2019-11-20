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

	printf("Registed keyboard handler.\n");

	/* Register the disk handler. */
	con.con_1 = 0x2e;
	con.con_2 = (unsigned int)disk_handler;
	msg.reset_message(0, con);
	msg.send(0x93000);

	printf("Registed disk handler.\n");
	
	while (1) ;

	return 0;
}

