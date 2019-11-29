#include <global.h>
#include <stdio.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "sync.h"
#include "timer.h"

void kernel_work();

int main()
{
    enable_paging();
    init_intr();

    deal_init_process();  /* The init process. */
    start_process("idle", 12, (void (*)(void*))idle_process, NULL, (struct pcb*)0x99000);
	start_process("mm", 32, (void (*)(void*))0x20000, NULL, (struct pcb*)0x91000);
	start_process("fs", 32, (void (*)(void*))0x30000, NULL, (struct pcb*)0x92000);
	start_process("dr", 32, (void (*)(void*))0x40000, NULL, (struct pcb*)0x94000);
	start_process("kernel", 32, (void (*)(void*))kernel_work, NULL, (struct pcb*)0x93000);

    enable_intr();

    while (1);

    return 0;
}

void kernel_work()
{
	Message msg(0x93000);

	while (1) {
		msg.receive(0);
		switch (msg.get_type()) {
		case 0:
		{
			unsigned int intr_nr = msg.get_context().con_1;
			unsigned int handler_addr = msg.get_context().con_2;
			printf("will register a interrupt handler. %d %d\n", intr_nr, handler_addr);
			register_intr_handler(intr_nr, (void (*)(void))handler_addr);

			break;
		}
		case 1:
		{
			unsigned int seconds = msg.get_context().con_1;
			printf("will sleep %x seconds.\n", seconds);
			
			/* Not sure it's the best way as the kernel process will sleep.(Others can still work.) */
			sleep_seconds(seconds);
			msg.reply();

			break;
		}
		default:
		{
			printf("default.\n");
		}
		}
	}
}
