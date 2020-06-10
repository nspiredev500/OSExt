#ifndef LOCK_H
#define LOCK_H


struct lock
{
	uint32_t lock_count; // how many times it has been locked
	uint32_t ref_count; // how many times it has been referenced
}



struct lock* lock_create();
struct lock* lock_reference(struct lock* lock);
void lock_unreference(struct lock* lock);
void lock_lock(struct lock* lock);
void lock_unlock(struct lock* lock);
bool lock_locked(struct lock* lock);


















#endif