#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT 8
#define UCHAR_MAX 255
#define SCHAR_MAX 127
#define SCHAR_MIN -127
#define CHAR_MIN 0
#define CHAR_MAX SCHAR_MAX

#define _XOPEN_PATH_MAX 1024
#define _XOPEN_NAME_MAX 255
#define _POSIX_PIPE_BUF 512

#define PATH_MAX _XOPEN_PATH_MAX
#define NAME_MAX _XOPEN_NAME_MAX
#define PIPE_BUF _POSIX_PIPE_BUF

#endif

