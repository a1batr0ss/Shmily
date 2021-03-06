#ifndef __KERNEL_PROCESS_H__
#define __KERNEL_PROCESS_H__

#include <cycle_list.h>
#include "ipc.h"

#define NR_PROC 64
#define PRIORITY_KIND 5

#define SELECTOR_U_DATA ((5 << 3) + (0 << 2) + 3)
#define SELECTOR_U_CODE ((4 << 3) + (0 << 2) + 3)
#define SELECTOR_GS ((6 << 3) + (0 << 2) + 3)
#define EF_MBS (1 << 1)
#define EF_IF_ON (1 << 9)
#define EF_IF_OFF (0 << 9)
#define EF_IOPL_ON (3 << 12)

typedef void proc_target(void*);

enum process_priority {
	PRIORITY_A, PRIORITY_B, PRIORITY_C, PRIORITY_D, PRIORITY_E
};

enum process_status {
    RUNNING, READY, BLOCKED, WAITING, HANGING, DIED, WAITING_MSG, SENDING_MSG, WAITING_MID
};

struct intr_stack {
    unsigned int no;
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp_;
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
    unsigned int gs;
    unsigned int fs;
    unsigned int es;
    unsigned int ds;

    unsigned int err;
    void (*ip)(void*);
    unsigned int cs;
    unsigned int eflags;
    unsigned int esp;
    unsigned int ss;
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
	struct cyclelist_elem ready_elem;
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
	unsigned char intr_cnt;
	unsigned char reply_cnt;

	unsigned int userstack;
    bool is_userproc;
    char padding[3000];  /* tentatively. */
    struct thread_stack self_stack;
} __attribute__((packed));

extern struct pcb *processes[NR_PROC];
extern struct pcb *cur_proc;
extern char cur_proc_idx;

void init_pm();
void schedule();
struct pcb* start_process(char *name, int pid, enum process_priority priority, proc_target func, void *args, struct pcb *proc);
void deal_init_process(int pid);
void self_block(enum process_status stat);
void proc_yield();
void unblock_proc(struct pcb *proc);
void traverse_ready_queue();
void idle_process(void *args);
struct pcb* get_current_proc();
struct pcb* pid2pcb(unsigned int pid);
void start_userprocess(char *name, int pid, enum process_priority priority, proc_target func, void *args, struct pcb *proc, unsigned int userstack);

void create_process(proc_target *func);

void ps();

#endif

