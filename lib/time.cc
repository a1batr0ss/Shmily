#include <cmos.h>

struct time num2time(unsigned long long num)
{
	struct time t;
	t.year = num >> 32;
	t.month = (num >> 28) & 0xf;
	t.day = (num >> 22) & 0x3f;
	t.hour = (num >> 16) & 0x3f;
	t.minute = (num >> 8) & 0x3f;
	t.second = num & 0xff;

	return t;
}

unsigned long long time2num(struct time t)
{
	/* Must cast to 64bit before. */
	return ((unsigned long long)(t.year) << 32) | ((t.month&0xf) << 28) | ((t.day&0x3f) << 22) | ((t.hour&0x3f) << 16) | (t.minute << 8) | t.second;
}

