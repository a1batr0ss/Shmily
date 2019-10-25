#include <global.h>
#include "sync.h"
#include "process.h"
#include "interrupt.h"
#include "print.h"

Semaphore::Semaphore()
{
    value = 1;
    waiters = NULL;
}

void Semaphore::wait()
{
    disable_intr();

    while (0 == value) {
    	/* Append to waiters queue. */
        cur_proc->next_ready = waiters;
        waiters = cur_proc;

        self_block(BLOCKED);

    }

    value--;
    enable_intr();
}

void Semaphore::signal()
{
    disable_intr();

    if (NULL != waiters) {
        /* wakeup a process. */
        struct pcb *will_wakeup = waiters;
        waiters = waiters->next_ready;  /* Next process in waiter queue. */

        unblock_proc(will_wakeup);
    }
    value++;

    enable_intr();
}

Lock::Lock()
{
    holder = NULL;
}

void Lock::acquire()
{
    
    if (holder != cur_proc) {
        sema.wait();
        holder = cur_proc;
    }
}

void Lock::release()
{
    holder = NULL;
    sema.signal(); 
}

