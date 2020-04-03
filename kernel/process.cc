#include <global.h>
#include <string.h>
#include <stdio.h>
#include <all_syscall.h>
#include "page.h"
#include "process.h"
#include "interrupt.h"
#include "print.h"
#include "tss.h"

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

	memset(proc->name, 0, 16);
	strcpy(proc->name, name_);
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
    proc->status = READY;  /* default is 0(RUNNING) */

    proc->sendings = NULL;
    proc->message = NULL;
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
    if (RUNNING == cur_proc->status) {
        if (NULL == last_ready_proc) {
        }

        if ((NULL == first_ready_proc) && (NULL == last_ready_proc)) {
            first_ready_proc = cur_proc;
            last_ready_proc = cur_proc;
            last_ready_proc->next_ready = first_ready_proc;
        } else {
             /* Append to ready queue. */
            last_ready_proc->next_ready = cur_proc;
            last_ready_proc = cur_proc;
            last_ready_proc->next_ready = first_ready_proc;
        }
    } else {
    }

    if (NULL != first_ready_proc) {
        next_proc = first_ready_proc;
        next_proc->status = RUNNING;

        if (first_ready_proc == last_ready_proc) {
            /* The last process. */
            first_ready_proc->next_ready = NULL;
            first_ready_proc = NULL;
            last_ready_proc = NULL;
        } else {
            last_ready_proc->next_ready = first_ready_proc->next_ready;
            first_ready_proc = first_ready_proc->next_ready;
        }

        if (next_proc->is_userproc)
            update_tss(next_proc);
        /* 2. switch to the ready process which just picked. */
        switch_to(next_proc);
    } else {
        /* TODO: Revoke the idle. */
    }
}

void init_userprocess(proc_target func)
{
    // asm volatile ("xchg %%bx, %%bx"::);
    /* Kernel mode. */
    struct pcb *cur_proc = get_current_proc();
    cur_proc->esp =(unsigned int*)((unsigned int)cur_proc + sizeof(struct pcb));
    struct intr_stack *intr_stack = (struct intr_stack*)(cur_proc->esp);
    intr_stack->edi = 0;
    intr_stack->esi = 0;
    intr_stack->ebp = 0;
    intr_stack->esp_ = 0;
    intr_stack->ebx = 0;
    intr_stack->edx = 0;
    intr_stack->ecx = 0;
    intr_stack->eax = 0;

    intr_stack->gs = SELECTOR_GS;
    intr_stack->ds = SELECTOR_U_DATA;
    intr_stack->fs = SELECTOR_U_DATA;
    intr_stack->es = SELECTOR_U_DATA;

    intr_stack->ip = func;
    intr_stack->cs = SELECTOR_U_CODE;
    intr_stack->eflags = EF_MBS | EF_IOPL_ON | EF_IF_ON;
    intr_stack->esp = cur_proc->userstack;
    intr_stack->ss = SELECTOR_U_DATA;

    asm volatile ("movl %0, %%esp; jmp intr_exit":: "g" (intr_stack) : "memory");
}

void start_userprocess(char *name, unsigned int priority, proc_target func, void *args, struct pcb *proc, unsigned int userstack)
{
    init_process_info(proc, name, priority);
    proc->is_userproc = true;
    proc->userstack = userstack;
    create_process(proc, (void (*)(void*))init_userprocess, (void*)func);

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

/* The init process's information. */
void deal_init_process()
{
    struct pcb *init_proc = (struct pcb*)0x80000;
    init_process_info(init_proc, "init", 2);
    cur_proc = init_proc;
    cur_proc_idx = -1;
    init_proc->status = RUNNING;

	append_ready_array(init_proc);

    first_ready_proc = NULL;
    last_ready_proc = NULL;
}

void self_block(enum process_status stat)
{
    bool old_status = disable_intr();

    cur_proc->status = stat;

    schedule();

    set_intr(old_status);
}

void proc_yield()
{
    bool old_status = disable_intr();

    last_ready_proc->next_ready = cur_proc;
    cur_proc->next_ready = first_ready_proc;
    last_ready_proc = cur_proc;

    cur_proc->status = READY;

    schedule();

    set_intr(old_status);
}

/* Haven't test it. */
void unblock_proc(struct pcb *proc)
{
    bool old_status = disable_intr();

    proc->status = READY;

    if ((NULL == last_ready_proc) && (NULL == first_ready_proc)) {
        first_ready_proc = proc;
        last_ready_proc = proc;
        last_ready_proc->next_ready = proc;
    } else {
        last_ready_proc->next_ready = proc;
        proc->next_ready = first_ready_proc;
        last_ready_proc = proc;
    }

    set_intr(old_status);
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

void idle_process(void *args)
{
    while (1) {
        self_block(BLOCKED);
        printf("idle wake up.\n");
        asm volatile ("sti; hlt" : :);
    }
}

struct pcb* get_current_proc()
{
	unsigned int esp = 0;
	asm volatile ("mov %%esp, %0" : "=g"(esp));

	return (struct pcb*)(esp & 0xffffff000);
}

void create_process(proc_target *func)
{
    struct pcb *pcb = (struct pcb*)malloc(sizeof(struct pcb));
    unsigned int userstack = (unsigned int)malloc(paging::page_size);

    start_userprocess("USER_CREATE", 21, func, NULL, pcb, userstack);
}

void status2string(enum process_status stat, char *stat_string)
{
    switch (stat) {
    case RUNNING:
        strcpy(stat_string, "RUN");
        return;
    case READY:
        strcpy(stat_string, "RDY");
        return;
    case BLOCKED:
        strcpy(stat_string, "BLD");
        return;
    case WAITING:
        strcpy(stat_string, "WAT");
        return;
    case HANGING:
        strcpy(stat_string, "DIE");
        return;
    case WAITING_MSG:
        strcpy(stat_string, "WMG");
        return;
    case SENDING_MSG:
        strcpy(stat_string, "SMG");
        return;
    default:
        strcpy(stat_string, "UKN");
    }
}

void ps()
{
    printf("PID   STAT   PROC\n");
    for (int i=0; i<NR_PROC; i++) {
        char stat[4] = {0};
        if (NULL == processes[i])
            continue;

        status2string(processes[i]->status, stat);
        printf("%d     %s    %s\n", processes[i]->pid, stat, processes[i]->name);
    }
}
