#include <stdio.h>
#include <print.h>
#include <io.h>
#include "interrupt.h"

void sys_halt()
{
	disable_all_intr();

	set_cursor(0);
	cls();
	printf("============= H a l t ===========\n");
	while (1);
}

/* Might reboot. */
void sys_power_off()
{
	disable_all_intr();

	struct idt_ptr wrong_idt_ptr;
	wrong_idt_ptr.limit = 0;
	wrong_idt_ptr.base = 0x99;  /* Set randomly. */

	load_idt((unsigned int)&wrong_idt_ptr);
	asm volatile ("int $0x99"::);  /* Occur a error. */
}

void sys_reboot()
{
	outb(0x64, 0xfe);
}

