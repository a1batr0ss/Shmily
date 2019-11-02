#include "memory.h"
#include "../include/other_module.h"
#include "../include/ipc.h"
#include "print.h"

void memory_manager()
{
    init_mempool();   

    Message msg(all_processes::MM);
    while (1) {
        msg.receive(all_processes::ANY); 

        switch (msg.get_type()) {
        case mm::malloc:
        {
            unsigned int cnt = msg.get_context();
            unsigned int addr = (unsigned int)malloc_page(cnt);
            msg.reset_message(1, addr);
            msg.reply();
            break;
        }
        case mm::free:
        {
            /* As the message have the unique field, so free a page without count temporarily. */
            unsigned int addr = msg.get_context();
            free_page((void*)addr, 1);
            break;
        }
        }
    }
}

