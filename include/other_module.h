#ifndef __INCLUDE_OTHER_MODULE_H__
#define __INCLUDE_OTHER_MODULE_H__

namespace {
namespace all_processes {
	struct pcb *FS_PCB = (struct pcb*)0x90000;
	struct pcb *MM_PCB = (struct pcb*)0x91000;
	struct pcb *DR_PCB = (struct pcb*)0x92000;
	const unsigned int ANY = 0x0;
	const unsigned int FS = 0x90000;
	const unsigned int MM = 0x91000;
	const unsigned int DR = 0x92000;
};
};

void memory_manager();
void filesystem_manager();
void disk_driver();
void init_keyboard();

#endif

