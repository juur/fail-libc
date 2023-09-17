#ifndef STDDEF_H
#define STDDEF_H

#include <features.h>

#define NULL (void *)(0)
#define offsetof(st, m) \
    __builtin_offsetof(st, m)

#include <sys/types.h>

#endif
