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

unsigned int get_keyboard_buffer()
{
	Message msg;
	struct _context con;
	msg.reset_message(dr::WHERE_KEYBOARD_BUFFER, con);
	msg.send_then_recv(all_processes::DR);

	return msg.get_context().con_1;
}

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
    msg.send_then_recv(all_processes::DR);  /* Write must be sychronized. */
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

void send_packet(unsigned int pkt)
{
    Message msg;
    struct _context con;
    con.con_1 = pkt;
    msg.reset_message(dr::SEND_PKT, con);
    msg.send_then_recv(all_processes::DR);
    return;
}

void get_mac_addr(char *mac_addr)
{
    Message msg;
    struct _context con;
    msg.reset_message(dr::GET_MAC, con);
    msg.send_then_recv(all_processes::DR);

    for (int i=0; i<6; i++)
        mac_addr[i] = ((char*)(msg.get_context().con_1))[i];

    return;
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

unsigned long long get_current_time()
{
    Message msg;
    struct _context con;
    msg.reset_message(kr::GET_TIME, con);
    msg.send_then_recv(all_processes::KR);

    struct _context con_ret = msg.get_context();
   
    unsigned char second = con_ret.con_1 & 0xff;
    unsigned char minute = (con_ret.con_1 >> 8 ) & 0xff;
    unsigned char hour = (con_ret.con_1 >> 16) & 0xff;
    unsigned char day = (con_ret.con_1 >> 24) & 0xff;
    unsigned char month = con_ret.con_2 & 0xff;
    unsigned long long year = con_ret.con_3;

    return (year << 32) | ((month&0xf) << 28) | ((day&0x3f) << 22) |
        ((hour&0x3f) << 16) | (minute << 8) | second;
}

/* ************************************* */

void mkdir(char *path)
{
    Message msg;
    struct _context con;
    con.con_1 = (unsigned int)path;
    msg.reset_message(fs::MKDIR, con);
    msg.send_then_recv(all_processes::FS);  
    return;
}

void rmdir(char *path)
{
    Message msg;
    struct _context con;
    con.con_1 = (unsigned int)path;
    msg.reset_message(fs::RMDIR, con);
    msg.send_then_recv(all_processes::FS);
    return;
}

void mkfile(char *path)
{
    Message msg;
    struct _context con;
    con.con_1 = (unsigned int)path;
    msg.reset_message(fs::MKFILE, con);
    msg.send_then_recv(all_processes::FS);  
    return;

}

void rmfile(char *path)
{
    Message msg;
    struct _context con;
    con.con_1 = (unsigned int)path;
    msg.reset_message(fs::RMFILE, con);
    msg.send_then_recv(all_processes::FS);
    return;
}

int open(char *path)
{
    Message msg;
    struct _context con;
    con.con_1 = (unsigned int)path;
    msg.reset_message(fs::OPEN_FILE, con);
    msg.send_then_recv(all_processes::FS);

    struct _context con_ret = msg.get_context();
    return con_ret.con_1;
}

void close(unsigned int fd)
{
    Message msg;
    struct _context con;
    con.con_1 = fd;
    msg.reset_message(fs::CLOSE_FILE, con);
    msg.send_then_recv(all_processes::FS);
    return;
}

void write(unsigned int fd, char *str, unsigned int count)
{
    Message msg;
    struct _context con;
    con.con_1 = fd;
    con.con_2 = (unsigned int)str;
    con.con_3= count;
    msg.reset_message(fs::WRITE_FILE, con);
    msg.send_then_recv(all_processes::FS);
    return;
}

unsigned int read(unsigned int fd, char *buf, unsigned int count)
{
    Message msg;
    struct _context con;
    con.con_1 = fd;
    con.con_2 = (unsigned int)buf;
    con.con_3 = count;
    msg.reset_message(fs::READ_FILE, con);
    msg.send_then_recv(all_processes::FS);

    struct _context con_ret = msg.get_context();
    return con_ret.con_1;
}

bool eof(unsigned int fd)
{
	Message msg;
	struct _context con;
	con.con_1 = fd;
	msg.reset_message(fs::IS_EOF, con);
	msg.send_then_recv(all_processes::FS);

	return (bool)(msg.get_context().con_1);
}

void lseek(unsigned int fd, unsigned int offset)
{
	Message msg;
	struct _context con;
	con.con_1 = fd;
	con.con_2 = offset;
	msg.reset_message(fs::LSEEK_FILE, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

/* It might be slow, or we can make is as a independent call of fs. */
void readline(unsigned int fd, char *buf)
{
    while (!eof(fd)) {
        read(fd, buf, 1);
        if ('\n' == *buf) {
            *buf = 0;
            break;
        }
        buf++;
    }
    return;
}
