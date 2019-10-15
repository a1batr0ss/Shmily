#ifndef __INIT_PAGE_H__
#define __INIT_PAGE_H__

struct pte {
    unsigned int present    :1;
    unsigned int rw         :1;
    unsigned int us         :1;
    unsigned int pwt        :1;
    unsigned int pcd        :1;
    unsigned int accessed   :1;
    unsigned int dirty      :1;
    unsigned int pat_or_ps  :1;
    unsigned int global     :1;
    unsigned int available  :3;
    unsigned int pagebase   :20;
} __attribute__((packed));

struct page_table {
    struct pte pages[1024];
};

namespace paging {
    const unsigned int page_dir_base = 0x100000;
    const unsigned int page_size = 4096;

    const int US_U = 0 << 2;
    const int US_S = 1 << 2;
    const int RW_R = 0 << 1;
    const int RW_W = 1 << 1;
    const int P = 1;
};

void enable_paging();

#endif

