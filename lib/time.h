#ifndef __LIB_TIME_H__
#define __LIB_TIME_H__

struct time;

struct time num2time(unsigned long long num);
unsigned long long time2num(struct time t);

#endif

