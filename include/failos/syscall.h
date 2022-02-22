#ifndef FAILOS_SYSCALL_H
#define FAILOS_SYSCALL_H

#define __NR_read           0
#define __NR_write          1
#define __NR_open           2
#define __NR_close          3
#define __NR_stat           4
#define __NR_mmap           9
#define __NR_brk           12
#define __NR_ioctl         16
#define __NR_access        21
#define __NR_nanosleep     35
#define __NR_getpid        39
#define __NR_clone         56
#define __NR_fork          57
#define __NR_execve        59
#define __NR_exit          60
#define __NR_kill          62
#define __NR_chown         92
#define __NR_lchown        94
#define __NR_gettimeofday  96
#define __NR_getuid       102
#define __NR_getgid       104
#define __NR_geteuid      107
#define __NR_getegid      108
#define __NR_arch_prctl   158   
#define __NR_gettid       186
#define __NR_getdents64   217
#define __NR_exit_group   231

#endif
