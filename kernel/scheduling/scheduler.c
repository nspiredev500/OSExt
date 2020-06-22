#include "../kernel.h"


const uint32_t SCHEDULER_OK = 0;
const uint32_t SCHEDULER_DATA_ABORT = 1;
const uint32_t SCHEDULER_PREFETCH_ABORT = 2;
const uint32_t SCHEDULER_BREAKPOINT = 3;
const uint32_t SCHEDULER_UNDEFINED_INSTRUCTION = 4;
const uint32_t SCHEDULER_OTHER = 5;



static volatile uint32_t timeslice_length = 5;
static volatile bool sched_stop = false;


static struct svc_thread running_kernel_thread = NULL; // is set to the main thread when starting scheduling. Also acts as a linkedlist head for all other kernel threads. The threads are in a circular linked list





static struct thread* running_thread = NULL;
static struct thread_return_desc return_thread;


static struct process* running_process = NULL;

static void schedule();


// gets or sets the timeslice length in milliseconds
void scheduler_set_timelice_length(uint32_t slice)
{
	timeslice_length = slice;
}

uint32_t scheduler_get_timeslice_length()
{
	return timeslice_length;
}




// disables irqs and enables the timeslice timer fiq
void scheduler_enable_kernel_thread_scheduling()
{
	if (running_kernel_thread != NULL)
	{
		return;
	}
	disableIRQ();
	struct svc_thread *t = create_svc_thread(false, NULL, 1024*128, NULL); // most of these values don't matter for the main thread
	t->main = true;
	t->next = t; // close the list
	running_kernel_thread = t;
	
	
	
	
	
	
}

// has to be called before returning to the OS after a program exited,
// because the OS doesn't work with irqs disabled and context switches in other nucleus threads could have fatal consequences
void scheduler_disable_kernel_thread_scheduling()
{
	if (running_kernel_thread == NULL)
		return;
	}
	if (running_kernel_thread->main == false) // you can only exit scheduling from the main thread
	{
		return;
	}
	
	
	
	
	
	enableIRQ();
}


struct svc_thread* scheduler_add_kernel_thread()
{
	
	
	
	
	
}


void scheduler_remove_kernel_thread(struct svc_thread *t)
{
	
	
	
	
	
}



// makes the scheduler return after the next timeslice
void scheduler_end()
{
	sched_stop = true;
}

/* 
	runs the scheduler for the  specified amount of timeslices
	runs until ended when timeslices = 0
*/
void scheduler_start(uint32_t timeslices)
{
	if (timeslices == 0)
	{
		while (! sched_stop)
		{
			schedule();
		}
	}
	for (uint32_t i = 0;i<timeslices;i++)
	{
		schedule();
		if (sched_stop)
		{
			break;
		}
	}
}



// returns to the scheduler from the current thread, with the specified exit code
void scheduler_return(uint32_t code)
{
	returnThread(&return_thread,code);
}




void scheduler_add(struct process *p)
{
	
	
	
	
	
	
}



// remove a process from the scheduler before deleting it
void scheduler_remove_process(struct process *p)
{
	// extra case if the process is the running process
	// TODO scan the deferred actions for the process if it was blocked, and remove the action
	
	
	
	
}

// remove a thread from the scheduler before deleting it
void scheduler_remove_thread(struct process *p,struct thread* t)
{
	// extra case if the process is the running process
	
	
	
	
}






// returns the currently running process, or NULL if no one is running
struct process* scheduler_running_process()
{
	return running_process;
}

// returns the currently running thread, or NULL if no one is running
struct thread* scheduler_running()
{
	return running_thread;
}



static void schedule()
{
	
	
	
	
	
	
	
	
}























