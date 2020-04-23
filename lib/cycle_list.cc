#include <global.h>
#include "cycle_list.h"
#include <stdio.h>

CycleList::CycleList()
{
	initialize();
}

void CycleList::initialize()
{
	this->head.prev = NULL;
	this->tail.next = NULL;
	this->head.next = &(this->tail);
	this->tail.prev = &(this->head);
}

void CycleList::append(struct cyclelist_elem *elem)
{
	elem->prev = this->tail.prev;
	elem->next = &(this->tail);

	this->tail.prev->next = elem;

	this->tail.prev = elem;
}

void CycleList::push(struct cyclelist_elem *elem)
{
	elem->prev = &(this->head);
	elem->next = this->head.next;

	this->head.next->prev = elem;

	this->head.next = elem;
}

void CycleList::remove(struct cyclelist_elem *elem)
{
	elem->next->prev = elem->prev;
	elem->prev->next = elem->next;
}

struct cyclelist_elem* CycleList::pop()
{
	struct cyclelist_elem *ret = this->head.next;
	remove(ret);

	return ret;
}

bool CycleList::is_empty()
{
	return this->head.next == &(this->tail);
}

int CycleList::get_length()
{
	int i = 0;
	for (struct cyclelist_elem *elem=this->head.next; elem!=&(this->tail); elem=elem->next, i++)
		// printf("In");
		;

	return i;
}

struct cyclelist_elem* CycleList::get_elem(int idx)
{
	if (idx > get_length()-1)
		return NULL;

	struct cyclelist_elem *elem = this->head.next;
	for (int i=0; i<idx; i++) ;

	return elem;
}

struct cyclelist_elem* CycleList::next_elem(struct cyclelist_elem *cur_elem)
{
	if (cur_elem->next == &(this->tail))
		return this->head.next;
	else
		return cur_elem->next;
}

