#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LinkedList LinkedList;
typedef struct LinkedList {
	void* data;
	LinkedList *next;
} LinkedList;



LinkedList* searchLinkedListEntry(LinkedList **list,void* data,uint32_t *index);
LinkedList* getLinkedListEntry(LinkedList **list,uint32_t index);
void destroyLinkedList(LinkedList **list);

















#endif