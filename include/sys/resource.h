#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <features.h>

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

typedef unsigned int rlim_t;

#include <sys/types.h>

int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int prio);

#endif
