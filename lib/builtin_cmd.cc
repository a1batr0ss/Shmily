#include <ipc_glo.h>
#include <global.h>
#include <syscall.h>

void ps()
{
	Message msg;
	struct _context con;
	msg.reset_message(kr::CMD_PS, con);
	msg.send_then_recv(all_processes::KR);

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

void cd(char *path="/")
{
	Message msg;
	struct _context con;
	con.con_1 = (unsigned int)path;
	msg.reset_message(fs::CMD_CD, con);
	msg.send_then_recv(all_processes::FS);

	return;
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

