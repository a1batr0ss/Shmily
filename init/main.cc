#include <global.h>
#include <stdio.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "sync.h"
#include "../include/other_module.h"

int main()
{
    printf("In kernel.\n");
    enable_paging();
    init_intr();

	init_keyboard();

    deal_init_process();  /* The init process. */
    start_process("fm", 32, (void (*)(void*))filesystem_manager, NULL, (struct pcb*)0x90000);
    start_process("mm", 32, (void (*)(void*))memory_manager, NULL, (struct pcb*)0x91000);
    start_process("dr", 32, (void (*)(void*))disk_driver, NULL, (struct pcb*)0x92000);

    enable_intr();

    while (1);

    return 0;
}
