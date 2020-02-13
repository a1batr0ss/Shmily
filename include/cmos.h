#ifndef __INCLUDE_CMOS_H__
#define __INCLUDE_CMOS_H__

struct time {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned int year;
};

#endif

