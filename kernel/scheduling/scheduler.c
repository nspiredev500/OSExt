#include "../kernel.h"


const uint32_t SCHEDULER_OK = 0;
const uint32_t SCHEDULER_DATA_ABORT = 1;
const uint32_t SCHEDULER_PREFETCH_ABORT = 2;
const uint32_t SCHEDULER_BREAKPOINT = 3;
const uint32_t SCHEDULER_UNDEFINED_INSTRUCTION = 4;
const uint32_t SCHEDULER_OTHER = 5;

static NU_TASK* nuc_gui_task = NULL;

static volatile uint32_t timeslice_length = 2; // in milliseconds
static volatile bool sched_stop = false;


static struct svc_thread *running_kernel_thread = NULL;// is set to the main thread when starting scheduling. Also acts as a linkedlist head for all other kernel threads. The threads are in a circular linked list





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

bool scheduler_is_kernel_scheduling()
{
	if (running_kernel_thread != NULL)
	{
		return true;
	}
	return false;
}

struct svc_thread* scheduler_get_running_kernel_thread()
{
	return running_kernel_thread;
}



// disables irqs and enables the timeslice timer fiq
// regs is the pointer to the register array from the syscall of the calling program
void scheduler_enable_kernel_thread_scheduling()
{
	if (running_kernel_thread != NULL)
	{
		return;
	}
	nuc_gui_task = TCC_Current_Task_Pointer();
	disableIRQ();
	struct svc_thread *t = create_svc_thread(NULL, 1024*128, NULL); // most of these values don't matter for the main thread
	t->main = true;
	t->next = t; // close the list
	running_kernel_thread = t;
	
	
	
	timer_disable(SCHEDULER_TIMER);
	timer_set_prescaler(SCHEDULER_TIMER,0);
	timer_set_mode(SCHEDULER_TIMER,1);
	timer_set_size(SCHEDULER_TIMER,1);
	if (timer_get_clockselect(SCHEDULER_TIMER) != 0xa)
	{
		timer_set_clockselect(SCHEDULER_TIMER,0xa);
	}
	timer_set_load(SCHEDULER_TIMER,32000*timeslice_length);
	timer_set_bg_load(SCHEDULER_TIMER,32000*timeslice_length);
	timer_enable(SCHEDULER_TIMER);
	
	
	
}

// has to be called before returning to the OS after a program exited,
// because the OS doesn't work with irqs disabled and context switches in other nucleus threads could have fatal consequences
void scheduler_disable_kernel_thread_scheduling()
{
	if (running_kernel_thread == NULL)
	{
		return;
	}
	if (running_kernel_thread->main == false) // you can only exit scheduling from the main thread
	{
		return;
	}
	
	timer_disable(SCHEDULER_TIMER);
	
	struct svc_thread *t = running_kernel_thread;
	do
	{
		struct svc_thread *tmp = t->next;
		destroy_svc_thread(t);
		t = tmp;
	}
	while (t != running_kernel_thread);
	running_kernel_thread = NULL;
	
	
	enableIRQ();
}

void schedule_kernel_thread(volatile uint32_t* regs)
{
	// assumes it is in fiq mode with fiqs disabled
	/// TODO switch to svc mode, still with fiqs disabled, to use the stack of the thread in thread_resume
	/// or just use a special scheduler stack for storing the old registers
	/// but what if svc_thread_resume just doesn't return at all? we could just go straight to scheduling another process
	if (running_kernel_thread == NULL)
	{
		return;
	}
	if (TCC_Current_Task_Pointer() != nuc_gui_task)
	{
		return; // for heavens sake, do not context-switch if another nucleus task is running
	}
	if (running_kernel_thread->next == running_kernel_thread)
	{
		return; // only one thread running, no need to context-switch
	}
	register uint32_t spsr_asm asm("r0");
	asm volatile("mrs r0, spsr":"=r" (spsr_asm)::);
	uint32_t spsr = spsr_asm;
	if (PSR_MODE(spsr) != MODE_SVC)
	{
		return; // also don't interrupt other handlers
	}
	{
		struct svc_thread* next = running_kernel_thread;
		while (next != running_kernel_thread)
		{
			if (next->status == SVC_THREAD_WAITING)
			{
				if (next->waiting == 0)
				{
					next->status = SVC_THREAD_RUNNABLE;
				}
				else
				{
					next->waiting--;
					if (next->waiting == 0)
					{
						next->status = SVC_THREAD_RUNNABLE;
					}
				}
			}
			next = next->next;
		}
	}
	
	for (int i = 0;i<=7;i++)
	{
		running_kernel_thread->regs[i] = regs[i+1];
	}
	{ // store all the registers
		register volatile uint32_t* reg8 asm("r0") = running_kernel_thread->regs+8;
		asm volatile(
		"mrs r1, cpsr \n"
		"mov r2, r1 \n"
		"bic r1, r1, #31 \n" // clear the mode bits
		"orr r1, r1, #0b10011 \n" // set the mode to svc
		"str r8, [r0]\n"
		"str r9, [r0, #4]\n"
		"str r10, [r0, #8]\n"
		"str r11, [r0, #12]\n"
		"str r12, [r0, #16]\n"
		"str r13, [r0, #20]\n"
		"str r14, [r0, #24]\n"
		"mrs r3, spsr\n"
		"str r3, [r0, #36]\n" // store the spsr
		"msr cpsr, r2 \n"
		"mrs r2, spsr \n"
		"str r2, [r0, #32]\n" // store the cpsr
		::"r" (reg8):"memory","r1","r2","r3");
		running_kernel_thread->regs[15] = regs[9]-4;
	}
	schedule_kernel_thread_current_saved();
}

