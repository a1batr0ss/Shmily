#include <global.h>
#include <string.h>
#include "page.h"
#include "process.h"
#include "interrupt.h"
#include "print.h"

struct pcb *processes[NR_PROC];
struct pcb *cur_proc;
struct pcb *next_proc;
struct pcb *prev_proc;
struct pcb *first_ready_proc;
struct pcb *last_ready_proc;
char cur_proc_idx;

unsigned int allocate_pid();

void init_process_info(struct pcb *proc, char *name_, unsigned int priority_)
{
    proc->priority = priority_;
    proc->pid = allocate_pid();
    proc->ticks = priority_;
    proc->elapsed_ticks = 0;
    proc->pagedir_pos = NULL;
}

void execute_func(proc_target func, void *args)
{
    enable_intr();
    func(args);
}

void create_process(struct pcb *proc, proc_target func_, void *args_)
{
    proc->esp = (unsigned int*)((unsigned int)proc + sizeof(struct pcb) - sizeof(struct thread_stack));
    struct thread_stack *self_stack = &proc->self_stack;
    self_stack->ebp = 0;
    self_stack->ebx = 0;
    self_stack->esi = 0;
    self_stack->edi = 0;
    self_stack->eip = execute_func;
    self_stack->func = func_;
    self_stack->args = args_;
    proc->status = RUNNING;  /* default is 0(RUNNING) */
}

unsigned int allocate_pid()
{
    static unsigned int pid = 0;
    pid++;
    return pid;
}

int append_ready_array(struct pcb *proc)
{
    /* Get a free slot in processes */
    for (unsigned char idx=0; idx<NR_PROC; idx++) {
        if (0 == processes[idx]) {
            processes[idx] = proc;
            return idx;
        }
    }
    return -1;  /* ready array is full, append failed. */
}

struct pcb* start_process(char *name, unsigned int priority, proc_target func, void *args, struct pcb *proc)
{
    init_process_info(proc, name, priority);
    create_process(proc, func, args);
    /* Append to all queue. */
    append_ready_array(proc);

    /* Append to ready queue. */
    if (NULL == first_ready_proc)
    first_ready_proc = proc;

    if (NULL != last_ready_proc) {
        last_ready_proc->next_ready = proc;
        last_ready_proc = proc;
    } else {
        last_ready_proc = proc;
    }

    last_ready_proc->next_ready = first_ready_proc;
}

void switch_to(struct pcb *next)
{
    unsigned int *retaddr = (unsigned int*)&next - 1;

    asm volatile ("movl %%esp, %0" : "=m" (cur_proc->esp) : : "memory");

    prev_proc = cur_proc;
    cur_proc = next_proc;

    asm volatile ("pushl %1\n\t" \
                    "pushl %%esi\n\t" \
                    "pushl %%edi\n\t" \
                    "pushl %%ebx\n\t" \
                    "pushl %%ebp\n\t" \
                    "movl %%esp, %0\n\t" \
                    "movl %2, %%esp\n\t" \
                    "popl %%ebp\n\t" \
                    "popl %%ebx\n\t" \
                    "popl %%edi\n\t" \
                    "popl %%esi\n\t" \
                    "ret" : "+m" (prev_proc->esp) : "m" (*retaddr), "m" (cur_proc->esp) : "memory");
}

void schedule()
{
    /* 1. pick a ready process */

    if (RUNNING == cur_proc->status) {
        if (NULL == last_ready_proc) {
            
        }
        last_ready_proc->next_ready = cur_proc;
        last_ready_proc = cur_proc;
        // cur_proc = first_ready_proc;
        last_ready_proc->next_ready = first_ready_proc;
    }

    if (NULL != first_ready_proc) { 
        next_proc = first_ready_proc;
        last_ready_proc->next_ready = first_ready_proc->next_ready; 
        next_proc->status = RUNNING;
        first_ready_proc = first_ready_proc->next_ready;

        /* 2. switch to the ready process which just picked. */
        switch_to(next_proc);
    }
}

/* The init process's information. */
void deal_init_process()
{
    struct pcb *init_proc = (struct pcb*)0x80000;
    init_process_info(init_proc, "init", 2);
    cur_proc = init_proc;
    cur_proc_idx = -1;
    init_proc->status = RUNNING;

    first_ready_proc = NULL;
    last_ready_proc = NULL;
}

void self_block(enum process_status stat)
{
    disable_intr();

    cur_proc->status = stat;

    schedule(); 

    enable_intr();  /* Not always should enable interrupt, or disabled original. */
}

/* Haven't test it. */
void unblock_proc(struct pcb *proc)
{
    disable_intr();

    proc->status = READY;
    last_ready_proc->next_ready = proc;
    proc->next_ready = first_ready_proc;
    last_ready_proc = proc;

    enable_intr(); 
} 

/* Just for debug. */
void traverse_ready_queue()
{
    putstring("In ready ");
    putstring("ready queue is ");
    struct pcb *first = first_ready_proc;
    while (1) {
        if (first == last_ready_proc) {
        puthex((unsigned int)first);
            break;
        }
        puthex((unsigned int)first);
        putstring("-->");
        first = first->next_ready;
    }
    putstring("\n"); 
}
