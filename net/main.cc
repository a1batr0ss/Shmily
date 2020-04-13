#include <all_syscall.h>
#include <ipc_glo.h>
#include <stdio.h>
#include <syscall.h>
#include <indirect_ring_buffer.h>
#include "arp.h"
#include "net.h"
#include "icmp.h"
#include "udp.h"
#include "dns.h"

int main()
{
	init_net();
	IndirectRingBuffer *net_buf = (IndirectRingBuffer*)get_net_buffer();

	while (1) {
		if (net_buf->is_empty())
			continue;

		char *data = net_buf->get_first_used_buffer();
		resolve_packet((unsigned char*)data);
	}

	return 0;
}