void schedule_kernel_thread_current_saved()
{
	if (running_kernel_thread == NULL)
	{
		return;
	}
	if (TCC_Current_Task_Pointer() != nuc_gui_task)
	{
		return; // for heavens sake, do not context-switch if another nucleus task is running
	}
	// TODO skip blocking and sleeping threads
	// if there are only sleeping and blocking threads, wait for the smallest amount of timeslices for the next thread to run and update all waiting times
	// if all threads are blocked waiting for each other, just resume the main thread regardless to prevent a deadlock
	//DEBUGPRINTLN_1("scheduling\n");
	
	struct svc_thread* next = running_kernel_thread;
	do
	{
		if (next->status == SVC_THREAD_RUNNABLE)
		{
			//DEBUGPRINTLN_1("resuming thread: 0x%x\n",next);
			running_kernel_thread = next;
			resume_svc_thread(next);
		}
		next = next->next;
	}
	while (next != running_kernel_thread);
	bool all_blocked = true;
	if (running_kernel_thread->status == SVC_THREAD_WAITING)
	{
		all_blocked = false;
	}
	else
	{
		struct svc_thread* next = running_kernel_thread;
		do
		{
			if (next->status == SVC_THREAD_WAITING)
			{
				all_blocked = false;
				break;
			}
			next = next->next;
		}
		while (next != running_kernel_thread);
	}
	if (all_blocked)
	{
		//DEBUGPRINTLN_1("deadlock detected, resuming main thread despite it's blocked")
		struct svc_thread* next = running_kernel_thread;
		do
		{
			if (next->main)
			{
				//DEBUGPRINTLN_1("resuming thread: 0x%x\n",next);
				running_kernel_thread = next;
				resume_svc_thread(next);
			}
			next = next->next;
		}
		while (next != running_kernel_thread);
		panic("deadlock and no main thread to return to!n");
	}
	else
	{
		uint32_t min_wait = 0xffffffff;
		struct svc_thread* min_wait_thread = NULL;
		struct svc_thread* next = running_kernel_thread;
		do
		{
			if (next->status == SVC_THREAD_WAITING)
			{
				if (next->waiting < min_wait)
				{
					min_wait =  next->waiting;
					min_wait_thread = next;
				}
				else
				{
					if (next->waiting == min_wait && next != running_kernel_thread) // if the current thread and another thread wait for the same time, chose the other one for fairness
					{
						min_wait_thread = next;
					}
				}
			}
			next = next->next;
		}
		while (next != running_kernel_thread);
		if (min_wait_thread == NULL)
		{
			panic("could not find a kernel thread to wait for!\n");
		}
		DEBUGPRINTLN_1("sleeping for %d milliseconds",min_wait*timeslice_length)
		msleep(min_wait*timeslice_length);
		next = running_kernel_thread;
		do // subtract the waited timeslices from all thread's waiting time
		{
			if (next->status == SVC_THREAD_WAITING)
			{
				if (next->waiting <= min_wait)
				{
					next->status = SVC_THREAD_RUNNABLE;
					next->waiting = 0;
				}
				else
				{
					next->waiting -= min_wait;
				}
			}
			next = next->next;
		}
		while (next != running_kernel_thread);
		running_kernel_thread = min_wait_thread;
		resume_svc_thread(min_wait_thread);
	}
	panic("could not find a kernel thread to schedule!\n");
}


