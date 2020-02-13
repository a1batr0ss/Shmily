#include <ipc_glo.h>
#include <global.h>
#include <stdio.h>
#include <syscall.h>
#include <all_syscall.h>
#include "fs.h"
#include "inode.h"

int main()
{
	init_fs(); 



	Message msg;

	struct inode *inode = (struct inode*)malloc(sizeof(struct inode));
	create_inode(inode);
	sync_inode(inode);

	msg.receive(0);
	printf("fs received message: %x.\n", msg.get_context().con_1);
	while (1);

	return 0;
}

