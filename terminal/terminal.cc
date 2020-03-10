#include <global.h>
#include <stdio.h>
#include <print.h>
#include <string.h>
#include <builtin_cmd.h>
#include <ring_buffer.h>
#include <all_syscall.h>
#include <cmos.h>
#include <time.h>
#include "terminal.h"
#include "user.h"

Terminal::Terminal(struct ring_buffer *rb)
{
	memset(this->cur_user, 0, 32);
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
	while (!login()) ;
}

void Terminal::start()
{
	user_login();

	record_to_log();

	init_screen();

	run();
}

void Terminal::record_to_log()
{
	char record[32] = {0};
	unsigned long long time_n = get_current_time();
	struct time t = num2time(time_n);
	sprintf(record, "User: %s Date: %d-%d-%d %d:%d\n", cur_user, t.year, t.month, t.day, t.hour, t.minute);

	/* Write to disk. */
	unsigned int fd = open("/var/login.log");
	write(fd, record, strlen(record), file_io::APPEND);
	close(fd);
	return;
}

/* Bad code. */
bool Terminal::login()
{
	bool is_passwd = false;
	char username[32] = {0};
	char password[32] = {0};
	char *login_str = username;
	unsigned char idx = 0;

	print_login_interface();
	set_cursor(6);
	memset(cur_user, 0, 32);

	while (1) {
		if (ringbuffer_is_empty(this->keyboard_buf))
			continue;

		unsigned char ch = ringbuffer_get(this->keyboard_buf);

		if ('\b' == ch)
			idx--;

		if ((13 == ch) && (false == is_passwd)) {
			is_passwd = true;
			login_str = password;
			idx = 0;
			set_cursor(90);
			continue;
		} else if ((13 == ch) && (true == is_passwd)) {
			bool ret = user_check(username, password);
			return ret;
		}

		if (is_passwd && '\b' != ch)
			putchar('*');
		else {
			putchar(ch);
			this->cur_user[idx] = ch;
		}

		login_str[idx++] = ch;
	}
}

void Terminal::print_shell()
{
	printf("%s@localhost:%s$", this->cur_user, this->cur_dir);
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

void Terminal::handle_input()
{
	format_input();

	if (strcmp(this->argv[0], "ps"))
		ps();
	else if (strcmp(this->argv[0], "pwd"))
		pwd();
	else if (strcmp(this->argv[0], "cd")) {
		if (cd(this->argv[1])) {
			memset(this->cur_dir, 0, 64);
			strcpy(this->cur_dir, argv[1]);  /* Just for Absolute path. */
		}
	}
	else if (strcmp(this->argv[0], "ls"))
		ls(this->argv[1]);
	else if (strcmp(this->argv[0], "mkdir"))
		mkdir(this->argv[1]);
	else if (strcmp(this->argv[0], "rmdir"))
		rmdir(this->argv[1]);
	else if (strcmp(this->argv[0], "meminfo"))
		meminfo();
	else if (strcmp(this->argv[0], "cat"))
		cat(this->argv[1]);
	else if (strcmp(this->argv[0], "useradd"))
		useradd(this->argv[1], this->argv[2]);
	else
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
