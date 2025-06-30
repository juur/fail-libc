#ifndef _LIMITS_H
#define _LIMITS_H

#include <features.h>

#define UCHAR_MAX 0xff
#define SCHAR_MAX 0x7f
#define SCHAR_MIN (-SCHAR_MAX - 1)

#define CHAR_BIT 8
#define CHAR_MIN 0
#define CHAR_MAX SCHAR_MAX

#define WORD_BIT 32
#define INT_MAX 0x7FFFFFFF
#define INT_MIN (-INT_MAX - 1)

#define UINT_MAX 0xffffffffU

#define LONG_BIT 64
#define LONG_MAX  0x7fffffffffffffffL
#define LONG_MIN  (-LONG_MAX - 1L)

#define LLONG_MAX 0x7fffffffffffffffLL
#define LLONG_MIN (-LLONG_MAX - 1LL)

#define ULONG_MAX  0xffffffffffffffffUL
#define ULLONG_MAX 0xffffffffffffffffULL

#define MB_LEN_MAX 4

#define SHRT_MAX 0x7fff
#define SHRT_MIN (-SHRT_MAX - 1)
#define USHRT_MAX 0xffff

#define SSIZE_MAX LONG_MAX

#define _POSIX2_LINE_MAX                      2048
#define _POSIX_AIO_LISTIO_MAX                 2
#define _POSIX_AIO_MAX                        1
#define _POSIX_ARG_MAX                        4096
#define _POSIX_CHILD_MAX                      25
#define _POSIX_DELAYTIMER_MAX                 32
#define _POSIX_HOST_NAME_MAX                  255
#define _POSIX_LINK_MAX                       8
#define _POSIX_LOGIN_NAME_MAX                 9
#define _POSIX_MAX_CANON                      255
#define _POSIX_MAX_INPUT                      255
#define _POSIX_MQ_OPEN_MAX                    8
#define _POSIX_MQ_PRIO_MAX                    32
#define _POSIX_NAME_MAX                       14
#define _POSIX_NGROUPS_MAX                    8
#define _POSIX_OPEN_MAX                       20
#define _POSIX_PATH_MAX                       256
#define _POSIX_PIPE_BUF                       512
#define _POSIX_RE_DUP_MAX                     255
#define _POSIX_RTSIG_MAX                      8
#define _POSIX_SEM_NSEMS_MAX                  256
#define _POSIX_SEM_VALUE_MAX                  32767
#define _POSIX_SIGQUEUE_MAX                   32
#define _POSIX_SSIZE_MAX                      32767
#define _POSIX_SS_REPL_MAX                    4
#define _POSIX_STREAM_MAX                     8
#define _POSIX_SYMLINK_MAX                    255
#define _POSIX_SYMLOOP_MAX                    8
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS   4
#define _POSIX_THREAD_KEYS_MAX                128
#define _POSIX_THREAD_THREADS_MAX             64
#define _POSIX_TIMER_MAX                      32
#define _POSIX_TTY_NAME_MAX                   9
#define _POSIX_TZNAME_MAX                     6

#define _POSIX2_BC_BASE_MAX          99
#define _POSIX2_BC_DIM_MAX           2048
#define _POSIX2_BC_SCALE_MAX         99
#define _POSIX2_BC_STRING_MAX        1000
#define _POSIX2_CHARCLASS_NAME_MAX   14
#define _POSIX2_COLL_WEIGHTS_MAX     2
#define _POSIX2_EXPR_NEST_MAX        32
#define _POSIX2_LINE_MAX             2048
#define _POSIX2_RE_DUP_MAX           255

#define _XOPEN_IOV_MAX               16
#define _XOPEN_NAME_MAX              255
#define _XOPEN_PATH_MAX              1024

#define ARG_MAX          _POSIX_ARG_MAX
#define ATEXIT_MAX       32
#define CHILD_MAX        _POSIX_CHILD_MAX
#define HOST_NAME_MAX    _POSIX_HOST_NAME_MAX
#define LINE_MAX         _POSIX2_LINE_MAX
#define LOGIN_NAME_MAX   16
#define NAME_MAX         _XOPEN_NAME_MAX
#define NGROUPS_MAX      _POSIX_NGROUPS_MAX
#define OPEN_MAX         _POSIX_OPEN_MAX
#define PAGESIZE         0x1000
#define PAGE_SIZE        PAGESIZE
#define PATH_MAX         _XOPEN_PATH_MAX
#define PIPE_BUF         _POSIX_PIPE_BUF
#define TTY_NAME_MAX     _POSIX_TTY_NAME_MAX
#define TZNAME_MAX       _POSIX_TZNAME_MAX

#endif
