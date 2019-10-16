#include "page.h"
#include "print.h"

int main()
{
    enable_paging();

    set_cursor(0);
    putstring("Halou World.");

    while (1);

    return 0;
}

