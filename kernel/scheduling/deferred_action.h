#ifndef DEFERRED_ACTION_H
#define DEFERRED_ACTION_H


// TODO make locks and use one in the deferred action
//      locks should use reference counting

struct deferred_syscall {
	uint32_t number;
	uint32_t *return_value; // the address where the return value will be put
};

struct deferred_function { // deferred function don't support arguments or return values
	void *address; // the address of the function to call
};

struct deferred_action;

struct deferred_action {
	uint8_t type; // 0 = invalid, 1 = syscall, 2 = function
	union {
		struct deferred_syscall syscall;
		struct deferred_function function;
	};
	uint32_t params[6];
	struct process *proc; // the process that is blocked because this action hasn't happened yet, or NULL
	struct deferred_action* next; // next action in the list, or NULL
};





struct deferred_action* action_get_next(struct deferred_action **queue); // get the current action on the top of the list
void action_add(struct deferred_action **queue, struct deferred_action *action); // add an action to the end of the queue
struct deferred_action* action_do_next(struct deferred_action **queue); // executes the next action and removes it from the queue, returns it for freeing, or NULL if the queue was empty







#endif