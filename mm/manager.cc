#include "memory.h"
#include "../include/other_module.h"
#include "../include/ipc.h"
#include "print.h"

void memory_manager()
{
    init_mempool();   

    Message msg(0x91000);
    /* Ready to receive message. */
    while (1) {
        putstring("memory manger.\n");
        msg.receive(0); 
        
        msg.set_context(123);
        puthex((unsigned int)msg.get_context());
        msg.send(0x90000);
    }
}

