#include <global.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include "terminal.h"
#include "user.h"

int main()
{
	mkdir("/etc");
	mkfile("/etc/passwd");
	mkdir("/var");
	mkfile("/var/login.log");

	mkdir("/home");
	useradd("root", "root");
	useradd("halou", "world");

	struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	Terminal ter1(keyboard_buf);

	ter1.start();

	return 0;
}
