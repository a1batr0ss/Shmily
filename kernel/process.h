#ifndef __PM_PROCESS_H__
#define __PM_PROCESS_H__

#include "ipc.h"

#define NR_PROC 64

typedef void proc_target(void*);

enum process_status {
    RUNNING, READY, BLOCKED, WAITING, HANGING, DIED, WAITING_MSG, SENDING_MSG
};

struct thread_stack {
    unsigned int ebp;
    unsigned int ebx;
    unsigned int edi;
    unsigned int esi;
    void (*eip)(proc_target func, void *args);
    void *ret_addr;
    proc_target *func;
    void *args;
} __attribute__((packed));

struct pcb {
    unsigned int pid;
    char name[16];
    enum process_status status;
    unsigned int priority;
    unsigned int ticks;
    unsigned int elapsed_ticks;
    unsigned int *pagedir_pos;
    unsigned int *esp;
    struct pcb *next_ready;
    Message *message;
    struct pcb *sendings;
    char padding[3000];  /* tentatively. */
    struct thread_stack self_stack;
} __attribute__((packed));

extern struct pcb *processes[NR_PROC];
extern struct pcb *cur_proc;
extern char cur_proc_idx;

void schedule();
struct pcb* start_process(char *name, unsigned int priority, proc_target func, void *args, struct pcb *proc);
void deal_init_process();
void self_block(enum process_status stat);
void proc_yield();
void unblock_proc(struct pcb *proc);
void traverse_ready_queue();
void idle_process(void *args);

#endif

