#include <io.h>
#include <stdio.h>
#include "cmos.h"

struct time systime_start;

inline unsigned char bcd2dec(unsigned char bcd)
{
	return ((bcd & 0xf) + ((bcd>>4)*10));
}

struct time get_current_time();

void init_systime()
{
	systime_start = get_cmos_time();

	printf("Now is %d-%d-%d %d:%d:%d \n", systime_start.year, systime_start.month, systime_start.day, systime_start.hour, systime_start.minute, systime_start.second);
}

struct time get_cmos_time()
{
	struct time cur_time;
	outb(0x70, 0x0);
	cur_time.second = bcd2dec(inb(0x71));
	outb(0x70, 0x2);
	cur_time.minute = bcd2dec(inb(0x71));
	outb(0x70, 0x4);
	cur_time.hour = bcd2dec(inb(0x71));
	outb(0x70, 0x7);
	cur_time.day = bcd2dec(inb(0x71));
	outb(0x70, 0x8);
	cur_time.month = bcd2dec(inb(0x71));
	outb(0x70, 0x9);
	cur_time.year = bcd2dec(inb(0x71)) + 2000;

	return cur_time;
}

