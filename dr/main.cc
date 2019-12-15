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
		case dr::READ:
		{
			unsigned int disk_nr = con.con_1;
			unsigned int lba = con.con_2;
			unsigned int str_uint = con.con_3;
			unsigned int cnt = con.con_4;
			char *str = (char*)str_uint;
			read_sector(disk_nr, lba, str, cnt);

			break;
		}
		case dr::WRITE:
		{
			unsigned int disk_nr = con.con_1;
			unsigned int lba = con.con_2;
			unsigned int str_uint = con.con_3;
			unsigned int cnt = con.con_4;
			char *str = (char*)str_uint;
			printf("%d %s %x %x %x", disk_nr, str, lba, str, cnt);
			write_sector(disk_nr, lba, str, cnt);

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

