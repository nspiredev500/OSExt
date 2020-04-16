#ifndef THREAD_H
#define THREAD_H


struct thread_return_desc { // effectively a special longjmp to the thread return point
	uint32_t sp; // saves the svc sp
};


struct thread;
struct thread {
	uint32_t regs[17];
	uint16_t tid;
	uint16_t status;
	struct thread *next;
};


struct thread* createThread(uint16_t tid, uint32_t pc);
void destroyThread(struct thread *t);




// 0 means successfully run until the timeslice completed
// 
uint32_t runThread(struct thread *t,struct thread_return_desc *ret);
void returnThread(struct thread_return_desc *ret,uint32_t return_value);

































#endif