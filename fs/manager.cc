#include "../include/other_module.h"
#include "../include/ipc.h"
#include "print.h"

void filesystem_manager()
{
    putstring("filesystem manager.\n");

    Message msg(0x90000, 1, 213);
    msg.send_then_recv(0x91000);
    putstring("fs received.\n");
    puthex((unsigned int)msg.get_context());
	
	msg.reset_message(1, 1234);
	// msg.send_then_recv(0x92000);
	msg.send(0x92000);
	putstring("fs ended.\n");

    while (1) ;
}

