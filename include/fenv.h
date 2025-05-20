#ifndef _FENV_H
#define _FENV_H

typedef struct {
    /* needs to mirror the FSTENV instruction */
    unsigned long blah;
} fenv_t;

#endif
