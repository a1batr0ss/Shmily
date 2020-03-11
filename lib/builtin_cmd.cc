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
