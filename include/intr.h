#ifndef __INCLUDE_INTR_H__
#define __INCLUDE_INTR_H__

typedef void (*intr_handler)();

void init_intr();
void enable_intr();
void disable_intr();
void register_intr_handler(unsigned int nr, intr_handler handler);

#endif

