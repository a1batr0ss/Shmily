#include <all_syscall.h>
#include <stdio.h>
#include "net.h"

char mac_addr[6];

void init_net()
{
	get_mac_addr(mac_addr);
}

