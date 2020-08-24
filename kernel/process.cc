#include <global.h>
#include <string.h>
#include <stdio.h>
#include <cycle_list.h>
#include <all_syscall.h>
#include "page.h"
#include "process.h"
#include "interrupt.h"
#include "print.h"
#include "tss.h"

struct pcb *processes[NR_PROC];
CycleList ready_processes[PRIORITY_KIND];  /* kind of priority. */
unsigned int max_rounds[PRIORITY_KIND];  /* Prevent the low priority process starve. */
struct pcb *cur_proc;
struct pcb *next_proc;
struct pcb *prev_proc;
struct pcb *first_ready_proc;
struct pcb *last_ready_proc;
char cur_proc_idx;
unsigned int max_priority;  /* Recored the max priority ready queue which has process, so that we couldn't scan ready_processes from head. */

unsigned int allocate_pid();

void init_pm()
{
	for (int i=0; i<PRIORITY_KIND; i++) {
		ready_processes[i].initialize();
		max_rounds[i] = 10;
	}

	max_priority = PRIORITY_KIND + 1;
}

inline unsigned int priority2ticks(enum process_priority priority)
{
	return (PRIORITY_KIND - (unsigned int)priority) * 8;
}

void init_process_info(struct pcb *proc, char *name_, int pid, enum process_priority priority_)
{
    proc->priority = priority_;
    proc->pid = pid;
    proc->ticks = priority2ticks(priority_);
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

int append_all_array(struct pcb *proc, int pid)
{
	if (pid >= 0) {
		processes[pid] = proc;
		return pid;
	}

    /* Get a free slot in processes */
    for (unsigned char idx=0; idx<NR_PROC; idx++) {
        if (0 == processes[idx]) {
            processes[idx] = proc;
            return idx;
        }
    }
    return -1;  /* ready array is full, append failed. */
}

struct pcb* start_process(char *name, int pid, enum process_priority priority, proc_target func, void *args, struct pcb *proc)
{
	int pid_ = append_all_array(proc, pid);

    init_process_info(proc, name, pid_, priority);
    create_process(proc, func, args);

    /* Append to all queue. */
    // append_all_array(proc, pid);

    /* Append to ready queue. */
	unsigned int prio_idx = (unsigned int)priority;
	ready_processes[prio_idx].append(&(proc->ready_elem));

	if (priority < max_priority)
		max_priority = priority;
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
	next_proc = NULL;

	/* For block() expecially in ipc. The status filed is WAITING/SENDING. */
	if (RUNNING == cur_proc->status)
		cur_proc->status = READY;
	else if (WAITING_MID == cur_proc->status) {  /* Lazy schedule. */
		cur_proc->status = WAITING_MSG;
		unsigned int ready_idx = (unsigned int)(cur_proc->priority);
		ready_processes[ready_idx].remove(&(cur_proc->ready_elem));
	} else {
		/* Nothing to do. */
	}

	/* Pick a process. (Priority schedule.) */
	unsigned int max_prio = max_priority;
	for (; max_prio<PRIORITY_KIND; max_prio++) {
		if ((0 == max_rounds[max_prio]) && (max_prio < 3)) {
			max_rounds[max_prio] = 10;
			continue;
		}

		if (!ready_processes[max_prio].is_empty())
			break;
	}

	if ((unsigned int)(cur_proc->priority) == max_prio) {
		next_proc = (struct pcb*)ready_processes[max_prio].next_elem(&(cur_proc->ready_elem));
		max_rounds[cur_proc->priority]--;
	}
	else if (max_prio < PRIORITY_KIND)
		next_proc = (struct pcb*)(ready_processes[max_prio].get_elem(0));
	else {
		/*
		 * max_prio > PRIORITY_KIND
		 * That means no process in ready queue, We shoud revoke the idle process.
		 * But the init process is in ready queue at anytime. So we the ready queue is always has the init process. In this case, we need not do anything.(Process the idle)
	     */
	}

    if (NULL != next_proc) {
		next_proc->status = RUNNING;

        if (next_proc->is_userproc)
            update_tss(next_proc);

        /* 2. switch to the ready process which just picked. */
        switch_to(next_proc);
    } else {
        /* TODO: Revoke the idle. Wouldnt' get there. */
		printf("NULL");
    }
}

void init_userprocess(proc_target func)
{
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

void start_userprocess(char *name, int pid, enum process_priority priority, proc_target func, void *args, struct pcb *proc, unsigned int userstack)
{
	int pid_ = append_all_array(proc, pid);

    init_process_info(proc, name, pid_, priority);
    proc->is_userproc = true;
	proc->userstack = userstack;
    create_process(proc, (void (*)(void*))init_userprocess, (void*)func);

    /* Append to all queue. */
    // append_all_array(proc, pid);

	unsigned int prio_idx = (unsigned int)priority;
	ready_processes[prio_idx].append(&(proc->ready_elem));

	if (priority < max_priority)
		max_priority = priority;
}

/* The init process's information. */
void deal_init_process(int pid)
{
    struct pcb *init_proc = (struct pcb*)0x80000;
	int pid_ = append_all_array(init_proc, pid);

    init_process_info(init_proc, "init", pid_, PRIORITY_E);
    cur_proc = init_proc;
    cur_proc_idx = -1;
    init_proc->status = RUNNING;

	append_all_array(init_proc, pid);

	unsigned int prio_idx = (unsigned int)PRIORITY_E;
	ready_processes[prio_idx].append(&(init_proc->ready_elem));

	if (PRIORITY_E < max_priority)
		max_priority = PRIORITY_E;
}

void self_block(enum process_status stat)
{
    bool old_status = disable_intr();

	if (WAITING_MSG == stat) {
		cur_proc->status = WAITING_MID;
		schedule();
		set_intr(old_status);

		return;
	}

    cur_proc->status = stat;

	unsigned int ready_idx = (unsigned int)(cur_proc->priority);
	ready_processes[ready_idx].remove(&(cur_proc->ready_elem));
    schedule();

    set_intr(old_status);
}

void proc_yield()
{
    bool old_status = disable_intr();

    cur_proc->status = READY;

    schedule();

    set_intr(old_status);
}

/* Haven't test it. */
void unblock_proc(struct pcb *proc)
{
    bool old_status = disable_intr();

    proc->status = READY;

	if (WAITING_MID == proc->status) {
		set_intr(old_status);

		return;
	}

	if (proc->priority < max_priority)
		max_priority = proc->priority;

	unsigned int ready_idx = (unsigned int)(proc->priority);
	ready_processes[ready_idx].append(&(proc->ready_elem));

    set_intr(old_status);
}

void idle_process(void *args)
{
    while (1) {
        asm volatile ("sti; hlt" : :);
        self_block(BLOCKED);
    }
}

struct pcb* get_current_proc()
{
	unsigned int esp = 0;
	asm volatile ("mov %%esp, %0" : "=g"(esp));

	return (struct pcb*)(esp & 0xffffff000);
}

struct pcb* pid2pcb(unsigned int pid)
{
	return processes[pid];
}

void create_process(proc_target *func)
{
	struct pcb *pcb = (struct pcb*)malloc(sizeof(struct pcb));
	unsigned int userstack = (unsigned int)malloc(paging::page_size);

	start_userprocess("USER_CREATE", -1, PRIORITY_D, func, NULL, pcb, userstack);
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

