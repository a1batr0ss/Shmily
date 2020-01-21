#include <all_syscall.h>
#include "ne2k.h"

int main()
{
	init_ne2k();

	print_partition_info(1);

	while (1);

	return 0;
}

