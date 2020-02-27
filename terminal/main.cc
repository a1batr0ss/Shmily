#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include "terminal.h"

int main()
{
	char buf[12] = {0};

	mkfile("/a.txt");
	int fd = open("/a.txt");
	write(fd, "halou world", 12);
	lseek(fd, 5);
	read(fd, buf, 7);
	bool is_eof = eof(fd);
	printf("buf is %s %d\n", buf, is_eof);

	// struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	// Terminal ter1(keyboard_buf);

	// ter1.init_screen();
	// ter1.run();

	return 0;
}



