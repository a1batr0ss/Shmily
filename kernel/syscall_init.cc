#include <stdio.h>
#include "interrupt.h"
#include "ipc.h"

extern "C" void syscall_handler(unsigned int type, Message *msg, unsigned int target)
{
	if (type & ipc::SEND) {
		msg->send(target);
	}

	if (type & ipc::RECV) {
		msg->receive(target);
	}

}

