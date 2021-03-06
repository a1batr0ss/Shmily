#ifndef __LIB_IPC_H__
#define __LIB_IPC_H__

/* The member can replace as an array. */
struct _context {
	unsigned int con_1;
	unsigned int con_2;
	unsigned int con_3;
	unsigned int con_4;
};

class Message {
  private:
    unsigned int source;  /* Who send the message. (PCB's first address) */
    unsigned int destination;
    int type;
    // int context;  [> message content, just for test now. <]
	struct _context context;

  public:
	Message();
    Message(unsigned int source_);  /* For receiving. */
    Message(unsigned int source_, int type_, struct _context context_);  /* For sending. */
    unsigned int get_source();
    int get_type();
    struct _context get_context();
	unsigned int get_destination();
    void set_source(unsigned int source_);
    void set_type(int type_);
    void set_context(struct _context context_);
    void set_dest(unsigned int dst);
    void send(unsigned int dest);
    void receive(unsigned int want_whose_msg);
    void send_then_recv(unsigned int dest);
    void reset_message(int type_, struct _context context_);
    void reply();
};

namespace user_ipc {
	const unsigned int SEND = 0x1;
	const unsigned int RECV = 0x2;
	const unsigned int SEND_RECV = 0x03;
};

#endif

