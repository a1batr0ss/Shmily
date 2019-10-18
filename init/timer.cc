#include "io.h"

void init_pit()
{
    unsigned int divisor = 1193180 / 100;
    unsigned char low_byte = (unsigned char)(divisor & 0xff);
    unsigned char high_byte = (unsigned char)((divisor>>8) & 0xff);

    outb(0x43, 0x36);  /* repeating mode */
    outb(0x40, low_byte);
    outb(0x40, high_byte);
}

