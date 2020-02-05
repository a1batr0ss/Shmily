#include <global.h>
#include "interrupt.h"
#include "io.h"
#include "print.h"
#include "timer.h"
#include "process.h"

struct idt_entry {
    unsigned short off_low16;
    unsigned short seg_selector;
    unsigned char reserved;
    unsigned char flags;
    unsigned short off_high16;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

namespace interrupt {
    const unsigned int idt_entries_cnt = 256;
};

namespace selector {
    const unsigned short code = 0x8;
    const unsigned short data = 0x16;                            
};

struct idt_entry IDT[256];
struct idt_ptr idt_ptr;
intr_handler real_handlers[256];

extern "C" void intr_handler0();
extern "C" void intr_handler1();
extern "C" void intr_handler2();
extern "C" void intr_handler3();
extern "C" void intr_handler4();
extern "C" void intr_handler5();
extern "C" void intr_handler6();
extern "C" void intr_handler7();
extern "C" void intr_handler8();
extern "C" void intr_handler9();
extern "C" void intr_handler10();
extern "C" void intr_handler11();
extern "C" void intr_handler12();
extern "C" void intr_handler13();
extern "C" void intr_handler14();
extern "C" void intr_handler15();
extern "C" void intr_handler16();
extern "C" void intr_handler17();
extern "C" void intr_handler18();
extern "C" void intr_handler19();
extern "C" void intr_handler20();
extern "C" void intr_handler21();
extern "C" void intr_handler22();
extern "C" void intr_handler23();
extern "C" void intr_handler24();
extern "C" void intr_handler25();
extern "C" void intr_handler26();
extern "C" void intr_handler27();
extern "C" void intr_handler28();
extern "C" void intr_handler29();
extern "C" void intr_handler30();
extern "C" void intr_handler31();
extern "C" void intr_handler32();
extern "C" void intr_handler33();
extern "C" void intr_handler34();
extern "C" void intr_handler35();
extern "C" void intr_handler36();
extern "C" void intr_handler37();
extern "C" void intr_handler38();
extern "C" void intr_handler39();
extern "C" void intr_handler40();
extern "C" void intr_handler41();
extern "C" void intr_handler42();
extern "C" void intr_handler43();
extern "C" void intr_handler44();
extern "C" void intr_handler45();
extern "C" void intr_handler46();
extern "C" void intr_handler47();

extern "C" void syscall_pre_handler();

void init_8259A();
void init_idt();
void set_idt_entry(unsigned char nr, unsigned int off_, unsigned short seg_selector_, unsigned char flags_);
void load_idt(unsigned int p_idtptr);
void register_intr_handler(unsigned int nr, intr_handler handler);

void init_intr()
{
    init_8259A();
    init_idt();
    init_pit();
}

/* can be a variable arguments function to decide which irq should be enabled. */
void init_8259A()
{
    outb(0x20, 0x11);
    outb(0xa0, 0x11);
    outb(0x21, 0x20);  /* irq0: 32 */
    outb(0xa1, 0x28);  /* irq8: 40 */
    outb(0x21, 0x04);
    outb(0xa1, 0x02);
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
    // outb(0x21, 0xfd);  [> Only enable the keyboard interrupt for test, only can test once as our keyboard interrupt handler doesn't fetch data(scancode) from keyboard buffer. <]
    outb(0x21, 0xf8);
    outb(0xa1, 0xbb);
}

void init_idt()
{
    /* set idt ptr. */
    idt_ptr.limit = sizeof(struct idt_entry) * interrupt::idt_entries_cnt - 1;
    idt_ptr.base = (unsigned int)(&IDT);

    /* set idt entries. */
    // for (int i=0; i<interrupt::idt_entries_cnt; i++)

   set_idt_entry(0, (unsigned int)intr_handler0, selector::code, 0x8e); 
   set_idt_entry(1, (unsigned int)intr_handler1, selector::code, 0x8e); 
   set_idt_entry(2, (unsigned int)intr_handler2, selector::code, 0x8e); 
   set_idt_entry(3, (unsigned int)intr_handler3, selector::code, 0x8e); 
   set_idt_entry(4, (unsigned int)intr_handler4, selector::code, 0x8e); 
   set_idt_entry(5, (unsigned int)intr_handler5, selector::code, 0x8e); 
   set_idt_entry(6, (unsigned int)intr_handler6, selector::code, 0x8e); 
   set_idt_entry(7, (unsigned int)intr_handler7, selector::code, 0x8e); 
   set_idt_entry(8, (unsigned int)intr_handler8, selector::code, 0x8e); 
   set_idt_entry(9, (unsigned int)intr_handler9, selector::code, 0x8e); 
   set_idt_entry(10, (unsigned int)intr_handler10, selector::code, 0x8e); 
   set_idt_entry(11, (unsigned int)intr_handler11, selector::code, 0x8e); 
   set_idt_entry(12, (unsigned int)intr_handler12, selector::code, 0x8e); 
   set_idt_entry(13, (unsigned int)intr_handler13, selector::code, 0x8e); 
   set_idt_entry(14, (unsigned int)intr_handler14, selector::code, 0x8e); 
   set_idt_entry(15, (unsigned int)intr_handler15, selector::code, 0x8e); 
   set_idt_entry(16, (unsigned int)intr_handler16, selector::code, 0x8e); 
   set_idt_entry(17, (unsigned int)intr_handler17, selector::code, 0x8e); 
   set_idt_entry(18, (unsigned int)intr_handler18, selector::code, 0x8e); 
   set_idt_entry(19, (unsigned int)intr_handler19, selector::code, 0x8e); 
   set_idt_entry(20, (unsigned int)intr_handler20, selector::code, 0x8e); 
   set_idt_entry(21, (unsigned int)intr_handler21, selector::code, 0x8e); 
   set_idt_entry(22, (unsigned int)intr_handler22, selector::code, 0x8e); 
   set_idt_entry(23, (unsigned int)intr_handler23, selector::code, 0x8e); 
   set_idt_entry(24, (unsigned int)intr_handler24, selector::code, 0x8e); 
   set_idt_entry(25, (unsigned int)intr_handler25, selector::code, 0x8e); 
   set_idt_entry(26, (unsigned int)intr_handler26, selector::code, 0x8e); 
   set_idt_entry(27, (unsigned int)intr_handler27, selector::code, 0x8e); 
   set_idt_entry(28, (unsigned int)intr_handler28, selector::code, 0x8e); 
   set_idt_entry(29, (unsigned int)intr_handler29, selector::code, 0x8e); 
   set_idt_entry(30, (unsigned int)intr_handler30, selector::code, 0x8e); 
   set_idt_entry(31, (unsigned int)intr_handler31, selector::code, 0x8e); 

   /* external interrupt */
   set_idt_entry(32, (unsigned int)intr_handler32, selector::code, 0x8e); 
   set_idt_entry(33, (unsigned int)intr_handler33, selector::code, 0x8e); 
   set_idt_entry(34, (unsigned int)intr_handler34, selector::code, 0x8e); 
   set_idt_entry(35, (unsigned int)intr_handler35, selector::code, 0x8e); 
   set_idt_entry(36, (unsigned int)intr_handler36, selector::code, 0x8e); 
   set_idt_entry(37, (unsigned int)intr_handler37, selector::code, 0x8e); 
   set_idt_entry(38, (unsigned int)intr_handler38, selector::code, 0x8e); 
   set_idt_entry(39, (unsigned int)intr_handler39, selector::code, 0x8e); 
   set_idt_entry(40, (unsigned int)intr_handler40, selector::code, 0x8e); 
   set_idt_entry(41, (unsigned int)intr_handler41, selector::code, 0x8e); 
   set_idt_entry(42, (unsigned int)intr_handler42, selector::code, 0x8e); 
   set_idt_entry(43, (unsigned int)intr_handler43, selector::code, 0x8e); 
   set_idt_entry(44, (unsigned int)intr_handler44, selector::code, 0x8e); 
   set_idt_entry(45, (unsigned int)intr_handler45, selector::code, 0x8e); 
   set_idt_entry(46, (unsigned int)intr_handler46, selector::code, 0x8e); 
   set_idt_entry(47, (unsigned int)intr_handler47, selector::code, 0x8e); 

   set_idt_entry(0x99, (unsigned int)syscall_pre_handler, selector::code, 0x8e); 

   load_idt((unsigned int)(&idt_ptr));
}

void set_idt_entry(unsigned char nr, unsigned int off_, \
                    unsigned short seg_selector_, \
                    unsigned char flags_)
{
    IDT[nr].off_low16 = off_ & 0xffff;
    IDT[nr].seg_selector = seg_selector_;
    IDT[nr].reserved = 0;
    IDT[nr].flags = flags_;
    IDT[nr].off_high16 = (off_ >> 16) & 0xffff;
}

void load_idt(unsigned int p_idtptr)
{
    asm volatile ("movl %0, %%eax \n\t"
                    "lidt (%%eax)":: "r" (p_idtptr) : "memory");
}

extern "C" void general_intr_handler(unsigned int nr)
{
	if (NULL != real_handlers[nr])
	    real_handlers[nr]();
} 

void enable_intr()
{
    asm volatile ("sti":::"memory");
}

void disable_intr()
{
    asm volatile ("cli":::"memory");
} 

void register_intr_handler(unsigned int nr, intr_handler handler)
{
    real_handlers[nr] = handler;
} 

