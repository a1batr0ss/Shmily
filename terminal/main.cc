#include <global.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include "terminal.h"

int main()
{
	mkfile("/passwd");
	int fd = open("/passwd");
	char *passwd = "root:root\nhalou:world\n";
	write(fd, passwd, strlen(passwd), file_io::COVER);
	close(fd);

	mkdir("/var");
	mkfile("/var/login.log");

	struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	Terminal ter1(keyboard_buf);

	ter1.start();

	return 0;
}
