#ifndef __TERMINAL_TERMINAL_H__
#define __TERMINAL_TERMINAL_H__

struct ring_buffer;

class Terminal {
  private:
	char cur_user[32];
	char cur_dir[64];
	char line[128];
	unsigned char line_idx;
	struct ring_buffer *keyboard_buf;

	void print_shell();
	void handle_input();
	void reset_terminal();

  public:
	Terminal(struct ring_buffer *rb);
	void init_screen();
	void run();
};

#endif

