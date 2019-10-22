#include <global.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "print.h"
#include "memory.h"

void f1(void *args);
void f2(void *args);

int main()
{
    enable_paging();

    init_mempool();
    init_intr();
    deal_init_process();

    start_process("proc1", 32, f1, NULL, (struct pcb*)0x90000);
    start_process("proc2", 39, f2, NULL, (struct pcb*)0x91000);
    start_process("proc3", 39, f2, NULL, (struct pcb*)0x92000);

    /* enable_intr(); */

    while (1);

    return 0;
}

void f1(void *args)
{
    while (1) {
        putstring("In f1");
    }
}

void f2(void *args)
{
    while (1) {
        putstring("In f2");
    }
}
