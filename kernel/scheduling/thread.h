#ifndef THREAD_H
#define THREAD_H




struct svc_thread;
struct svc_thread { // kernel-mode threads, exposed via syscalls. OSExt itself will use other threads
	volatile uint32_t regs[18]; // 16 registers + cpsr + spsr
	bool main; // if this is the original thread of the nucleus OS. Can't be killed.
	void* stack; // the stack is deallocated with ti_free
	uint32_t stacksize; // size in bytes
	uint16_t status;
	uint32_t waiting; // number of scheduler ticks to wait for, or 0 if not waiting
	
	/// TODO add new fields to the constructor
	struct svc_thread *next;
};

struct svc_thread* create_svc_thread(void* stack, uint32_t stacksize, void* entry);

// make sure the thread isn't running
void destroy_svc_thread(struct svc_thread* thread);

/// WARNING: this function can not be used recursively without using return_from_svc_thread first!
void resume_svc_thread(struct svc_thread* thread);

void return_from_svc_thread(struct svc_thread* thread);






struct thread_return_desc { // effectively a special longjmp to the thread return point
	uint32_t sp; // saves the svc sp
};


struct thread;
struct thread {
	volatile uint32_t regs[17];
	uint16_t tid;
	uint16_t status;
	uint32_t waiting; // number of scheduler ticks to wait for, or 0 if not waiting
	
	
	struct thread *next;
};


struct thread* createThread(uint16_t tid, uint32_t pc);
void destroyThread(struct thread *t);




// 0 means successfully run until the timeslice completed
// 
uint32_t runThread(struct thread *t,struct thread_return_desc *ret);
void returnThread(struct thread_return_desc *ret,uint32_t return_value);

































#endif