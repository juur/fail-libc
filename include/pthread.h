#ifndef PTHREAD_H
#define PTHREAD_H

#include <features.h>

typedef struct __pthread *	pthread_t;

#include <sys/types.h>

struct __pthread {
	struct __pthread *self;
	int my_tid;
	int parent_tid;
	int errnum;
	void *stack;
	size_t stack_size;
	void *(*start_routine)(void *);
	void *start_arg;
};

typedef struct {
	int dummy;
} pthread_rwlock_t;

typedef struct {
	int dummy;
} pthread_mutex_t;

typedef struct {
	int dummy;
} pthread_mutexattr_t;

typedef struct {
	int dummy;
} pthread_rwlockattr_t;

typedef struct {
	int dummy;
} pthread_attr_t;

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutexattr_t * attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_init(pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
int pthread_join(pthread_t thread, void **retval);

#endif
// vim: set ft=c:
