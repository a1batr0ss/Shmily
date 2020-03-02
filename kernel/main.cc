#include <global.h>
#include <ipc_glo.h>
#include <stdio.h>
#include "page.h"
#include "interrupt.h"
#include "process.h"
#include "sync.h"
#include "timer.h"
#include "cmos.h"
#include "tss.h"

void kernel_work();

int main()
{
    enable_paging();
    init_intr();
	init_systime();
	install_tss();

    deal_init_process();  /* The init process. */
    start_process("idle", 12, (void (*)(void*))idle_process, NULL, (struct pcb*)0x99000);
	start_userprocess("mm", 32, (void (*)(void*))0x20000, NULL, all_processes::MM_PCB);
	start_userprocess("fs", 32, (void (*)(void*))0x60000, NULL, all_processes::FS_PCB);
	start_userprocess("dr", 32, (void (*)(void*))0x40000, NULL, all_processes::DR_PCB);
	start_userprocess("net", 32, (void (*)(void*))0x50000, NULL, all_processes::NET_PCB);
	start_userprocess("terminal", 32, (void (*)(void*))0x30000, NULL, all_processes::TER_PCB);
	start_process("kernel", 32, (void (*)(void*))kernel_work, NULL, all_processes::KR_PCB);

    enable_intr();

    while (1);

    return 0;
}

void kernel_work()
{
	Message msg(all_processes::KR);

	while (1) {
		msg.receive(all_processes::ANY);
		switch (msg.get_type()) {
		case kr::REGR_INTR:
		{
			unsigned int intr_nr = msg.get_context().con_1;
			unsigned int handler_addr = msg.get_context().con_2;
			register_intr_handler(intr_nr, (void (*)(void))handler_addr);
			msg.reply();

			break;
		}
		case kr::SLEEP:
		{
			unsigned int seconds = msg.get_context().con_1;
			printf("will sleep %x seconds.\n", seconds);

			/* Not sure it's the best way as the kernel process will sleep.(Others can still work.) */
			sleep_seconds(seconds);
			msg.reply();

			break;
		}
		case kr::GET_TIME:
		{
			struct time cur_time = get_cmos_time();
			struct _context con_ret;
			con_ret.con_1 = (cur_time.second) | (cur_time.minute << 8) |
							(cur_time.hour << 16) | (cur_time.day << 24);
			con_ret.con_2 = cur_time.month & 0xff;
			con_ret.con_3 = cur_time.year;

			msg.reset_message(1, con_ret);
			msg.reply();

			break;
		}
		case kr::CMD_PS:
		{
			ps();
			msg.reply();

			break;
		}
		default:
		{
			printf("default.\n");
		}
		}
	}
}
