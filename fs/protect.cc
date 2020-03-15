#include "fs.h"
#include "inode.h"
#include "dir.h"

bool check_inode_permission(struct inode &inode, unsigned char op)
{
	if (uid == 0)
		return true;

	if (uid == inode.uid)  /* Owner. */
		op <<= 3;

	return inode.mode & op;
}

/* Not recursion. */
void sys_chmod(char *path, unsigned char new_mode)
{
	int ino = dir_is_exists(path);
	if (-1 == ino)
		return;

	struct inode inode = ino2inode(ino);
	/* Check permission. Not use check_inode_permission(), For the owner, he can execute this command anyway, but check_inode_permission() would have a check for anyone.(Inclue the owner) */
	if ((0 == uid) || (uid == inode.uid))
		inode.mode = new_mode;

	sync_inode(&inode);
	return;
}

