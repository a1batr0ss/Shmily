#ifndef __FS_PROTECT_H__
#define __FS_PROTECT_H__

bool check_inode_permission(struct inode &inode, unsigned char op);
void sys_chmod(char *path, unsigned char new_mode);
void sys_chown(char *path, unsigned int new_uid);

#endif

