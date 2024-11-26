#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

static void *thread_start(void *arg __attribute__((unused)))
{
	printf("getpid=%d\ngettid=%d\n", getpid(), gettid());
	printf("pthread_self=%p\n", pthread_self());

	return NULL;
}

int main(void)
{
	pthread_t id;
	printf("getpid=%d\ngettid=%d\n", getpid(), gettid());
	printf("pthread_self=%p\n", pthread_self());

	if (pthread_create(&id, NULL, thread_start, NULL))
		err(EXIT_FAILURE, "pthread_create");

	void *ret;

	if (pthread_join(id, &ret))
		err(EXIT_FAILURE, "pthread_join");

    exit(EXIT_SUCCESS);
}
