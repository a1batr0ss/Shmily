#include <all_syscall.h>
#include "arp.h"
#include "net.h"

int main()
{
	init_net();

	unsigned char t_ip[4] = {192, 168, 11, 33};
	arp_request(t_ip);

	while (1);

	return 0;
}

