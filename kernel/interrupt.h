#ifndef __KERNEL_INTERRUPT_H__
#define __KERNEL_INTERRUPT_H__

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

typedef void (*intr_handler)();

extern intr_handler real_handlers[256];  /* Can't replace it's type with void*. */
extern unsigned int timer_ticks;

void init_intr();
bool enable_intr();
bool disable_intr();
void set_intr(bool old_status);
void register_intr_handler(unsigned int nr, intr_handler handler);
void disable_all_intr();
void load_idt(unsigned int idt_ptr);

#endif

