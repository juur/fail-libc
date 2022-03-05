#ifndef FAILOS_SYSCALL_H
#define FAILOS_SYSCALL_H

#define __NR_read           0
#define __NR_write          1
#define __NR_open           2
#define __NR_close          3
#define __NR_stat           4
#define __NR_lseek          8
#define __NR_mmap           9
#define __NR_brk           12
#define __NR_sigaction     13
#define __NR_ioctl         16
#define __NR_access        21
#define __NR_pause         34
#define __NR_nanosleep     35
#define __NR_getpid        39
#define __NR_accept        43
#define __NR_bind          49
#define __NR_listen        50
#define __NR_clone         56
#define __NR_fork          57
#define __NR_execve        59
#define __NR_exit          60
#define __NR_wait4         61
#define __NR_kill          62
#define __NR_mkdir         83
#define __NR_creat         85
#define __NR_link          86
#define __NR_unlink        87
#define __NR_symlink       88
#define __NR_readlink      89
#define __NR_chmod         90
#define __NR_chown         92
#define __NR_lchown        94
#define __NR_gettimeofday  96
#define __NR_getuid       102
#define __NR_getgid       104
#define __NR_geteuid      107
#define __NR_getegid      108
#define __NR_setpgid      109
#define __NR_getppid      110
#define __NR_getpgrp      111
#define __NR_setsid       112
#define __NR_getpgid      121
#define __NR_getsid       124
#define __NR_utime        132
#define __NR_getpriority  140
#define __NR_setpriority  141
#define __NR_arch_prctl   158   
#define __NR_gettid       186
#define __NR_time         201
#define __NR_getdents64   217
#define __NR_exit_group   231
#define __NR_utimes       235

#endif
