#ifndef __INIT_INTERRUPT_H__
#define __INIT_INTERRUPT_H__

// #define JOIN(front, back) front##back
typedef void (*intr_handler)();

intr_handler real_handlers[256];  /* Can't replace it's type with void*. */

void init_intr();
void enable_intr();
void disable_intr();

#endif

