#include <global.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "sync.h"
#include "../include/other_module.h"
#include "print.h"

int main()
{
    putstring("In kernel.\n");
    enable_paging();
    init_intr();

    deal_init_process();  /* The init process. */
    start_process("fm", 32, (void (*)(void*))filesystem_manager, NULL, (struct pcb*)0x90000);
    start_process("mm", 32, (void (*)(void*))memory_manager, NULL, (struct pcb*)0x91000);

    enable_intr();

    while (1);

    return 0;
}
