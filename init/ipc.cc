#include <global.h>
#include "ipc.h"
#include "process.h"
#include "print.h"

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

int Message::get_type()
{
    return type;
}

int Message::get_context()
{
    return context;
}

void Message::set_source(unsigned int source_)
{
    source = source_;
}

void Message::set_type(int type_)
{
    type = type_;
}

void Message::set_context(int context_)
{
    context = context_;
}

void Message::send(unsigned int dest)
{
    struct pcb *dst = (struct pcb*)dest;
    enum process_status dst_status = dst->status;

    if (BLOCKED == dst_status) {
        /* Copy the message to dest, not only point to the message. */
        dst->message->set_source(source);
        dst->message->set_type(type);
        dst->message->set_context(context);
        /* Unblock the dest process. */
        unblock_proc(dst);
    } else {
        struct pcb *src = (struct pcb*)source;
        src->message = this;

        cur_proc->next_ready = dst->sendings;
        dst->sendings = cur_proc;

        /* Block self, and append to dest's sending queue. */
        self_block(SENDING_MSG);  /* Set the status to BLOCKED tentatively. */
    } 
}

void Message::receive(unsigned int want_whose_msg)
{
    struct pcb *src = (struct pcb*)source;
    struct pcb *prev_sender = src->sendings;
    struct pcb *sender = NULL;
    struct pcb *want_whose = (struct pcb*)want_whose_msg;

    if (NULL != prev_sender) {
        if ((want_whose == prev_sender) || (0 == want_whose)) {
            type = prev_sender->message->get_type();
            context = prev_sender->message->get_context();

            src->sendings = src->sendings->next_ready;

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

                if (SENDING_MSG == prev_sender->status)
                    unblock_proc(prev_sender);
                return;
            }
            prev_sender = prev_sender->next_ready;
        } 
    }

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
