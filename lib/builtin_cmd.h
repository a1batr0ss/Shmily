#ifndef __LIB_BUILTIN_CMD_H__
#define __LIB_BUILTIN_CMD_H__

#include <global.h>

void ps();
void meminfo();
void ls(char *path=NULL);
bool cd(char *path="/");
void pwd();

#endif

