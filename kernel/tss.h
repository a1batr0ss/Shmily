#ifndef __KERNEL_TSS_H__
#define __KERNEL_TSS_H__

void update_tss(struct pcb *proc);
void install_tss();

#endif

