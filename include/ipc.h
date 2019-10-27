#ifndef __INIT_IPC_H__
#define __INIT_IPC_H__

class Message {
  private:
    unsigned int source;  /* Who send the message. (PCB's first address) */
    int type;
    int context;  /* message content, just for test now. */

  public:
    Message(unsigned int source_);  /* For receiving. */
    Message(unsigned int source_, int type_, int context_);  /* For sending. */
    unsigned int get_source();
    int get_type();
    int get_context();
    void set_source(unsigned int source_);
    void set_type(int type_);
    void set_context(int context_);
    void send(unsigned int dest);
    void receive(unsigned int want_whose_msg);
};

#endif

