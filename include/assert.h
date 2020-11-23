#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
# define assert(ignore) ((void) 0)
#else
# define assert(expr) ((expr) ? ((void) 0) : __assert_fail(#expr,__FILE__,__LINE__,__func__))
extern void __assert_fail(char *assertion, char *file, int line, const char *func);
#endif

#endif
