#include <string.h>
#include "process.h"

struct gdt_desc {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_mid;
	unsigned char attr_low;
	unsigned char limit_high;
	unsigned char base_high;
} __attribute__((packed));

struct tss {
	unsigned int backlink;
	unsigned int *esp0;
	unsigned int ss0;
	unsigned int *esp1;
	unsigned int ss1;
	unsigned int *esp2;
	unsigned int ss2;
	unsigned int cr3;
	unsigned (*eip)(void);
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;
	unsigned int cs;
	unsigned int ss;
	unsigned int ds;
	unsigned int fs;
	unsigned int gs;
	unsigned int ldt;
	unsigned int trace;
	unsigned int io_base;
};

struct tss tss;

#define TSS_ATTR_LOW ((1 << 7) + (00 << 5) + (0 << 4) + 9)
#define TSS_ATTR_HIGH ((1 << 7) + (0 << 6) + (0 << 5) + (0 << 4) + 0)

void update_tss(struct pcb *proc)
{
	tss.esp0 = (unsigned int*)((unsigned int)proc + sizeof(struct pcb));
}

static struct gdt_desc make_gdt_desc(unsigned int desc_addr, unsigned int limit, unsigned char attr_low, unsigned char attr_high)
{
	struct gdt_desc desc;
	desc.limit_low = limit & 0x0000ffff;
	desc.base_low = desc_addr & 0x0000ffff;
	desc.base_mid = ((desc_addr & 0x00ff0000) >>  16);
	desc.attr_low = attr_low;
	desc.limit_high = (((limit & 0x000f0000) >> 16) + attr_high);
	desc.base_high = desc_addr >> 24;
	return desc;
}
void install_tss()
{
	memset((char*)&tss, 0, sizeof(struct tss));
	tss.ss0 = (2 << 3);
	tss.io_base = sizeof(struct tss);

	*((struct gdt_desc*)(0x502 + 56)) = make_gdt_desc((unsigned int)&tss, sizeof(struct tss) - 1, TSS_ATTR_LOW, TSS_ATTR_HIGH);

	asm volatile ("ltr %w0" : : "r" (7 << 3));
}

