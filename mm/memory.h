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

class MemoryManager {
  private:
	struct viraddr_manage initproc_vir;
	struct mem_pool phypool;
	struct mem_global_desc glo_desc[7];  /* 16B, 32B, 64B, 128B, 256B, 512B, 1024B */

	unsigned int get_mem_capacity();
	void* malloc_page(unsigned int cnt);
	void* get_vir_page(unsigned int cnt);
	void* get_phy_page();
	void free_page(void *viraddr, unsigned int cnt);
	void free_phy_page(unsigned int phyaddr);
	void free_vir_page(unsigned int viraddr, unsigned int cnt);
	void remove_map_addr(unsigned int viraddr);

	void map_vir_phy(unsigned int viraddr, unsigned int phyaddr);

  public:
	MemoryManager();
	void* malloc(unsigned int cnt_bytes);
	void free(void *addr);

	void print_mem_info();
};

#endif

