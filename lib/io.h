#ifndef __LIB_IO_H__
#define __LIB_IO_H__

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

#endif

