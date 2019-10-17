#include "page.h"
#include "interrupt.h"

int main()
{
    enable_paging();
    init_intr();

    enable_intr();

    while (1);

    return 0;
}

