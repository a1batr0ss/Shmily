#include <global.h>
#include <string.h>
#include "page.h"
#include "process.h"
#include "interrupt.h"
#include "print.h"

struct pcb *processes[NR_PROC];
struct pcb *cur_proc;
unsigned char cur_proc_idx;

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
    /* append to ready array */
    append_ready_array(proc);
}

void schedule()
{
    /* 1. pick a ready process */
    struct pcb *next = NULL;  /* Define the variable just for easily understand the switch_to function. */
    int old_pos = cur_proc_idx;  /* avoid the assignment below affect the compare in for. */
    for (int i=cur_proc_idx+1; i!=old_pos; i++, i%=NR_PROC) {
        /* roll-back */
        if ((NULL != processes[i]) && (RUNNING == processes[i]->status)) {
            cur_proc_idx = i;
            next = processes[i];
            cur_proc = processes[i];
            puthex((unsigned int)processes[i]);
        }  
    }

    /* 2. switch to the ready process which just picked. */
}

