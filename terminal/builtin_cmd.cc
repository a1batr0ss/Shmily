#include <ipc_glo.h>
#include <global.h>
#include <syscall.h>
#include <all_syscall.h>
#include <stdio.h>
#include <string.h>
#include "user.h"

void ps()
{
	Message msg;
	struct _context con;
	msg.reset_message(kr::CMD_PS, con);
	msg.send_then_recv(all_processes::KR);

	return;
}

void meminfo()
{
	Message msg;
	struct _context con;
	msg.reset_message(mm::CMD_MEMINFO, con);
	msg.send_then_recv(all_processes::MM);

	return;
}

void pwd()
{
	Message msg;
	struct _context con;
	msg.reset_message(fs::CMD_PWD, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

bool cd(char *path="/")
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)path;
	msg.reset_message(fs::CMD_CD, con);
	msg.send_then_recv(all_processes::FS);

	return (bool)(msg.get_context().con_1);
}

void ls(char *path=NULL)
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)path;
	msg.reset_message(fs::CMD_LS, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

void cat(char *filename)
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)filename;
	msg.reset_message(fs::CMD_CAT, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

void cp_file(char *src, char *dst)
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)src;
	con.con_2 = (unsigned int)dst;
	msg.reset_message(fs::CMD_CPFILE, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

void mv_file(char *src, char *dst)
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)src;
	con.con_2 = (unsigned int)dst;
	msg.reset_message(fs::CMD_MVFILE, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

void chmod(char *new_mode, char *path)
{
	unsigned int mode = new_mode[1] - '0';
	mode += ((new_mode[0] - '0') << 3);

	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)path;
	con.con_2 = mode;
	msg.reset_message(fs::CMD_CHMOD, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

void chown(char *path, char *new_owner)
{
	int uid = username2uid(new_owner);
	if (-1 == uid)
		return;

	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)path;
	con.con_2 = uid;
	msg.reset_message(fs::CMD_CHOWN, con);
	msg.send_then_recv(all_processes::FS);

	return;
}

/* First show earlier login event. */
void last()
{
	int fd = open("/var/login.log");
	char buf[64] = {0};

	while (!eof(fd)) {
		readline(fd, buf);
		printf("%s", buf);
		memset(buf, 0, 64);
	}

	close(fd);
}

void halt()
{
	Message msg;
	struct _context con;
	msg.reset_message(kr::CMD_HALT, con);
	msg.send(all_processes::KR);  /* Not wait. As it can't return here. */
}

void power_off()
{
	Message msg;
	struct _context con;
	msg.reset_message(kr::CMD_POWEROFF, con);
	msg.send(all_processes::KR);  /* Not wait. As it can't return here. */
}

