#ifndef __INCLUDE_IPC_GLO_H__
#define __INCLUDE_IPC_GLO_H__

struct pcb;

namespace {
namespace all_processes {
	struct pcb *MM_PCB = (struct pcb*)0x91000;
	struct pcb *FS_PCB = (struct pcb*)0x92000;
	struct pcb *KR_PCB = (struct pcb*)0x93000;
	struct pcb *DR_PCB = (struct pcb*)0x94000;
	struct pcb *NET_PCB = (struct pcb*)0x95000;
	struct pcb *TER_PCB = (struct pcb*)0x96000;
	struct pcb *INTERRUPT_PCB = (struct pcb*)0x98000;
	const unsigned int ANY = 0x0;
	const unsigned int INTERRUPT = 0x98000;
	const unsigned int MM = 0x91000;
	const unsigned int FS = 0x92000;
	const unsigned int KR = 0x93000;
	const unsigned int DR = 0x94000;
	const unsigned int NET = 0x95000;
 	const unsigned int TER = 0x96000;
};

namespace mm {
	const unsigned int MALLOC = 1;
	const unsigned int FREE = 2;

	const unsigned int CMD_MEMINFO = 256;
};

namespace dr {
	const unsigned int IDEN = 1;
	const unsigned int OPEN = 2;
	const unsigned int READ = 3;
	const unsigned int WRITE = 4;
	const unsigned int PRINT_PART = 5;
	const unsigned int ASK_DISK = 6;
	const unsigned int SEND_PKT = 7;
	const unsigned int GET_MAC = 8;
	const unsigned int WHERE_KEYBOARD_BUFFER = 9;
};

namespace fs {
	const unsigned int MKDIR = 1;
	const unsigned int RMDIR = 2;
	const unsigned int OPEN_FILE = 3;
	const unsigned int CLOSE_FILE = 4;
	const unsigned int WRITE_FILE = 5;
	const unsigned int READ_FILE = 6;
	const unsigned int MKFILE = 7;
	const unsigned int RMFILE = 8;
	const unsigned int LSEEK_FILE = 9;
	const unsigned int IS_EOF = 10;
	const unsigned int GET_CUR_DIR = 11;
	const unsigned int CUR_USER = 12;

	const unsigned int CMD_PWD = 256;
	const unsigned int CMD_CD = 257;
	const unsigned int CMD_LS = 258;
	const unsigned int CMD_CAT = 259;
	const unsigned int CMD_CPFILE = 260;
	const unsigned int CMD_MVFILE = 261;
	const unsigned int CMD_CHMOD = 262;
	const unsigned int CMD_CHOWN = 263;
};

namespace kr {
	const unsigned int REGR_INTR = 1;  /* register a interrupt handler. */
	const unsigned int SLEEP = 2;
	const unsigned int GET_TIME = 3;

	const unsigned int CMD_PS = 256;
	const unsigned int CMD_HALT = 257;
	const unsigned int CMD_POWEROFF = 258;
};

namespace net {
	const unsigned int PKT_ARRIVED = 1;
};
};

#endif

