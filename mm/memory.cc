#include <global.h>
#include <string.h>
#include <print.h>
#include <stdio.h>
#include <bitmap.h>
#include "memory.h"
#include "swar.h"

#define DIV_ROUND_UP(x, y) ((x + y - 1) / (y))

MemoryManager::MemoryManager()
{
	/* Initialize the memory pool. */
	unsigned int mem_capacity = get_mem_capacity();

    unsigned int stationary_mem = 0x210000;  /* more than 2M can't be accessed by other processes */
    unsigned int free_mem = mem_capacity - stationary_mem;
    unsigned int free_pages = free_mem / paging::page_size;

    unsigned int bmap_length = free_pages / 8;
    unsigned int phy_start = 0x210000;

    this->phypool.phyaddr_start = phy_start;
    this->phypool.pool_size = free_mem;
    this->phypool.phyaddr_bmap.bytes_len = bmap_length;
    this->phypool.phyaddr_bmap.base = (unsigned char*)0x9000;

    init_bitmap(&(this->phypool.phyaddr_bmap));

    /* Initialise the virtaul address bitmap. */
    this->initproc_vir.vaddr_start = 0x210000;
    this->initproc_vir.vaddr_bmap.bytes_len = bmap_length;
    this->initproc_vir.vaddr_bmap.base = (unsigned char*)(0x9000 + bmap_length);

    init_bitmap(&(this->initproc_vir.vaddr_bmap));

	/* Initialize the global descriptor. */
	unsigned int desc_start = 0x200000;
	unsigned int desc_bmap_start = 0x207000;
	unsigned int size = 16;  /* 16B-->32B-->64B .... */
	for (int i=0; i<memory::glo_desc_nr; i++, size*=2) {
		this->glo_desc[i].start = desc_start;
		this->glo_desc[i].size_bytes = paging::page_size;
		this->glo_desc[i].desc_nr = paging::page_size / size;

		this->glo_desc[i].bmap.base = (unsigned char*)desc_bmap_start;
		this->glo_desc[i].bmap.bytes_len = DIV_ROUND_UP(this->glo_desc[i].desc_nr, 8);
		init_bitmap(&(this->glo_desc[i].bmap));  /* Modify the paging table before doing this. */

		desc_bmap_start += this->glo_desc[i].bmap.bytes_len;
		desc_start += paging::page_size;
	}

}

unsigned int MemoryManager::get_mem_capacity()
{
   return *(unsigned int*)0x548;
}

void* MemoryManager::get_vir_page(unsigned int cnt)
{
    int idx_start = -1;
    int vaddr_start = 0;
    idx_start = bitmap_scan(&(this->initproc_vir.vaddr_bmap), cnt);
    if (-1 == idx_start)
        return NULL;

    unsigned int cnt_idx = 0;
    while (cnt_idx < cnt)
        bitmap_set_bit(&(this->initproc_vir.vaddr_bmap), idx_start+(cnt_idx++), 1);

    vaddr_start = this->initproc_vir.vaddr_start + idx_start*paging::page_size;

    return (void*)vaddr_start;
}

void* MemoryManager::get_phy_page()
{
    int idx_free = bitmap_scan(&(this->phypool.phyaddr_bmap), 1);

    if (-1 == idx_free)
        return NULL;

    bitmap_set_bit(&(this->phypool.phyaddr_bmap), idx_free, 1);
    unsigned int phyaddr = this->phypool.phyaddr_start + idx_free*paging::page_size;
    return (void*)phyaddr;
}

static unsigned int* vaddr2pde(unsigned int vaddr)
{
    unsigned int *pde = (unsigned int*)((0xfffff000) + \
            4 * ((vaddr&0xffc00000) >> 22));
    return pde;
}

static unsigned int* vaddr2pte(unsigned int vaddr)
{
    unsigned int *pte = (unsigned int*)(0xffc00000 + \
            ((vaddr&0xffc00000)>>10) + \
            4 * ((vaddr&0x003ff000)>>12));
}

static unsigned int vaddr2phy(unsigned int vaddr)
{
	unsigned int *pte = vaddr2pte(vaddr);
	return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}

void MemoryManager::map_vir_phy(unsigned int viraddr, unsigned int phyaddr)
{
    unsigned int *pde_vaddr = vaddr2pde(viraddr);
    unsigned int *pte_vaddr = vaddr2pte(viraddr);
    unsigned int is_present = 0x00000001;

    if (*pde_vaddr & is_present) {
        if (*pte_vaddr & is_present) {
            /* Should not exists. */
        } else {
            *pte_vaddr = phyaddr | paging::US_U | paging::RW_W | paging::P;
        }
    } else {
        /* This pde is not exists, which means the page table is not exists. */
        unsigned int pg_table = (unsigned int)get_phy_page();
        *pde_vaddr = pg_table | paging::US_U | paging::RW_W | paging::P;
        memset((char*)((unsigned int)pte_vaddr&0xfffff000), 0, paging::page_size);
        *pte_vaddr = phyaddr | paging::US_U | paging::RW_W | paging::P;
    }
}

