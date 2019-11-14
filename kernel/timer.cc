#include <io.h>
#include "interrupt.h"
#include "process.h"

unsigned int timer_ticks;

void timer_handler()
{
	timer_ticks++;
    cur_proc->elapsed_ticks++;
    if (0 == cur_proc->ticks)
        schedule();
    else
        cur_proc->ticks--;
}

void init_pit()
{
	register_intr_handler(0x20, timer_handler);

    unsigned int divisor = 1193180 / 100;
    unsigned char low_byte = (unsigned char)(divisor & 0xff);
    unsigned char high_byte = (unsigned char)((divisor>>8) & 0xff);

    outb(0x43, 0x36);  /* repeating mode */
    outb(0x40, low_byte);
    outb(0x40, high_byte);
}

void sleep_seconds(unsigned int seconds)
{
	unsigned int start_time = timer_ticks;
	unsigned int slp_ticks = seconds * 500;  /* Multiply 500, almost sleep n seonds. */

	while ((timer_ticks-start_time) < slp_ticks) 
		proc_yield();
}
