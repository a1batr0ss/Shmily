#ifndef __KERNEL_CMOS_H__
#define __KERNEL_CMOS_H__

struct time {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned int year;
};

extern struct time systime_start;

void init_systime();

#endif

