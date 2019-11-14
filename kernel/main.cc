#include <global.h>
#include <stdio.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "sync.h"

int main()
{
    enable_paging();
    init_intr();

    deal_init_process();  /* The init process. */
    start_process("mm", 32, (void (*)(void*))0x20000, NULL, (struct pcb*)0x91000);

    enable_intr();

    while (1)
		printf("In kernel.\n");;

    return 0;
}
