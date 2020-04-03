#ifndef __LIB_ALL_SYSCALL_H__
#define __LIB_ALL_SYSCALL_H__

void* malloc(unsigned int cnt);
void free(void *addr);

unsigned int get_keyboard_buffer();
void write_disk(unsigned int disk_nr, unsigned int lba, char *str, unsigned int cnt);
void read_disk(unsigned int disk_nr, unsigned int lba, char *buf, unsigned int cnt);
void print_partition_info(unsigned int disk_nr);
unsigned int get_disk(unsigned int disk_nr);
void send_packet(unsigned int pkt);
void get_mac_addr(char *mac_addr);

void register_intr_handler(unsigned int intr_num, void *handler);
unsigned long long get_current_time();
void create_process(unsigned int func);

void mkdir(char *path);
void rmdir(char *path);
void mkfile(char *path);
void rmfile(char *path);
int open(char *path);
void close(unsigned int fd);
void write(unsigned int fd, char *str, unsigned int count, unsigned char mode);
unsigned int read(unsigned int fd, char *buf, unsigned int count);
bool eof(unsigned int fd);
void lseek(unsigned int fd, unsigned int offset);
void readline(unsigned int fd, char *buf);
void get_cur_dir(char *buf);

#endif

