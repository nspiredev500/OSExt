#include "../kernel.h"



static void action_syscall(struct deferred_action *action);


// get the current action on the top of the list
struct deferred_action* action_get_next(struct deferred_action **queue)
{
	if (queue == NULL)
	{
		return NULL;
	}
	if (*queue == NULL)
	{
		return NULL;
	}
	return *queue;
}


// add an action to the end of the queue
void action_add(struct deferred_action **queue, struct deferred_action *action)
{
	if (queue == NULL)
	{
		return;
	}
	if (*queue == NULL)
	{
		*queue = action;
		return;
	}
	struct irq_state s;
	irq_save_state(&s);
	irq_disable();
	
	
	struct deferred_action *next = *queue;
	while (next->next != NULL)
	{
		next = next->next;
	}
	next->next = action;
	
	irq_restore_state(&s);
}

// executes the next action and removes it from the queue, returns it for freeing, or NULL if the queue was empty
struct deferred_action* action_do_next(struct deferred_action **queue)
{
	if (queue == NULL)
	{
		return NULL;
	}
	struct irq_state s;
	irq_save_state(&s);
	irq_disable();
	
	if (*queue == NULL)
	{
		return NULL;
	}
	
	struct deferred_action *action = *queue;
	
	
	
	if (action->type == 1)
	{
		action_syscall(action);
	}
	if (action->type == 2)
	{
		void (*func)(void) = action->function.address;
		func();
	}
	
	
	irq_restore_state(&s);
	return action;
}






static void action_syscall(struct deferred_action *action)
{
	
}





















