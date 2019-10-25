#ifndef __INIT_SYNC_H__
#define __INIT_SYNC_H__

#include "process.h"

class Semaphore {
  private:
    int value;
    struct pcb *waiters;

  public:
    Semaphore();
    void signal();  /* V operation. */
    void wait();  /* P operation.  */
};

class Lock {
  private:
    struct pcb *holder;
    Semaphore sema;

  public:
    Lock();
    void acquire();
    void release();
};

#endif

