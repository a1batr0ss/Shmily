#ifndef __TERMINAL_TERMINAL_H__
#define __TERMINAL_TERMINAL_H__

#include "user.h"

struct ring_buffer;

class Terminal {
  private:
	struct user cur_user;
	char cur_dir[64];
	char line[128];
	unsigned char line_idx;
	struct ring_buffer *keyboard_buf;
	char* argv[9];
	unsigned char argc;

	void print_login_interface();
	void print_shell();
	void format_input();
	void handle_input();
	void reset_terminal();
	int login();
	void init_screen();
	void run();
	void user_login();
	void record_to_log();
	void tell_fs();
	void exit();
	void path_relative2abs(char *realtive_path, char *abs_path);

  public:
	Terminal(struct ring_buffer *rb);
	void start();
};

#endif

