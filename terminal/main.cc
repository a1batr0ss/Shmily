#include <stdio.h>
#include <print.h>
#include <syscall.h>
#include <string.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include "terminal.h"

int main()
{
	struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	cls();
	set_cursor(0);
	init_terminal();

	char line[64] = {0};
	unsigned char line_idx = 0;

	while (1) {
		if (ringbuffer_is_empty(keyboard_buf))
			continue;

		unsigned char ch = ringbuffer_get(keyboard_buf);
		putchar(ch);

		if (13 != ch)  /* Not '\n' (10) */
			line[line_idx++] = ch;
		else {
			handler_input(line);
			memset(line, 0, 64);
			line_idx = 0;
			print_shell();
		}
	}

	return 0;
}



