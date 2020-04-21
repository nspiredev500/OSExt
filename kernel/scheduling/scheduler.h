#ifndef SCHEDULER_H
#define SCHEDULER_H

const uint32_t SCHEDULER_OK;
const uint32_t SCHEDULER_DATA_ABORT;
const uint32_t SCHEDULER_PREFETCH_ABORT;
const uint32_t SCHEDULER_BREAKPOINT;
const uint32_t SCHEDULER_UNDEFINED_INSTRUCTION;
const uint32_t SCHEDULER_OTHER;



// gets or sets the timeslice length in milliseconds
void scheduler_set_timelice_length(uint32_t slice);
uint32_t scheduler_get_timeslice_length();


// makes the scheduler return after the next timeslice
void scheduler_end();

/* 
	runs the scheduler for the  specified amount of timeslices
	runs until ended when timeslices = 0
*/
void scheduler_start(uint32_t timeslices);


// returns to the scheduler from the current thread, with the specified exit code
void scheduler_return(uint32_t code);



void scheduler_add(struct process *p);

// remove a process from the scheduler before deleting it
void scheduler_remove_process(struct process *p);
void scheduler_remove_thread(struct process *p,struct thread* t);



// returns the currently running process, or NULL if no one is running
struct process* scheduler_running_process();
// returns the currently running thread, or NULL if no one is running
struct thread* scheduler_running();





























#endif