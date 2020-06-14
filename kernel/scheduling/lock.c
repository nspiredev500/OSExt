#include "../kernel.h"







struct lock* lock_create()
{
	struct lock* lock = kmalloc(sizeof(struct lock));
	if (lock == NULL)
	{
		return NULL;
	}
	lock->lock_count = 0;
	lock->ref_count = 1;
	return lock;
}

struct lock* lock_reference(struct lock* lock)
{
	if (lock == NULL)
	{
		return NULL;
	}
	struct irq_state state;
	irq_save_state(&state);
	irq_disable();
	lock->ref_count++;
	irq_restore_state(&state);
	return lock;
}

void lock_unreference(struct lock* lock)
{
	if (lock == NULL)
	{
		return;
	}
	struct irq_state state;
	irq_save_state(&state);
	irq_disable();
	lock->ref_count--;
	if (lock->ref_count == 0)
	{
		kfree_hint(lock,sizeof(struct lock));
	}
	irq_restore_state(&state);
}

void lock_lock(struct lock* lock)
{
	if (lock == NULL)
	{
		return;
	}
	struct irq_state state;
	irq_save_state(&state);
	irq_disable();
	lock->lock_count++;
	irq_restore_state(&state);
}

void lock_unlock(struct lock* lock)
{
	if (lock == NULL)
	{
		return;
	}
	struct irq_state state;
	irq_save_state(&state);
	irq_disable();
	if (lock->lock_count != 0)
	{
		lock->lock_count--;
	}
	irq_restore_state(&state);
}

bool lock_locked(struct lock* lock)
{
	if (lock == NULL)
	{
		return false;
	}
	if (lock->lock_count != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


