#include <stdio.h>
#include "syscall.h"

UserMessage::UserMessage(unsigned int source_) : source(source_) {}

UserMessage::UserMessage(unsigned int source_, int type_, int context_)
{
	source = source_;
	type = type_;
	context = context_;
}

unsigned int UserMessage::get_source()
{
	return source;
}

unsigned int UserMessage::get_context()
{
	return context;
}

void UserMessage::set_context(unsigned int context_)
{
	context = context_;
}

void UserMessage::send(unsigned int dest)
{
	asm volatile ("int $0x99" : : "a" (user_ipc::SEND), "b" (source), "c" (dest), "d" (&context));
}

void UserMessage::recv(unsigned int want_whose_msg)
{
	asm volatile ("int $0x99" : : "a" (user_ipc::RECV), "b" (source), "c" (want_whose_msg), "d" (&context));
}

