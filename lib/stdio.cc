#include <global.h>
#include <string.h>
#include "print.h"



void itoa(unsigned int val, char **buf, unsigned base)
{
	unsigned int remainder = val % base;
	unsigned int multiples = val / base;
	if (multiples)
		itoa(multiples, buf, base);

	if (remainder < 10)
		*((*buf)++) = remainder + '0';
	else
		*((*buf)++) = remainder - 10 + 'A';
}

unsigned vsprintf(char *dst, const char *src, char *first_arg)
{
	int arg_int;
	char *arg_str;
	char cur_char = *src;
	char *arg_pointer = first_arg;
	while (cur_char) {
		if ('%' != cur_char) {
			*dst++ = cur_char;
			cur_char = *(++src);
			continue;
		}
		cur_char = *(++src);
		switch (cur_char) {
		case 's':
			arg_str = *((char**)(arg_pointer+=4));
			strcpy(dst, arg_str);
			dst += strlen(arg_str);
			cur_char = *(++src);
			break;
		case 'c':
			*dst++ = *((char*)(arg_pointer+=4));
			cur_char = *(++src);
			break;
		case 'd':
			arg_int = *((int*)(arg_pointer+=4));
			if (arg_int < 0) {
				arg_int = 0 - arg_int;
				*dst++ = '-';
			}
			itoa(arg_int, &dst, 10);
			cur_char = *(++src);
			break;
		case 'x':
			arg_int = *((int*)(arg_pointer+=4));
			itoa(arg_int, &dst, 16);
			cur_char = *(++src);
			break;
		}
	}
	return strlen(dst);
}

void printf(char *str, ...)
{
	char *first_arg = (char*)&str;  /* Point to the first argument in stack. next is the first variable argument. */
	char buf[1024] = {0};
	vsprintf(buf, str, first_arg);
	first_arg = NULL;
	putstring(buf);
}

