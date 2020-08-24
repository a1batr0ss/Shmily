#include <global.h>
#include <stdio.h>
#include <ipc_glo.h>
#include "ipc.h"
#include "process.h"
#include "print.h"

/* Couldn't assgin 0, it can't get the current process as user process.(No interrupt.) */
Message::Message()
{
    source = (unsigned int)get_current_proc();
}

Message::Message(unsigned int source_) : source(source_) {}

Message::Message(unsigned int source_, int type_, struct _context context_)
{
    source = source_;
    type = type_;
    context = context_;
}

unsigned int Message::get_source()
{
    return source;
}

int Message::get_type()
{
    return type;
}

struct _context Message::get_context()
{
    return context;
}

void Message::set_source(unsigned int source_)
{
    source = source_;
}

unsigned int Message::get_dest()
{
	return destination;
}

void Message::set_type(int type_)
{
    type = type_;
}

void Message::set_context(struct _context context_)
{
    context = context_;
}

void Message::set_dest(unsigned int dst)
{
    destination = dst;
}

void Message::send(unsigned int dest_pid)
{
	struct pcb *dst = (struct pcb*)dest_pid;
    enum process_status dst_status = dst->status;

	// if (((source == 0x95000) || (source == 0x94000) || (source == 0x91000) || (source == 0x98100)))
	// printf("In sending: %x->%x %x, %x, %x %x.\n", source, dst, dst_status, type, dst->message->get_dest(), ((struct pcb*)source)->status);

	if (source == all_processes::INTERRUPT_NET) {
		dst->intr_cnt++;

		/* Unblock the target process, otherwise these interrupts will head up if target is blocking. */
		if ((WAITING_MSG == dst_status) && (all_processes::ANY == dst->message->get_dest())) {
			dst->message->set_type(intr::INTR);
			unblock_proc(dst);
		}

		return;
	}

	/* That girl is waiting me? */
	/* Judge the condition of 'SENDING_MSG==dst_status' may not rational. */
	if ((((WAITING_MSG == dst_status) && ((all_processes::ANY == dst->message->get_dest()) || (source == dst->message->get_dest()))) || ((WAITING_MSG == dst_status) && (common::REPLY == this->type)))) {
        /* Copy the message to dest, not only point to the message. */
        // dst->message->set_source(source); // There would occured a bug.
        dst->message->set_type(type);
        dst->message->set_context(context);
        dst->message->set_dest(source);

        /* Unblock the dest process. */
        unblock_proc(dst);
    } else {
    	if (all_processes::INTERRUPT == source) {
			(all_processes::INTERRUPT_PCB)->next_ready = dst->sendings;
			dst->sendings = all_processes::INTERRUPT_PCB;
		} else {
   			struct pcb *src = (struct pcb*)source;
        	src->message = this;

    	    cur_proc->next_ready = dst->sendings;
	        dst->sendings = cur_proc;

       		/* Block self, and append to dest's sending queue. */
        	self_block(SENDING_MSG);  /* Set the status to BLOCKED tentatively. */
		}
    }
}

void Message::receive(unsigned int want_whose_msg)
{
    struct pcb *src = (struct pcb*)source;
    struct pcb *prev_sender = src->sendings;
    struct pcb *sender = NULL;
    struct pcb *want_whose = (struct pcb*)want_whose_msg;

	if ((0 != src->intr_cnt) && (all_processes::ANY == want_whose_msg)) {
		this->type = intr::INTR;

		src->intr_cnt--;
		return;
	}

    if (NULL != prev_sender) {
        if ((want_whose == prev_sender) || (0 == want_whose)) {
            type = prev_sender->message->get_type();
            context = prev_sender->message->get_context();

			/* Interrupt circle. */
			if (src->sendings->next_ready == src->sendings)
				src->sendings = nullptr;
			else
		        src->sendings = src->sendings->next_ready;

			destination = (unsigned int)prev_sender;
            if (SENDING_MSG == prev_sender->status)
                unblock_proc(prev_sender);
            return;
        }

        /* Havent's test. */
        /* Traverse the sendings queue. */
        while (NULL != prev_sender) {
            sender = prev_sender->next_ready;
            if (want_whose == sender) {
                type = want_whose->message->get_type();
                context = want_whose->message->get_context();

                prev_sender->next_ready = sender->next_ready;

               	destination = (unsigned int)prev_sender;
                if (SENDING_MSG == prev_sender->status)
                    unblock_proc(prev_sender);
                return;
            }
            prev_sender = prev_sender->next_ready;
        }
    }

	destination = want_whose_msg;  // include ANY (0)
    /* Store the message's pointer so that we can assign to the message when sender is sending. */
    src->message = this;
    /* Sendings queue is empty, Block self. */
    self_block(WAITING_MSG);
}

/* The dest also as the want_whose_msg */
void Message::send_then_recv(unsigned int dest)
{
    send(dest);
    receive(dest);
}

void Message::reset_message(int type_, struct _context context_)
{
    type = type_;
    context = context_;
}

void Message::reply()
{
    send(destination);
}