void* MemoryManager::malloc_page(unsigned int cnt)
{
    void *vaddr_start = get_vir_page(cnt);
    unsigned int vaddr = (unsigned int)vaddr_start;
    while (cnt-- > 0) {
        void *phyaddr_start = get_phy_page();

        map_vir_phy((unsigned int)vaddr, (unsigned int)phyaddr_start);
        vaddr += paging::page_size;
    }
    return vaddr_start;
}

void MemoryManager::free_phy_page(unsigned int phyaddr)
{
	unsigned int idx = (phyaddr - this->phypool.phyaddr_start ) / paging::page_size;
	bitmap_set_bit(&phypool.phyaddr_bmap, idx, 0);
}

void MemoryManager::free_vir_page(unsigned int viraddr, unsigned int cnt)
{
	unsigned int cnt_idx = 0;
	unsigned idx = (viraddr - this->initproc_vir.vaddr_start) / paging::page_size;

	while (cnt_idx < cnt)
		bitmap_set_bit(&(this->initproc_vir.vaddr_bmap), idx+(cnt_idx++), 0);
}

void MemoryManager::remove_map_addr(unsigned int viraddr)
{
	unsigned int *pte = vaddr2pte(viraddr);
	*pte &= ~paging::P;

	/* INVLPG: privilege instruction, if run below protect mode, the CPL must be zero. Now our MM is a user process.(CPL is 3) So we can't execute it. */
	// asm volatile ("invlpg %0" :: "m" (viraddr) : "memory");
}

void MemoryManager::free_page(void *viraddr, unsigned int cnt)
{
	unsigned int vaddr = (unsigned int)viraddr;
	unsigned int cnt_ = cnt;
	unsigned int paddr = 0;

	while ((cnt_--) > 0) {
		paddr = vaddr2phy(vaddr);
		free_phy_page(paddr);
		remove_map_addr(vaddr);

		vaddr += paging::page_size;
	}
	free_vir_page((unsigned int)viraddr, cnt);
}

void* MemoryManager::malloc(unsigned int cnt_bytes)
{
	if (cnt_bytes > 1024) {
		unsigned int page_cnt = DIV_ROUND_UP(cnt_bytes+4, paging::page_size);
		void *ret = malloc_page(page_cnt);
		memset((char*)ret, 0, page_cnt*paging::page_size);
		*(unsigned int*)ret = page_cnt;
		return ret+sizeof(void*);  /* ret + 1 is really ret + 1 ?? */
	} else {
		unsigned int desc_idx = 0;
		for (unsigned int cnt_start=16; cnt_start<cnt_bytes; cnt_start*=2, desc_idx++) ;

		struct mem_global_desc *desc = &(this->glo_desc[desc_idx]);

		int bit_idx = bitmap_scan(&(desc->bmap), 1);
		bitmap_set_bit(&(desc->bmap), bit_idx, 1);

		void *ret = (void*)(desc->start + bit_idx*(desc->size_bytes / desc->desc_nr));
		memset((char*)ret, 0, 2<<(desc_idx+4));
		return ret;
	}
}

void MemoryManager::free(void *buf)
{
	unsigned int buf_uint = (unsigned int)buf;
	if (buf_uint > 0x210000) {
		unsigned int cnt = *(unsigned int*)(buf_uint - sizeof(void*));
		free_page(buf, cnt);
	} else if ((buf_uint >= 0x200000) && (buf_uint < 0x207000)) {
		unsigned int desc_idx = (buf_uint - 0x200000) / 0x1000;
		struct mem_global_desc *desc = &(this->glo_desc[desc_idx]);
		unsigned int bmap_idx = (buf_uint - desc->start) / (desc->size_bytes / desc->desc_nr);
		bitmap_set_bit(&(desc->bmap), bmap_idx, 0);
	} else {
		printf("Error.\n");
	}
}

/* Roughly calculate the memory information. */
void MemoryManager::print_mem_info()
{
	unsigned int all_mem = get_mem_capacity();
    unsigned int *start = (unsigned int*)(this->phypool.phyaddr_bmap.base);

	unsigned int used_pages = 0;
	for (unsigned int i=0; i<(this->phypool.phyaddr_bmap.bytes_len / 4); i++)
		used_pages += swar(start[i]);

	/* Scattered. */
	unsigned int cnt_bytes = 0;
	for (int i=0; i<memory::glo_desc_nr; i++) {
		unsigned int bytes_units = 2 << (i + 4);
		unsigned int *start_desc = (unsigned int*)(this->glo_desc[i].bmap.base);

		for (int j; j<(this->glo_desc[i].bmap.bytes_len / 4); j++)
			cnt_bytes += bytes_units * swar(start_desc[j]);
	}

	unsigned int all_used = used_pages*paging::page_size + cnt_bytes + 0x200000;
	printf("totoal: %dKB   used: %dKB   free:  %dKB\n", all_mem>>10, all_used>>10, (all_mem-all_used)>>10);
}
