#include "../include/other_module.h"
#include "../include/ipc.h"
#include "print.h"

void filesystem_manager()
{
    putstring("filesystem manager.\n");

    Message msg(all_processes::FS, mm::malloc, 1);
    msg.send_then_recv(all_processes::MM);
    putstring("fs received.\n");
    puthex((unsigned int)msg.get_context());
	
    msg.reset_message(dd::open, 0);
    msg.send(all_processes::DR);
	putstring("fs ended.\n");

    while (1) ;
}

