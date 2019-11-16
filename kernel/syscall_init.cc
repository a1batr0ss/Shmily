#include <stdio.h>
#include "interrupt.h"
#include "ipc.h"

extern "C" void syscall_handler(unsigned int type, unsigned int source, unsigned int target, unsigned int *p_context)
{
	Message msg(source);
	if (type & ipc::SEND) {
		msg.set_context(*p_context);
		msg.send(target);
	}

	if (type & ipc::RECV) {
		msg.receive(target);
		*p_context = msg.get_context();
	}

}

