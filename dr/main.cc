#include <ipc_glo.h>
#include <syscall.h>
#include <stdio.h>
#include "keyboard.h"
#include "disk.h"
#include "ne2k.h"
#include "ethernet.h"

int main()
{
	init_keyboard();
	init_disk();
	init_ne2k();

	Message msg(all_processes::DR);	

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
			msg.reply();

			break;
		}
		case dr::WRITE:
		{
			unsigned int disk_nr = con.con_1;
			unsigned int lba = con.con_2;
			unsigned int str_uint = con.con_3;
			unsigned int cnt = con.con_4;
			char *str = (char*)str_uint;
			// printf("%d %s %x %x %x", disk_nr, str, lba, str, cnt);
			write_sector(disk_nr, lba, str, cnt);

			break;
		}
		case dr::ASK_DISK:
		{
			unsigned int disk_nr = con.con_1;
			struct disk *disk = &disks[disk_nr];

			struct _context con_ret;
			con_ret.con_1 = (unsigned int)disk;
			msg.reset_message(1, con_ret);
			msg.reply();

			break;
		}
		case dr::PRINT_PART:
		{
			unsigned int disk_nr = con.con_1;
			print_disk_partition_info(disk_nr);

			msg.reply();

			break;
		}
		case dr::SEND_PKT:
		{
			unsigned int pkt_ = con.con_1;
			struct packet pkt = *(struct packet*)pkt_;

			send_packet(pkt);

			break;
		}
		case dr::GET_MAC:
		{
			struct _context con_ret;
			con_ret.con_1 = (unsigned int)mac_addr;
			msg.reset_message(1, con_ret);
			msg.reply();

			break;
		}
		default:
		{
			printf("dr received default. %d\n", msg.get_type());
		}
		}
	}

	return 0;
}

