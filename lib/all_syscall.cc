#include <ipc_glo.h>
#include <syscall.h>
#include <stdio.h>
#include "all_syscall.h"

void* malloc(unsigned int cnt)
{
	Message msg;
	struct _context con;
	con.con_1 = cnt;
	msg.reset_message(mm::MALLOC, con);
	msg.send_then_recv(all_processes::MM);

	return (void*)(msg.get_context().con_1);
}

void free(void *addr)
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)addr;
	msg.reset_message(mm::FREE, con);
	msg.send_then_recv(all_processes::MM);
	return;	
}

/********************************************************/

void read_disk(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt)
{
    Message msg;
    struct _context con;
    con.con_1 = disk_nr;
    con.con_2 = lba;
    con.con_3 = (unsigned int)buf;
    con.con_4 = cnt;    
    msg.reset_message(dr::READ, con);
    msg.send_then_recv(all_processes::DR);
    return; 
}

void write_disk(unsigned int disk_nr, unsigned int lba, char *str, unsigned int cnt)
{
    Message msg;
    struct _context con;
    con.con_1 = disk_nr;                                                                                 
    con.con_2 = lba;
    con.con_3 = (unsigned int)str;  
    con.con_4 = cnt;
    msg.reset_message(dr::WRITE, con);
    msg.send(all_processes::DR);
    return;
}

void print_partition_info(unsigned int disk_nr)
{
	Message msg;
	struct _context con;
	con.con_1 = disk_nr;
	msg.reset_message(dr::PRINT_PART, con);
	msg.send_then_recv(all_processes::DR);
	return;
}

unsigned int get_disk(unsigned int disk_nr)
{
    Message msg;
    struct _context con;
    con.con_1 = disk_nr;
    msg.reset_message(dr::ASK_DISK, con);
    msg.send_then_recv(all_processes::DR);

    return msg.get_context().con_1;
}

/**********************************************/

void register_intr_handler(unsigned int intr_num, void *handler)
{
    Message msg;
    struct _context con;
    con.con_1 = intr_num;
    con.con_2 = (unsigned int)handler;
    msg.reset_message(kr::REGR_INTR, con);
    msg.send_then_recv(all_processes::KR);
    return;
}

