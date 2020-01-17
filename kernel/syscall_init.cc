#include <stdio.h>
#include "process.h"
#include "interrupt.h"
#include "ipc.h"

extern "C" void syscall_handler(unsigned int type, Message *msg, unsigned int target)
{
	// printf("=======In syscall_handler=======%x=========%x.\n", msg->get_source(), get_current_proc());
	if (0 == msg->get_source())
		msg->set_source((unsigned int)get_current_proc());

	if (type & ipc::SEND) {
		msg->send(target);
	}

	if (type & ipc::RECV) {
		msg->receive(target);
	}

}

