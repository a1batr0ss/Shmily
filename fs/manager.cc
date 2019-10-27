#include "../include/other_module.h"
#include "../include/ipc.h"
#include "print.h"

void filesystem_manager()
{
    putstring("filesystem manager.\n");

    Message msg(0x90000, 1, 213);
    msg.send(0x91000);
    while (1) ;
}

