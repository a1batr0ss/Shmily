#include <global.h>
#include <stdio.h>
#include <all_syscall.h>
#include "fs.h"

/* Just for the slave disk. */
void init_fs()
{
	printf("init fs.\n");

	struct super_block *buf = (struct super_block*)malloc(_fs::sector_size);

	if (NULL == buf)
		return;

	free(buf);
	printf("end init fs.\n");
}

