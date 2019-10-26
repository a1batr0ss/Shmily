#include <global.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "print.h"
#include "memory.h"
#include "sync.h"
#include "ipc.h"

void f1(void *args);
void f2(void *args);

Lock *lock;  /* Couldn't define object here. Just a pointer. */

int main()
{
    enable_paging();

    init_mempool();
    init_intr();
    deal_init_process();

    start_process("proc1", 32, f1, NULL, (struct pcb*)0x90000);
    start_process("proc2", 39, f2, NULL, (struct pcb*)0x91000);

    enable_intr();

    Lock l;
    lock = &l;

    while (1);

    return 0;
}

void f1(void *args)
{
    Message msg(0x90000);
    msg.receive(0x91000);

    putstring("context is ");
    puthex((unsigned int)msg.get_context());

    while (1) ;
}

void f2(void *args)
{
	Message msg(0x91000, 1, 123);
    msg.send(0x90000);
    
    while (1) ;
}
