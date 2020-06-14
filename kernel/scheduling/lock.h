#ifndef LOCK_H
#define LOCK_H

/*
	Usage:
	lock = lock_create();
	newlock = lock_reference(lock);
	
	lock_unreference(newlock);
	lock_unreference(lock); // now the lock gets freed, because there are no more references to it
	
	
*/



struct lock
{
	uint32_t lock_count; // how many times it has been locked
	uint32_t ref_count; // how many times it has been referenced
};



struct lock* lock_create();
struct lock* lock_reference(struct lock* lock);
void lock_unreference(struct lock* lock);
void lock_lock(struct lock* lock);
void lock_unlock(struct lock* lock);
bool lock_locked(struct lock* lock);


















#endif