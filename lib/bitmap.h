#ifndef __LIB_BITMAP_HH__
#define __LIB_BITMAP_HH__

struct bitmap {
    unsigned int bytes_len;
    unsigned char *base;
};

void init_bitmap(struct bitmap *bmap);
void bitmap_set_bit(struct bitmap *bmap, unsigned int idx, unsigned char val);
int bitmap_scan(struct bitmap *bmap, unsigned int cnt);

#endif