// lets a thread wait for the specified amount of milliseconds, but accuracy is limited by the timeslice length
// regs is from the syscall handler
/// WARNING: only call from the swi handler, as it can modify the syscall count
void scheduler_kernel_thread_wait(struct svc_thread* t,uint32_t millis,volatile uint32_t* regs)
{
	if (t == NULL || running_kernel_thread == NULL)
	{
		return;
	}
	uint32_t ticks = millis/timeslice_length;
	if (ticks == 0)
	{
		return;
	}
	if (t == running_kernel_thread)
	{
		ticks++; // the schedule_kernel_thread will subtract one tick
		if (running_kernel_thread->next == running_kernel_thread)
		{
			msleep(millis); // if this thread is the only one, just sleep
			return;
		}
		disableFIQ();
		running_kernel_thread->status = SVC_THREAD_WAITING;
		running_kernel_thread->waiting = ticks;
		for (int i = 0;i<=16;i++)
		{
			//DEBUGPRINTLN_1("thread r%d: 0x%x",i,syscall_get_reg(regs,i));
			running_kernel_thread->regs[i] = syscall_get_reg(regs,i);
		}
		//DEBUGPRINTLN_1("pc: 0x%x",syscall_get_reg(regs,15));
		//DEBUGPRINTLN_1("written pc: 0x%x",running_kernel_thread->regs[15]);
		//DEBUGPRINTLN_1("regs base: 0x%x",running_kernel_thread->regs);
		//DEBUGPRINTLN_1("current thread: 0x%x",running_kernel_thread);
		syscall_mark_as_finished();
		schedule_kernel_thread_current_saved();
	}
	else
	{
		disableFIQ();
		t->status = SVC_THREAD_WAITING;
		t->waiting = ticks;
		enableFIQ();
	}
}
// waits until the specified thread has finished
// regs is from the syscall handler
/// WARNING: only call from the swi handler, as it can modify the syscall count
void scheduler_kernel_thread_wait_thread(struct svc_thread* t,struct svc_thread* wait_for,volatile uint32_t* regs)
{
	if (t == NULL || wait_for == NULL || running_kernel_thread == NULL || t == wait_for)
	{
		return;
	}
	if (t == running_kernel_thread)
	{
		disableFIQ();
		running_kernel_thread->status = SVC_THREAD_WAITING_FOR;
		running_kernel_thread->status_data = wait_for;
		for (int i = 0;i<=16;i++)
		{
			running_kernel_thread->regs[i] = syscall_get_reg(regs,i);
		}
		syscall_mark_as_finished();
		schedule_kernel_thread_current_saved();
	}
	else
	{
		disableFIQ();
		t->status = SVC_THREAD_WAITING_FOR;
		t->status_data = wait_for;
		enableFIQ();
	}
}



void scheduler_add_kernel_thread(struct svc_thread* t)
{
	if (t == NULL || running_kernel_thread == NULL)
	{
		return;
	}
	//DEBUGPRINTLN_1("add thread: 0x%x",t);
	disableFIQ(); // to prevent a context switch
	t->next = running_kernel_thread->next;
	running_kernel_thread->next = t;
	enableFIQ();
}

// destroys the current thread and runs the next one
void scheduler_svc_thread_destroy_self()
{
	if (running_kernel_thread == NULL || running_kernel_thread->main || running_kernel_thread->next == running_kernel_thread)
	{
		return;
	}
	disableFIQ(); // to prevent a context switch
	struct svc_thread* next = running_kernel_thread;
	while (next->next != running_kernel_thread)
	{
		if (next->next->status == SVC_THREAD_WAITING_FOR && next->next->status_data == running_kernel_thread) // release the blocks
		{
			next->next->status = SVC_THREAD_RUNNABLE;
			next->next->status_data = NULL;
		}
		next = next->next;
	}
	next->next = running_kernel_thread->next;
	struct svc_thread* this = running_kernel_thread;
	running_kernel_thread = running_kernel_thread->next;
	destroy_svc_thread(this);
	schedule_kernel_thread_current_saved(); // not actually saved, but the state doesn't matter anymore since it's dead
}

/// WARNING: only call from the swi handler, as it can modify the syscall count
void scheduler_remove_kernel_thread(struct svc_thread *t)
{
	if (t == NULL || running_kernel_thread == NULL || t->main)
	{
		return;
	}
	disableFIQ(); // to prevent a context switch
	if (t == running_kernel_thread)
	{
		syscall_mark_as_finished();
		scheduler_svc_thread_destroy_self();
	}
	else
	{
		struct svc_thread* next = running_kernel_thread;
		do // go through the list
		{
			if (next->next == t)
			{
				next->next = t->next;
				destroy_svc_thread(t);
				struct svc_thread* next = running_kernel_thread;
				do
				{
					if (next->status == SVC_THREAD_WAITING_FOR && next->status_data == t) // release the blocks
					{
						next->status = SVC_THREAD_RUNNABLE;
						next->status_data = NULL;
					}
					next = next->next;
				}
				while (next != running_kernel_thread);
				enableFIQ();
				return;
			}
			next = next->next;
		}
		while (next != running_kernel_thread);
	}
	enableFIQ();
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























