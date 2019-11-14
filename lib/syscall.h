#ifndef __INIT_SYSCALL_H__
#define __INIT_SYSCALL_H__

class UserMessage {
 private:
	unsigned int source;
	unsigned int destination;
	int type;
	int context;

 public:
	UserMessage(unsigned int source_);
	UserMessage(unsigned int source_, int type_, int context_);
	void send(unsigned int dest);
	void receive(unsigned int want_whose_msg);	
};

namespace user_ipc {
	const unsigned int SEND = 0x01;
	const unsigned int RECV = 0x02;
};

#endif

