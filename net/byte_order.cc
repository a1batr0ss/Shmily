#include "byte_order.h"

/* Adjust a word's byte order. */
unsigned short swap_word(unsigned short word)
{
	return ((word & 0xff) << 8) | (word >> 8);
}

/* Adjust a double word's byte order. */
unsigned int swap_double_word(unsigned int dw)
{
	return (((dw & 0xff) << 24) | ((dw & 0xff00) << 8) | \
			((dw & 0xff0000) >> 8) | ((dw & 0xff000000) >> 24));
}

