#include <global.h>
#include <stdio.h>
#include <print.h>
#include <string.h>
#include <ring_buffer.h>
#include <syscall.h>
#include <ipc_glo.h>
#include <all_syscall.h>
#include <cmos.h>
#include <time.h>
#include "builtin_cmd.h"
#include "terminal.h"
#include "user.h"

Terminal::Terminal(struct ring_buffer *rb)
{
	this->cur_user.uid = -1;
	memset(this->cur_user.username, 0, 32);
	memset(this->cur_dir, 0, 64);
	memset(this->line, 0, 128);
	memset((char*)(this->argv), 0, 9);

	this->keyboard_buf = rb;
	this->line_idx = 0;
	this->argc = 0;

	get_cur_dir(this->cur_dir);

	return;
}

void Terminal::init_screen()
{
	set_cursor(0);
	cls();

	return;
}

void Terminal::print_login_interface()
{
	init_screen();
	/* Show some infomation. */
	printf("user: \n");
	printf("password: \n");
}

void Terminal::user_login()
{
	while (-1 == login()) ;

	if (strcmp(this->cur_user.username, "root"))
		this->cur_dir[0] = '/';
	else {
		/* Assume the home directory is alreay exists. No check. */
		strcpy(this->cur_dir, "/home/");
		strcpy(this->cur_dir + strlen("/home/"), this->cur_user.username);
	}
}

void Terminal::tell_fs()
{
	Message msg;
	struct _context con;

	con.con_1 = this->cur_user.uid;
	msg.reset_message(fs::CUR_USER, con);
	msg.send(all_processes::FS);  /* Not necessary to block. */
}

void Terminal::start()
{
	user_login();

	record_to_log();

	tell_fs();

	init_screen();

	run();
}

void Terminal::record_to_log()
{
	/* Couldn't be so small. like 32B */
	char record[64] = {0};
	unsigned long long time_n = get_current_time();
	struct time t = num2time(time_n);
	sprintf(record, "User: %s Date: %d-%d-%d %d:%d\n", this->cur_user.username, t.year, t.month, t.day, t.hour, t.minute);

	/* Write to disk. */
	unsigned int fd = open("/var/login.log");
	write(fd, record, strlen(record), file_io::APPEND);
	close(fd);
	return;
}

/* Bad code. */
int Terminal::login()
{
	bool is_passwd = false;
	char username[32] = {0};
	char password[32] = {0};
	char *login_str = username;
	unsigned char idx = 0;

	print_login_interface();
	set_cursor(6);

	this->cur_user.uid = -1;
	memset(this->cur_user.username, 0, 32);

	while (1) {
		if (ringbuffer_is_empty(this->keyboard_buf))
			continue;

		unsigned char ch = ringbuffer_get(this->keyboard_buf);

		if ((0 == idx) && ('\b' == ch))
			continue;

		if ('\b' == ch) {
			idx--;
			login_str[idx] = 0;
			putchar('\b');
			continue;
		}

		if ((13 == ch) && (false == is_passwd)) {
			is_passwd = true;
			login_str = password;
			idx = 0;
			set_cursor(90);
			continue;
		} else if ((13 == ch) && (true == is_passwd)) {
			int uid = user_check(username, password);
			this->cur_user.uid = uid;
			return uid;
		}

		if (is_passwd && '\b' != ch)
			putchar('*');
		else {
			putchar(ch);
			this->cur_user.username[idx] = ch;
		}

		login_str[idx++] = ch;
	}
}

void Terminal::print_shell()
{
	printf("%s@localhost:%s$", this->cur_user.username, this->cur_dir);
}


void Terminal::format_input()
{
	/* Change all space and '-' to 0. */
	for (int i=0; 0!=this->line[i]; i++) {
		if ((' ' == this->line[i]) || ('-' == this->line[i]))
				this->line[i] = 0;
	}

	bool is_head = true;
	for (int i=0; i<this->line_idx; i++) {
		while (0 == this->line[i++])
			is_head = true;

		i--;
		if (is_head)
			this->argv[this->argc++] = &(this->line[i]);

		is_head = false;
	}
	this->argc--;
}

void Terminal::exit()
{
	memset(this->cur_dir, 0, 64);

	this->cur_user.uid = 0;  /* Must be 0(root) to read /etc/passwd. (Not -1) */
	memset(this->cur_user.username, 0, 32);
	tell_fs();

	reset_terminal();
	start();
}

void Terminal::path_relative2abs(char *realtive_path, char *abs_path)
{
	if ((0 == realtive_path[0]) || (0 == realtive_path)) {
		strcpy(abs_path, this->cur_dir);  /* Default is cur_dir. */
		return;
	}

	if ('/' == realtive_path[0]) {
		strcpy(abs_path, realtive_path);
		return;
	}

	strcpy(abs_path, this->cur_dir);

	if (strcmp(this->cur_dir, "/")) {
		strcpy(abs_path + strlen(this->cur_dir), realtive_path);
	} else {
		abs_path[strlen(this->cur_dir)] = '/';
		strcpy(abs_path + strlen(this->cur_dir) + 1, realtive_path);
	}
	return;
}

