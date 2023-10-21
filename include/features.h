#ifndef _FEATURES_H
#define _FEATURES_H

#if defined(_XOPEN_SOURCE)
# if _XOPEN_SOURCE == 700
#  if !defined(_POSIX_C_SOURCE)
#   define _POSIX_C_SOURCE 200809L
#   define _POSIX_SOURCE
#  endif
# elif _XOPEN_SOURCE == 600
#  if !defined(_POSIX_C_SOURCE)
#   define _POSIX_C_SOURCE 200112L
#   define _POSIX_SOURCE
#  endif
# elif _XOPEN_SOURCE == 500
#  if !defined(_POSIX_C_SOURCE)
#   define _POSIX_C_SOURCE 199506L
#   define _POSIX_SOURCE
#  endif
# else
#  error "Unknown value for _XOPEN_SOURCE"
# endif
#endif

#endif

// vim: set ft=c:
