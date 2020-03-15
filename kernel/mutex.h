#ifndef MUTEX_H
#define MUTEX_H




void unlockMutex(uint32_t *mutex);
// returns false if the mutex is locked and true if the mutex wasn't locked and was acquired
bool tryMutex(uint32_t *mutex);











#endif