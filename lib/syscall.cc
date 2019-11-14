#include "syscall.h"

UserMessage::UserMessage(unsigned int source_) : source(source_) {}

UserMessage::UserMessage(unsigned int source_, int type_, int context_)
{
	source = source_;
	type = type_;
	context = context_;
}

void UserMessage::send(unsigned int dest)
{
	asm volatile ("int $0x99" : : "a" (user_ipc::SEND), "b" (this), "c" (dest));
}

void UserMessage::receive(unsigned int want_whose_msg)
{
	asm volatile ("int $0x99" : : "a" (user_ipc::RECV), "b" (this), "c" (want_whose_msg));
}

