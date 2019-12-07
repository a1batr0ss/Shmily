#ifndef __MM_MEMORY_H__
#define __MM_MEMORY_H__

#include "bitmap.h"

namespace memory {
	const unsigned int glo_desc_nr = 7;
};


struct viraddr_manage {
    struct bitmap vaddr_bmap;
    unsigned int vaddr_start;
};

struct mem_pool {
    struct bitmap phyaddr_bmap;
    unsigned int phyaddr_start;
    unsigned int pool_size;
};

struct mem_global_desc {
	unsigned int start;
	unsigned int size_bytes;
	unsigned int desc_nr;
	struct bitmap bmap;
};

void init_mem();
void* malloc(unsigned int cnt_bytes);
void free(void *buf);

#endif

