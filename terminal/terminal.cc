#include <stdio.h>
#include <print.h>
#include <string.h>
#include <builtin_cmd.h>
#include <ring_buffer.h>
#include <all_syscall.h>
#include "terminal.h"

Terminal::Terminal(struct ring_buffer *rb)
{
	memset(this->cur_user, 0, 32);
	memset(this->cur_dir, 0, 64);
	memset(this->line, 0, 128);
	strcpy(this->cur_user, "shmily");
	strcpy(this->cur_dir, "~");  /* tentatively, it should get from filesystem by message-passing. */
	this->keyboard_buf = rb;
	this->line_idx = 0;

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

/* Bad code. */
bool Terminal::login()
{
	bool is_passwd = false;
	char login_str[64] = {0};
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
			login_str[idx++] = ':';
			set_cursor(90);
			continue;
		} else if ((13 == ch) && (true == is_passwd)) {
			bool ret = user_check(login_str);
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

bool Terminal::user_check(char *login_str)
{
	char buf[64] = {0};
	unsigned int fd = open("/passwd");

	while (!eof(fd)) {
		readline(fd, buf);

		if (strcmp(buf, login_str)) {
			close(fd);
			return true;
		}
		memset(buf, 0, 64);
	}
	close(fd);
	return false;
}

void Terminal::print_shell()
{
	printf("%s@localhost:%s$", this->cur_user, this->cur_dir);
}

void Terminal::handle_input()
{
	if (strcmp(this->line, "ps"))
		ps();
	else if (strcmp(this->line, "pwd"))
		pwd();
	else
		;
}

void Terminal::reset_terminal()
{
	memset(this->line, 0, 64);
	this->line_idx = 0;
	print_shell();

	return;
}

/* TODO: There are some flaw when input '\b' */
void Terminal::run()
{
	print_shell();
	while (1) {
		if (ringbuffer_is_empty(this->keyboard_buf))
			continue;

		unsigned char ch = ringbuffer_get(this->keyboard_buf);
		if ((0 == this->line_idx) && ('\b' == ch))  /* Avoid to delete the shell's prompt. */
			continue;

		putchar(ch);

		if (13 != ch)
			this->line[this->line_idx++] = ch;
		else {
			handle_input();
			reset_terminal();
		}
	}
}
