#include <stdio.h>
#include <print.h>
#include <string.h>
#include <builtin_cmd.h>
#include <ring_buffer.h>
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
	printf("Init done.\n");

	return;
}

void Terminal::init_screen()
{
	cls();
	set_cursor(0);

	print_shell();

	return;
}

void Terminal::print_shell()
{
	printf("%s@localhost:%s$", this->cur_user, this->cur_dir);
}

void Terminal::handle_input()
{
	// printf("get a line %s\n", line);
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

void Terminal::run()
{
	while (1) {
		if (ringbuffer_is_empty(this->keyboard_buf))
			continue;

		unsigned char ch = ringbuffer_get(this->keyboard_buf);
		putchar(ch);

		if (13 != ch)
			this->line[this->line_idx++] = ch;
		else {
			handle_input();
			reset_terminal();
		}
	}
}

