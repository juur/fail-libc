#ifndef _LIMITS_H
#define _LIMITS_H

#include <features.h>

#define CHAR_BIT 8
#define UCHAR_MAX 255
#define SCHAR_MAX 127
#define SCHAR_MIN -127
#define CHAR_MIN 0
#define CHAR_MAX SCHAR_MAX
#define INT_MAX 2147483647
#define INT_MIN (-2147483648)
#define UINT_MAX 4294967295U
#define LONG_BIT 64
#define LONG_MAX  0x7fffffffffffffffL
#define LONG_MIN  (-LONG_MAX - 1L)
#define LLONG_MAX 9223372036854775807LL
#define LLONG_MIN (-LLONG_MAX - 1L)
#define ULONG_MAX 0xffffffffffffffffUL
#define ULLONG_MAX 0xffffffffffffffffULL

#define MB_LEN_MAX 4

#define _XOPEN_PATH_MAX 1024
#define _XOPEN_NAME_MAX 255
#define _POSIX_PIPE_BUF 512
#define _POSIX_NGROUPS_MAX 8
#define _POSIX_LOGIN_NAME_MAX 9
#define _POSIX_HOST_NAME_MAX 255
#define _POSIX2_LINE_MAX 2048

#define PATH_MAX _XOPEN_PATH_MAX
#define NAME_MAX _XOPEN_NAME_MAX
#define PIPE_BUF _POSIX_PIPE_BUF
#define NGROUPS_MAX _POSIX_NGROUPS_MAX
#define LOGIN_NAME_MAX 16
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#define LINE_MAX _POSIX2_LINE_MAX

#endif
