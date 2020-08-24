#include <global.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <all_syscall.h>
#include <ring_buffer.h>
#include <ipc_glo.h>
#include <net.h>
#include "terminal.h"
#include "user.h"

void test_net_proc();

int main()
{
	// mkdir("/etc");
	// mkfile("/etc/passwd");
	// mkdir("/var");
	// mkfile("/var/login.log");

	// mkdir("/home");
	// useradd("root", "root");
	// useradd("halou", "world");
	create_process((unsigned int)test_net_proc);

	struct ring_buffer *keyboard_buf = (struct ring_buffer*)get_keyboard_buffer();
	Terminal ter1(keyboard_buf);

	// ter1.start();

	return 0;
}

void test_net_proc()
{
	printf("user process wake up.\n");
	TcpSocket skt;
	skt.src_port = 80;
	listen(&skt, 80);

	int ac_res = accept(&skt);
	unsigned char data[100];
	memset((char*)data, 0, 80);
	recv(&skt, data, 80);
	for (int i=0; i<80; i++) {
		printf("%c", data[i]);
	}
	printf("\n");
	char *str = "HTTP/1.1 200 OK\r\nServer: Shmily\r\nContext-Type: text/html\r\n\r\n<html>halou</html>\r\n";
	send_socket(&skt, str, strlen(str));
	close_socket(&skt);
	printf("socket closed.\n");

	while (1);
}

