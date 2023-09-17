#ifndef _FEATURES_H
#define _FEATURES_H

#if defined(_XOPEN_SOURCE) && _XOPEN_SOURCE == 700
# if !defined(_POSIX_C_SOURCE)
#  define _POSIX_C_SOURCE 200809L
# endif
#endif

#endif
