#ifndef DEFERRED_ACTION_H
#define DEFERRED_ACTION_H



struct deferred_syscall {
	uint32_t number;
	
};

struct deferred_action;

struct deferred_action {
	uint8_t type; // 0 = invalid, 1 = syscall
	union {
		struct deferred_syscall syscall;
	};
	uint32_t params[6];
	struct process *proc; // the process that is blocked because this action hasn't happened yet, or NULL
	struct deferred_action* next; // next action in the list, or NULL
};





struct action_get_next(struct deferred_action **queue); // get the current action on the top of the list
struct action_delete(struct deferred_action **queue); // always call after finished processing the action, to advance to the next one









#endif