void Terminal::path_double_dot(char *path)
{
	char buf[128] = {0};
	/* Change / to 0.(The end of string.) */
	int l = strlen(path);
	for (int i=0; i<l; i++) {
		if ('/' == path[i])
			path[i] = 0;
	}

	int cnt = 0;  /* The times of return to parent directory. */
	for (int i=l-1; i>=0; i--) {  /* From back to front. */
		if (0 != path[i])
			continue;

		if (strcmp(path+i+1, "..")) {
			*(path+i+1) = 1;  /* Flag. (Not need to copy) */
			cnt++;
		} else {
			if (0 != cnt) {
				*(path+i+1) = 1;
				cnt--;
			}
		}
	}

	/* Copy valid entry. */
	for (int i=0; i<l; i+=strlen(path+i)) {
		buf[i] = '/';
		i++;
		if (1 != path[i])
			strcpy(buf + strlen(buf), path+i);
	}

	strcpy(path, buf);
	path[strlen(buf)] = 0;
	return;
}

void Terminal::format_path(char *path, char *new_path)
{
	path_relative2abs(path, new_path);
	path_double_dot(new_path);
}

void Terminal::handle_input()
{
	char abs_path[64] = {0};
	char abs_path2[64] = {0};
	
	format_input();

	if (strcmp(this->argv[0], "ps"))
		ps();
	else if (strcmp(this->argv[0], "pwd"))
		pwd();
	else if (strcmp(this->argv[0], "cd")) {
		format_path(this->argv[1], abs_path);
		if (cd(abs_path)) {
			memset(this->cur_dir, 0, 64);
			strcpy(this->cur_dir, abs_path);  /* Just for Absolute path. */
		}
	} else if (strcmp(this->argv[0], "ls")) {
		format_path(this->argv[1], abs_path);
		ls(abs_path);
	} else if (strcmp(this->argv[0], "mkdir")) {
		format_path(this->argv[1], abs_path);
		mkdir(abs_path);
	} else if (strcmp(this->argv[0], "rmdir")) {
		format_path(this->argv[1], abs_path);
		rmdir(abs_path);
	} else if (strcmp(this->argv[0], "meminfo"))
		meminfo();
	else if (strcmp(this->argv[0], "cat")) {
		format_path(this->argv[1], abs_path);
		cat(abs_path);
	} else if (strcmp(this->argv[0], "useradd"))
		useradd(this->argv[1], this->argv[2]);
	else if (strcmp(this->argv[0], "userdel")) {
		if (strcmp(argv[1], this->cur_user.username)) {
			printf("Couldn't delete the current user.\n");
			return;
		}
		userdel(this->argv[1]);
	} else if (strcmp(this->argv[0], "cpfile")) {
		format_path(this->argv[1], abs_path);
		format_path(this->argv[2], abs_path2);
		cp_file(abs_path, abs_path2);
	} else if (strcmp(this->argv[0], "mvfile")) {
		format_path(this->argv[1], abs_path);
		format_path(this->argv[2], abs_path2);
		mv_file(abs_path, abs_path2);
	} else if (strcmp(this->argv[0], "last"))
		last();
	else if (strcmp(this->argv[0], "exit"))
		exit();
	else if (strcmp(this->argv[0], "halt"))
		halt();
	else if (strcmp(this->argv[0], "poweroff"))
		power_off();
	else if (strcmp(this->argv[0], "id"))
		printf("user %s, uid %d\n", this->cur_user.username, this->cur_user.uid);
	else if (strcmp(this->argv[0], "chmod")) {
		format_path(argv[1], abs_path);
		chmod(this->argv[1], abs_path);
	} else if (strcmp(this->argv[0], "chown")) {
		format_path(argv[1], abs_path);
		chown(abs_path, this->argv[2]);
	} else
		;
}

void Terminal::reset_terminal()
{
	memset(this->line, 0, 128);
	this->line_idx = 0;
	memset((char*)this->argv, 0, 9);
	this->argc = 0;
	print_shell();

	return;
}

void Terminal::run()
{
	print_shell();
	while (1) {
		if (ringbuffer_is_empty(this->keyboard_buf))
			continue;

		unsigned char ch = ringbuffer_get(this->keyboard_buf);
		if ((0 == this->line_idx) && ('\b' == ch))  /* Avoid to delete the shell's prompt. */
			continue;
		else if ('\b' == ch) {
			this->line_idx--;
			this->line[this->line_idx] = 0;
			putchar('\b');
			continue;
		} else {
			putchar(ch);

			if (13 != ch)
				this->line[this->line_idx++] = ch;
			else {
				handle_input();
				reset_terminal();
			}
		}
	}
}
