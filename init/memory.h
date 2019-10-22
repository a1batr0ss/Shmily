#ifndef __MM_MEMORY_H__
#define __MM_MEMORY_H__

#include "bitmap.h"

struct viraddr_manage {
    struct bitmap vaddr_bmap;
    unsigned int vaddr_start;
};

struct mem_pool {
    struct bitmap phyaddr_bmap;
    unsigned int phyaddr_start;
    unsigned int pool_size;
};

void init_mempool();

#endif

