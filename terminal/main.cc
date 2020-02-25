#include <syscall.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include "terminal.h"

int main()
{
	struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	Terminal ter1(keyboard_buf);

	ter1.init_screen();
	ter1.run();

	return 0;
}



