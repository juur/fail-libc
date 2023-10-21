#ifndef PTHREAD_H
#define PTHREAD_H

#include <features.h>

/* default is 0 so memset() works */

#define PTHREAD_CANCEL_ENABLE  0
#define PTHREAD_CANCEL_DISABLE 1

#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

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
    pthread_attr_t attrs;
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

#include <time.h>

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
int pthread_attr_destroy(pthread_attr_t *);
int pthread_attr_init(pthread_attr_t *);
int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel(void);

pthread_t pthread_self(void);

#endif
// vim: set ft=c:
