#ifndef __LIB_PRINT_H__
#define __LIB_PRINT_H__

extern "C" void putchar(char ch);
extern "C" void putstring(const char *str);
extern "C" void puthex(unsigned int num);
extern "C" void set_cursor(unsigned int pos);
extern "C" void cls();

#endif

