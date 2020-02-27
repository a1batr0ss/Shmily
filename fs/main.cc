#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include "fs.h"
#include "inode.h"
#include "dir.h"
#include "file.h"

int main()
{
	init_fs();

	Message msg;

	while (1) {
		msg.receive(all_processes::ANY);
		struct _context con = msg.get_context();

		switch (msg.get_type()) {
		case fs::MKDIR:
		{
			char *path = (char*)(con.con_1);
			sys_mkdir(path);
			msg.reply();

			break;
		}
		case fs::RMDIR:
		{
			char *path = (char*)(con.con_1);
			sys_rmdir(path);
			msg.reply();

			break;
		}
		case fs::MKFILE:
		{
			char *path = (char*)(con.con_1);
			sys_mkfile(path);
			msg.reply();

			break;
		}
		case fs::RMFILE:
		{
			char *path = (char*)(con.con_1);
			sys_rmfile(path);
			msg.reply();

			break;
		}
		case fs::OPEN_FILE:
		{
			char *path = (char*)(con.con_1);
			int fd = open_file(path);

			struct _context con_ret;
			con_ret.con_1 = fd;
			msg.reset_message(1, con_ret);
			msg.reply();

			break;
		}
		case fs::CLOSE_FILE:
		{
			unsigned int fd = con.con_1;
			close_file(fd);
			msg.reply();

			break;
		}
		case fs::WRITE_FILE:
		{
			unsigned int fd = con.con_1;
			char *str = (char*)(con.con_2);
			unsigned int count = con.con_3;
			write_file(fd, str, count);

			msg.reply();

			break;
		}
		case fs::READ_FILE:
		{
			unsigned int fd = con.con_1;
			char *buf = (char*)(con.con_2);
			unsigned int count = con.con_3;
			unsigned int len = read_file(fd, buf, count);

			struct _context con_ret;
			con_ret.con_1 = len;
			msg.reset_message(1, con_ret);
			msg.reply();

			break;
		}
		case fs::LSEEK_FILE:
		{
			unsigned int fd = con.con_1;
			unsigned int offset = con.con_2;

			lseek_file(fd, offset);

			msg.reply();

			break;
		}
		case fs::IS_EOF:
		{
			unsigned int res = is_eof(con.con_1);

			struct _context con_ret;
			con_ret.con_1 = res;
			msg.reset_message(1, con_ret);
			msg.reply();

			break;
		}
		case fs::CMD_PWD:
		{
			pwd();
			msg.reply();

			break;
		}
		case fs::CMD_CD:
		{
			unsigned int path = con.con_1;
			cd((char*)path);
			msg.reply();

			break;
		}
		case fs::CMD_LS:
		{
			unsigned int path = con.con_1;
			ls((char*)path);
			msg.reply();

			break;
		}
		default:
		{
			printf("fs received unknown message type.\n");
		}
		}
	}

	return 0;
}

