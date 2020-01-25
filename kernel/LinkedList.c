#include "kernel.h"




void addLinkedListEntry(LinkedList **list, LinkedList *e)
{
	if (*list == NULL)
	{
		*list = e;
		return;
	}
	LinkedList *next = *list;
	while (next->next != NULL)
	{
		next = next->next;
	}
	next->next = e;
}


// return true if the cache entry was found (and removed)
bool removeLinkedListEntry(LinkedList **list, LinkedList *e)
{
	if (*list == e)
	{
		*list = (*list)->next;
		return true;
	}
	LinkedList *next = *list;
	while (next->next != NULL)
	{
		if (next->next == e)
		{
			next->next = (next->next)->next; // override the next value so it points after the removed entry
			return true;
		}
		next = next->next;
	}
	return false;
}


























