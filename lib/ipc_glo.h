#ifndef __INCLUDE_OTHER_MODULE_H__
#define __INCLUDE_OTHER_MODULE_H__

struct pcb;

namespace {
namespace all_processes {
	struct pcb *MM_PCB = (struct pcb*)0x91000;
	struct pcb *FS_PCB = (struct pcb*)0x92000;
	struct pcb *KR_PCB = (struct pcb*)0x93000;
	struct pcb *DR_PCB = (struct pcb*)0x94000;
	struct pcb *NET_PCB = (struct pcb*)0x95000;
	struct pcb *INTERRUPT_PCB = (struct pcb*)0x98000;
	const unsigned int ANY = 0x0;
	const unsigned int INTERRUPT = 0x98000;
	const unsigned int MM = 0x91000;
	const unsigned int FS = 0x92000;
	const unsigned int KR = 0x93000;
	const unsigned int DR = 0x94000;
	const unsigned int NET = 0x95000;
};

namespace mm {
	const unsigned int MALLOC = 1;
	const unsigned int FREE = 2;
};

namespace dr {
	const unsigned int IDEN = 1;
	const unsigned int OPEN = 2;
	const unsigned int READ = 3;
	const unsigned int WRITE = 4;
	const unsigned int PRINT_PART = 5;
	const unsigned int ASK_DISK = 6;
};

namespace fs {
};

namespace kr {
	const unsigned int REGR_INTR = 1;  /* register a interrupt handler. */
	const unsigned int SLEEP = 2;
};
};

#endif

