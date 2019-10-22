#include <string.h>
#include "bitmap.h"
#include "print.h"

void init_bitmap(struct bitmap *bmap)
{
    memset((char*)bmap->base, 0, bmap->bytes_len);
}

bool bitmap_verify_bit(struct bitmap *bmap, unsigned int idx)
{
    unsigned int byte_idx = idx / 8;
    unsigned int bit_idx = idx % 8;
    return (bmap->base[byte_idx] & (1 << bit_idx));
}

void bitmap_set_bit(struct bitmap *bmap, unsigned int idx, unsigned char val)
{
    unsigned int byte_idx = idx / 8;
    unsigned int bit_idx = idx % 8;

    if (val)
        bmap->base[byte_idx] |= (1 << bit_idx);
    else
        bmap->base[byte_idx] &= (~(1 << bit_idx));
}

/* Get cnt free bits in bitmap */
int bitmap_scan(struct bitmap *bmap, unsigned int cnt)
{
    unsigned int byte_idx = 0;
    while ((0xff==bmap->base[byte_idx]) && (byte_idx<bmap->bytes_len))
        byte_idx++;
    if (byte_idx == bmap->bytes_len)
        return -1;
    
    /* byte_idx is the byte which has free bits now. */
    int bit_idx = 0;
    while ((unsigned char)(1 << bit_idx) & (bmap->base[byte_idx]))
        bit_idx++;

    int ret = byte_idx*8 + bit_idx;
    if (1 == cnt)
       return ret;

    /* wanna get more free bits. */
    unsigned int left_bits = (bmap->bytes_len*8 - ret); 
    unsigned int next_bit = ret + 1;
    unsigned int count = 1;
    ret = -1;
    while (left_bits-- > 0) {
        if (!bitmap_verify_bit(bmap, next_bit))
            count++;
        else
            count = 0;
        
        if (cnt == count) {
            ret = next_bit - cnt + 1;
            break;
        }

        next_bit++; 
    } 
    return ret; 
}

