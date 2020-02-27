#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include "terminal.h"

int main()
{
	char buf[16] = {0};
	mkfile("/passwd");
	int fd = open("/passwd");
	char *passwd = "root:root\nhalou:world\n";
	write(fd, passwd, strlen(passwd));
	close(fd);

	struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	Terminal ter1(keyboard_buf);

	ter1.user_login();
	ter1.init_screen();
	ter1.run();

	return 0;
}
