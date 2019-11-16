#include <global.h>
#include <string.h>
#include <print.h>
#include <stdio.h>
#include <bitmap.h>
#include "memory.h"

struct viraddr_manage initproc_vir;
struct mem_pool phypool;

void* malloc_page(unsigned int cnt);
static unsigned int get_mem_capacity();
static void* get_vir_page(unsigned int cnt);
static void* get_phy_page();
void free_page(void *viraddr, unsigned int cnt);

void init_mempool()
{
    unsigned int mem_capacity = get_mem_capacity();

    unsigned int stationary_mem = 0x200000;  /* 2M can't be accessed by other processes */
    unsigned int free_mem = mem_capacity - stationary_mem;
    unsigned int free_pages = free_mem / paging::page_size;

    unsigned int bmap_length = free_pages / 8;
    unsigned int phy_start = 0x200000;
    
    phypool.phyaddr_start = phy_start;
    phypool.pool_size = free_mem;
    phypool.phyaddr_bmap.bytes_len = bmap_length;
    phypool.phyaddr_bmap.base = (unsigned char*)0x9000;

    init_bitmap(&phypool.phyaddr_bmap);

    /* Initialise the virtaul address bitmap. */
    initproc_vir.vaddr_start = 0x200000;
    initproc_vir.vaddr_bmap.bytes_len = bmap_length;
    initproc_vir.vaddr_bmap.base = (unsigned char*)(0x9000 + bmap_length);

    init_bitmap(&initproc_vir.vaddr_bmap);

    /* Just for test. */
    // void *addr = malloc_page(1);
	// printf("malloc is %x", (unsigned int)addr);
	// free_page(addr, 1);
    // addr = malloc_page(1);
	// printf("malloc is %x", (unsigned int)addr);
}

static unsigned int get_mem_capacity()
{
   return *(unsigned int*)0x538; 
}

static void* get_vir_page(unsigned int cnt)
{
    int idx_start = -1;
    int vaddr_start = 0;
    idx_start = bitmap_scan(&initproc_vir.vaddr_bmap, cnt);
    if (-1 == idx_start)
        return NULL;
    
    unsigned int cnt_idx = 0;
    while (cnt_idx < cnt)
        bitmap_set_bit(&initproc_vir.vaddr_bmap, idx_start+(cnt_idx++), 1);

    vaddr_start = initproc_vir.vaddr_start + idx_start*paging::page_size; 

    return (void*)vaddr_start; 
}

static void* get_phy_page()
{
    int idx_free = bitmap_scan(&phypool.phyaddr_bmap, 1);

    if (-1 == idx_free)
        return NULL;

    bitmap_set_bit(&phypool.phyaddr_bmap, idx_free, 1);
    unsigned int phyaddr = phypool.phyaddr_start + idx_free*paging::page_size; 
    return (void*)phyaddr;
}

unsigned int* vaddr2pde(unsigned int vaddr)
{
    unsigned int *pde = (unsigned int*)((0xfffff000) + \
            4 * ((vaddr&0xffc00000) >> 22));
    return pde;
}

unsigned int* vaddr2pte(unsigned int vaddr)
{
    unsigned int *pte = (unsigned int*)(0xffc00000 + \
            ((vaddr&0xffc00000)>>10) + \
            4 * ((vaddr&0x003ff000)>>12)); 
}

unsigned int vaddr2phy(unsigned int vaddr)
{
	unsigned int *pte = vaddr2pte(vaddr);
	return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}

static void map_vir_phy(unsigned int viraddr, unsigned int phyaddr)
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

void* malloc_page(unsigned int cnt)
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

static void free_phy_page(unsigned int phyaddr)
{
	unsigned int idx = (phyaddr - phypool.phyaddr_start ) / paging::page_size;
	bitmap_set_bit(&phypool.phyaddr_bmap, idx, 0);
}

static void free_vir_page(unsigned int viraddr, unsigned int cnt)
{
	unsigned int cnt_idx = 0;
	unsigned idx = (viraddr - initproc_vir.vaddr_start) / paging::page_size;

	while (cnt_idx < cnt)
		bitmap_set_bit(&initproc_vir.vaddr_bmap, idx+(cnt_idx++), 0);
}

static void remove_map_addr(unsigned int viraddr)
{
	unsigned int *pte = vaddr2pte(viraddr);
	*pte &= ~paging::P;
	asm volatile ("invlpg %0" :: "m" (viraddr) : "memory");
}

void free_page(void *viraddr, unsigned int cnt)
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

