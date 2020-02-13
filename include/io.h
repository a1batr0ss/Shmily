#ifndef __INCLUDE_IO_H__
#define __INCLUDE_IO_H__

static inline void outb(unsigned short port, unsigned char data)
{
    asm volatile ("outb %b0, %w1":: "a" (data), "Nd" (port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile ("inb %w1, %b0": "=a" (ret) : "Nd" (port));
    return ret;
}

static inline void outsw(unsigned port, const char *addr, unsigned int cnt)
{
	asm volatile ("cld; rep outsw" : "+S" (addr), "+c" (cnt) : "d" (port));
}

static inline void insw(unsigned port, char *addr, unsigned cnt)
{
	asm volatile ("cld; rep insw" : "+D" (addr), "+c" (cnt) : "d" (port) : "memory");
}

#endif

