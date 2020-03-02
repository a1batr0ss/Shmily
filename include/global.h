#ifndef __INCLUDE_GLOBAL_H__
#define __INCLUDE_GLOBAL_H__

#define NULL 0

namespace paging {
    const unsigned int page_dir_base = 0x100000;
    const unsigned int page_size = 4096;

    const int US_U = 1 << 2;
    const int US_S = 0 << 2;
    const int RW_R = 0 << 1;
    const int RW_W = 1 << 1;
    const int P = 1;
};


#endif

