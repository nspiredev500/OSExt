#include "kernel.h"




void addLinkedListEntry(LinkedList **list, LinkedList *e)
{
	if (*list == NULL)
	{
		*list = e;
		return;
	}
	e->next = *list;
	*list = e;
}


// return true if the entry was found (and removed)
bool removeLinkedListEntry(LinkedList **list, LinkedList *e)
{
	if (*list == e)
	{
		*list = (*list)->next;
		return true;
	}
	LinkedList *next = *list;
	while (next != NULL)
	{
		if (next->next == e)
		{
			if (next->next == NULL)
			{
				next->next = NULL;
				return true;
			}
			next->next = (next->next)->next; // override the next value so it points after the removed entry
			return true;
		}
		next = next->next;
	}
	return false;
}

LinkedList* getLinkedListEntry(LinkedList **list,uint32_t index)
{
	LinkedList *next = *list;
	if (next == NULL)
	{
		return NULL;
	}
	for (uint32_t i = 0;i<index;i++)
	{
		next = next->next;
		if (next == NULL)
		{
			return NULL;
		}
	}
	return next;
}


LinkedList* searchLinkedListEntry(LinkedList **list,void* data,uint32_t *index)
{
	int i = 0;
	LinkedList *next = *list;
	while (next != NULL)
	{
		if (next->data == data)
		{
			*index = i;
			return next;
		}
		next = next->next;
		i++;
	}
	*index = 0;
	return NULL;
}


void destroyLinkedList(LinkedList **list)
{
	LinkedList *next = *list;
	LinkedList *last = *list;
	while (next != NULL)
	{
		last = next;
		next = next->next;
		freeLinkedListEntry(last);
	}
}














