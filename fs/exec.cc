#include <elf.h>
#include <string.h>
#include <stdio.h>
#include <all_syscall.h>
#include "file.h"

void exec_file(char *file)
{
	create_process(0x50000);  /* Just for test. */
}

