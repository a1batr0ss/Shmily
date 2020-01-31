#include <all_syscall.h>
#include "ne2k.h"
#include "arp.h"

int main()
{
	init_ne2k();

	unsigned char t_ip[4] = {192, 168, 11, 33};
	arp_request(t_ip);

	while (1);

	return 0;
}

