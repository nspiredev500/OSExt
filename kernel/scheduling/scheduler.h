#ifndef SCHEDULER_H
#define SCHEDULER_H



// if not specified, use the second timer of the first timer module for the timeslice timing
#ifndef SCHEDULER_TIMER
	#define SCHEDULER_TIMER 1,1
#endif


extern const uint32_t SCHEDULER_OK;
extern const uint32_t SCHEDULER_DATA_ABORT;
extern const uint32_t SCHEDULER_PREFETCH_ABORT;
extern const uint32_t SCHEDULER_BREAKPOINT;
extern const uint32_t SCHEDULER_UNDEFINED_INSTRUCTION;
extern const uint32_t SCHEDULER_OTHER;



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



bool scheduler_is_kernel_scheduling();
// disables irqs and enables the timeslice timer fiq
void scheduler_enable_kernel_thread_scheduling();


struct svc_thread* scheduler_get_running_kernel_thread();

// has to be called before returning to the OS after a program exited
// because the OS doesn't work with irqs disabled and context switches in other nucleus threads could have fatal consequences
void scheduler_disable_kernel_thread_scheduling();

void schedule_kernel_thread(volatile uint32_t* regs_arg);
void schedule_kernel_thread_current_saved();

// lets a thread wait for the specified amount of milliseconds, but accuracy is limited by the timeslice length
// regs is from the syscall handler
/// WARNING: only call from the swi handler, as it can modify the syscall count
void scheduler_kernel_thread_wait(struct svc_thread* t,uint32_t millis,volatile uint32_t* regs);
// waits until the specified thread has finished
// regs is from the syscall handler
/// WARNING: only call from the swi handler, as it can modify the syscall count
void scheduler_kernel_thread_wait_thread(struct svc_thread* t,struct svc_thread* wait_for,volatile  uint32_t* regs);

// destroys the current thread and runs the next one
void  scheduler_svc_thread_destroy_self();

void scheduler_add_kernel_thread(struct svc_thread* t);
/// WARNING: only call from the swi handler, as it can modify the syscall count
void scheduler_remove_kernel_thread(struct svc_thread *t);






// returns the currently running process, or NULL if no one is running
struct process* scheduler_running_process();
// returns the currently running thread, or NULL if no one is running
struct thread* scheduler_running();





























#endif