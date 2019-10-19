#include <global.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"

void f1(void *args);
void f2(void *args);

int main()
{
    enable_paging();
    init_intr();

    enable_intr();

    start_process("proc1", 32, f1, NULL, (struct pcb*)0x90000);
    start_process("proc2", 39, f2, NULL, (struct pcb*)0x91000);
    start_process("proc3", 39, f2, NULL, (struct pcb*)0x92000);

    while (1);

    return 0;
}

void f1(void *args)
{
}

void f2(void *args)
{
}

