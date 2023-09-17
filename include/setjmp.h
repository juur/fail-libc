#ifndef _SETJMP_H
#define _SETJMP_H

#include <features.h>

typedef unsigned long jmp_buf[8];
typedef jmp_buf sigjmp_buf;

void   _longjmp(jmp_buf, int);
void   longjmp(jmp_buf, int);
void   siglongjmp(sigjmp_buf, int);
int    _setjmp(jmp_buf);
int    setjmp(jmp_buf);
int    sigsetjmp(sigjmp_buf, int);

#endif
