#include "string.h"
#include "stdio.h"

unsigned int string2uint(char *str)
{
	unsigned int res = 0;
	for (int i=strlen(str)-1, factor=1; i>=0; i--, factor*=10)
		res += (factor*(str[i] - '0'));

	return res;
}

/* Will trancate this string. */
unsigned int ipstring2ipuint(char *str)
{
	unsigned int res = 0;
	char *start = str;
	int times = 3;
	int len = strlen(str);
	// printf("str is %s len is %d\n", str, len);

	for (int i=0; i<len; i++) {
		if ('.' == str[i]) {
			str[i] = 0;

			unsigned int tmp = (string2uint(start) << (8*times));
			// printf("i %d times %d start %s tmp %x\n", i, times, start, tmp);
			res |= tmp;
			start = &str[i] + 1;
			times--;
		} else if (i == (len-1)) {
			unsigned int tmp = (string2uint(start) << (8*times));
			res |= tmp;
			times--;
		}

		if (times < 0)
			break;
	}

	return res;
}

