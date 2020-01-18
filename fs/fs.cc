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

	// write_disk(1, 0, "Halou World.\n", 1);	
	read_disk(1, 0, (char*)buf, 1);
	printf("buf is %s\n", buf);
	
	free(buf);
	printf("end init fs.\n");
}

