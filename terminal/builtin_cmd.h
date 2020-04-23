#ifndef __LIB_BUILTIN_CMD_H__
#define __LIB_BUILTIN_CMD_H__

#include <global.h>

void ps();
void meminfo();
void ls(char *path=NULL);
bool cd(char *path="/");
void pwd();
void cat(char *filename);
void cp_file(char *src, char *dst);
void mv_file(char *src, char *dst);
void chmod(char *new_mode, char *path);
void chown(char *new_mode, char *new_owner);
void last();
void halt();
void power_off();
void reboot();

#endif

