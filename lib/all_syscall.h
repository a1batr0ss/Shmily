#ifndef __LIB_ALL_SYSCALL_H__
#define __LIB_ALL_SYSCALL_H__

void* malloc(unsigned int cnt);
void free(void *addr);

void write_disk(unsigned int disk_nr, unsigned int lba, char *str, unsigned int cnt);
void read_disk(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt);

#endif

