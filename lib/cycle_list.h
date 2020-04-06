#ifndef __LIB_CYCLE_LIST_H__
#define __LIB_CYCLE_LIST_H__

struct cyclelist_elem {
	struct cyclelist_elem *prev;
	struct cyclelist_elem *next;
};

class CycleList {
  private:
	struct cyclelist_elem head;
	struct cyclelist_elem tail;


  public:
	CycleList();
	void initialize();  /* For global variables. */
	void append(struct cyclelist_elem *elem);  /* Add the element at tail. */
	void remove(struct cyclelist_elem *elem);
	void push(struct cyclelist_elem *elem);  /* Add the element at head. */
	struct cyclelist_elem* pop();  /* Add the element at head. */
	bool is_empty();
	int get_length();
	struct cyclelist_elem* get_elem(int idx);
	struct cyclelist_elem* next_elem(struct cyclelist_elem *cur_elem);
};

#endif

