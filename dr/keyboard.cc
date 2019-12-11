#include <io.h>
#include <syscall.h>
#include <ipc_glo.h>
#include <print.h>
#include "keyboard.h"
#include "../include/intr.h"

namespace keyboard {
	unsigned int intr_nr = 0x21;

	unsigned short port = 0x60;

	unsigned short shift_l_make = 0x2a;
	unsigned short shift_r_make = 0x36;
	unsigned short alt_l_make = 0x38;
	unsigned short alt_r_make = 0xe038;
	unsigned short alt_r_break = 0xe0b8;
	unsigned short ctrl_l_make = 0x1d;
	unsigned short ctrl_r_make = 0xe01d;
	unsigned short ctrl_r_break = 0xe09d;
	unsigned short caps_lock_make = 0x3a;

	char keymap[][2] = {
		/* 0x00 */	{0,	0},		
		/* 0x01 */	{'\x1b', '\x1b'},		
		/* 0x02 */	{'1',	'!'},		
		/* 0x03 */	{'2',	'@'},		
		/* 0x04 */	{'3',	'#'},		
		/* 0x05 */	{'4',	'$'},		
		/* 0x06 */	{'5',	'%'},		
		/* 0x07 */	{'6',	'^'},		
		/* 0x08 */	{'7',	'&'},		
		/* 0x09 */	{'8',	'*'},		
		/* 0x0A */	{'9',	'('},		
		/* 0x0B */	{'0',	')'},		
		/* 0x0C */	{'-',	'_'},		
		/* 0x0D */	{'=',	'+'},		
		/* 0x0E */	{'\b', '\b'},	
		/* 0x0F */	{'\t',	'\t'},		
		/* 0x10 */	{'q',	'Q'},		
		/* 0x11 */	{'w',	'W'},		
		/* 0x12 */	{'e',	'E'},		
		/* 0x13 */	{'r',	'R'},		
		/* 0x14 */	{'t',	'T'},		
		/* 0x15 */	{'y',	'Y'},		
		/* 0x16 */	{'u',	'U'},		
		/* 0x17 */	{'i',	'I'},		
		/* 0x18 */	{'o',	'O'},		
		/* 0x19 */	{'p',	'P'},		
		/* 0x1A */	{'[',	'{'},		
		/* 0x1B */	{']',	'}'},		
		/* 0x1C */	{'\r',  '\r'},
		/* 0x1D */	{0, 0},
		/* 0x1E */	{'a',	'A'},		
		/* 0x1F */	{'s',	'S'},		
		/* 0x20 */	{'d',	'D'},		
		/* 0x21 */	{'f',	'F'},		
		/* 0x22 */	{'g',	'G'},		
		/* 0x23 */	{'h',	'H'},		
		/* 0x24 */	{'j',	'J'},		
		/* 0x25 */	{'k',	'K'},		
		/* 0x26 */	{'l',	'L'},		
		/* 0x27 */	{';',	':'},		
		/* 0x28 */	{'\'',	'"'},		
		/* 0x29 */	{'`',	'~'},		
		/* 0x2A */	{0, 0},	
		/* 0x2B */	{'\\',	'|'},		
		/* 0x2C */	{'z',	'Z'},		
		/* 0x2D */	{'x',	'X'},		
		/* 0x2E */	{'c',	'C'},		
		/* 0x2F */	{'v',	'V'},		
		/* 0x30 */	{'b',	'B'},		
		/* 0x31 */	{'n',	'N'},		
		/* 0x32 */	{'m',	'M'},		
		/* 0x33 */	{',',	'<'},		
		/* 0x34 */	{'.',	'>'},		
		/* 0x35 */	{'/',	'?'},
		/* 0x36	*/	{0, 0},	
		/* 0x37 */	{'*',	'*'},    	
		/* 0x38 */	{0, 0},
		/* 0x39 */	{' ',	' '},		
		/* 0x3A */	{0, 0}
		};
};

static bool ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;

void keyboard_handler()
{
	unsigned short scancode = inb(keyboard::port);

  	bool ctrl_down_last = ctrl_status;	  
  	bool shift_down_last = shift_status;
  	bool caps_lock_down_last = caps_lock_status;
  	bool alt_down_last = alt_status;
   	
	int is_breakcode;

	if (0xe0 == scancode) {
		ext_scancode = true;
		return;
	}
	if (ext_scancode) {
		scancode |= 0xe000;
		ext_scancode = false;
	}

	is_breakcode = ((scancode & 0x0080) != 0);

	if (is_breakcode) {
		unsigned short make_code = (scancode &= 0xff7f);
		if (make_code == keyboard::ctrl_l_make || make_code == keyboard::ctrl_r_make)
			ctrl_status = false;
		else if (make_code == keyboard::alt_l_make || make_code == keyboard::alt_r_make)
			alt_status = false;
		else if (make_code == keyboard::shift_l_make || make_code == keyboard::shift_r_make)
			shift_status = false;
		return;
	} else if ((scancode>0x00 && scancode<0x3b) || \
			(scancode == keyboard::alt_r_make) || (scancode == keyboard::ctrl_r_make)) {
		bool shift = false;

		if ((scancode < 0x0e) || (scancode == 0x29) || (scancode == 0x1a) || \
				(scancode == 0x1b) || (scancode == 0x2b) || (scancode == 0x27) \
				|| (scancode == 0x28) || (scancode == 0x33) || \
				(scancode == 0x34) || (scancode == 0x35)) {
				if (shift_down_last)
					shift = true;
		} else {
			if (shift_down_last && caps_lock_down_last)
				shift = false;
			else if (shift_down_last || caps_lock_down_last)
				shift = true;
			else
				shift = false;
		}

		unsigned char index = (scancode &= 0x00ff);
		char cur_char = keyboard::keymap[index][shift];

		if (cur_char) {
			putchar(cur_char);
			return;
		}
		
		if (scancode == keyboard::ctrl_r_make || scancode == keyboard::ctrl_l_make)
			ctrl_status = true;
		else if (scancode == keyboard::shift_r_make || scancode == keyboard::shift_l_make)
			shift_status = true;
		else if (scancode == keyboard::alt_r_make || scancode == keyboard::alt_l_make)
			alt_status = true;
		else if (scancode == keyboard::caps_lock_make)
			caps_lock_status = !(caps_lock_status);
	} else {
		putstring("unknown key.\n");
	}
}

void init_keyboard()
{
    Message msg(all_processes::DR); 
    struct _context con;
    
  	/* Register the keyboard handler. */
    con.con_1 = 0x21;
    con.con_2 = (unsigned int)keyboard_handler;
    msg.reset_message(kr::REGR_INTR, con);
    msg.send_then_recv(all_processes::KR);
}

