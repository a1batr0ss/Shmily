#include <stdio.h>
#include "syscall.h"

Message::Message(unsigned int source_) : source(source_) {}

Message::Message(unsigned int source_, int type_, int context_)
{
	source = source_;
	type = type_;
	context = context_;
}

unsigned int Message::get_source()
{
	return source;
}

int Message::get_context()
{
	return context;
}

int Message::get_type()
{
	return type;
}

unsigned int Message::get_destination()
{
	return destination;
}

void Message::set_context(int context_)
{
	context = context_;
}

void Message::reset_message(int type_, int context_)
{
	type = type_;
	context = context_;
}

void Message::send(unsigned int dest)
{
	asm volatile ("int $0x99" : : "a" (user_ipc::SEND), "b" (this), "c" (dest));
}

void Message::receive(unsigned int want_whose_msg)
{
	asm volatile ("int $0x99" : : "a" (user_ipc::RECV), "b" (this), "c" (want_whose_msg));
}

void Message::reply()
{
	send(destination);
}

void Message::send_then_recv(unsigned int dest)
{
}

