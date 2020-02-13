#ifndef __LIB_STRING_H__
#define __LIB_STRING_H__

void memcpy(char *dst, const char *src, unsigned int cnt);
void memset(char *dst, unsigned char val, unsigned int cnt);
void strcpy(char *dst, const char *src);
unsigned int strlen(const char *str);
bool strcmp(const char *str1, const char *str2);

#endif

