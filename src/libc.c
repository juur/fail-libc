#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"

/* library defines */

#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <failos/syscall.h>
#include <asm/prctl.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <utmpx.h>
#include <termios.h>
#include <utime.h>
#include <sys/ioctl.h>
#include <regex.h>
#include <err.h>
#include <pwd.h>
#include <uchar.h>
#include <sys/socket.h>
#include <stdatomic.h>
#include <sys/statvfs.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/un.h>
#include <iconv.h>
#include <mntent.h>

#define hidden __attribute__((__visibility__("hidden")))

#define ARCH_SET_FS	0x1002

#define CLONE_VM			0x00000100
#define CLONE_FS			0x00000200
#define CLONE_FILES			0x00000400
#define CLONE_SIGHAND		0x00000800
#define CLONE_PTRACE		0x00002000
#define CLONE_VFORK			0x00004000
#define CLONE_PARENT		0x00008000
#define CLONE_THREAD		0x00010000
#define CLONE_NEWNS			0x00020000
#define CLONE_SYSVSEM		0x00040000
#define CLONE_SETTLS		0x00080000
#define CLONE_PARENT_SETTID	0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED		0x00400000
#define CLONE_UNTRACED		0x00800000
#define CLONE_CHILD_SETTID  0x01000000
#define CLONE_NEWCGROUP		0x02000000
#define CLONE_NEWUTS		0x04000000
#define CLONE_NEWIPC		0x08000000
#define CLONE_NEWUSER		0x10000000
#define CLONE_NEWPID		0x20000000
#define CLONE_NEWNET		0x40000000
#define CLONE_IO			0x80000000


/* library typedefs */

/* library structures */

struct atexit_fun {
	struct atexit_fun *next;
	void (*function)(void);
};

#define MEM_MAGIC	0x61666c69

struct mem_alloc {
	uint32_t flags;
	uint32_t magic;
	struct mem_alloc *next;
	struct mem_alloc *prev;
	size_t len;
	void *start;
	void *end;
} __attribute__((packed));

#define MF_FREE (1<<0)

/* global variables */

char **environ = NULL;
int daylight = 0;
long timezone = 0;
char *tzname[2] = {
	"GMT",
	"GMT"
};

/* hidden global variables */

hidden FILE __stdout = {
	.fd = 1
};

hidden FILE __stdin = {
	.fd = 0
};

hidden FILE __stderr = {
	.fd = 2
};

FILE *const stdout = &__stdout;
FILE *const stdin = &__stdin;
FILE *const stderr = &__stderr;

/* external function declarations */

/*
 * a reminder of syscall()
 *
 * %rdi    - number  -> %rax
 * %rsi    - 1st arg -> %rdi
 * %rdx    - 2nd arg -> %rsi
 * %rcx    - 3rd arg -> %rdx
 * %r8     - 4th arg -> %r10 (why?)
 * %r9     - 5th arg -> %r8
 * 8(%rsp) - 6th arg -> %r9
 */

extern int main(int, char *[], char *[]);

/* library declarations */

/* use <system> includes instead */

/* local declarations */

static int vxscanf(const char *restrict src, FILE *restrict stream, const char *restrict format, va_list ap);
static int vxnprintf(char *restrict dst, FILE *restrict stream, size_t size, const char *restrict format, va_list ap);
static struct mem_alloc *alloc_mem(size_t size);
static void free_alloc(struct mem_alloc *buf);
static void check_mem();
static struct __pthread *__pthread_self(void);
static char *fgets_delim(char *s, int size, FILE *stream, int delim);
static int calc_base(const char **ptr);

/* local variables */

static struct mem_alloc *tmp_first = NULL;
static struct mem_alloc *first = NULL;
static struct mem_alloc *last = NULL;
static void *_data_end, *_data_start;
static struct atexit_fun *global_atexit_list;

static void dump_one_mem(const struct mem_alloc *const mem)
{
	printf("mem @ %p [prev=%p,next=%p,free=%d,len=%d]",
			mem,
			mem->prev, mem->next,
			mem->flags & MF_FREE,
			(int)mem->len);
}

__attribute__((unused))
static void dump_mem()
{
	const struct mem_alloc *tmp;
	int i;

	printf("start: ");
	dump_one_mem(first);
	printf("\n");

	printf("last:  ");
	dump_one_mem(last);
	printf("\n");

	for (i = 0, tmp = first; tmp; tmp = tmp->next, i++) {
		printf("[%d] ", i);
		dump_one_mem(tmp);
		printf("\n");
	}
}

#define UTF8_1BYTE 0x00
#define UTF8_2BYTE 0xc0
#define UTF8_3BYTE 0xe0
#define UTF8_4BYTE 0xf0
#define UTF8_NBYTE 0x80

#define UTF8_1BYTE_MASK 0x80
#define UTF8_2BYTE_MASK 0xe0
#define UTF8_3BYTE_MASK 0xf0
#define UTF8_4BYTE_MASK 0xf8
#define UTF8_NBYTE_MASK 0xc0

__attribute__((nonnull))
static int utf32toutf8(const char32_t from, char *to)
{
	if (from < 0x80) {
		to[0] = ( (from >>  0) & ~UTF8_1BYTE_MASK ) | UTF8_1BYTE;
		return 1;

	} else if (from < 0x800) {
		to[0] = ( (from >>  6) & ~UTF8_2BYTE_MASK ) | UTF8_2BYTE;
		to[1] = ( (from >>  0) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
		return 2;

	} else if (from < 0x10000) {
		to[0] = ( (from >> 12) & ~UTF8_3BYTE_MASK ) | UTF8_3BYTE;
		to[1] = ( (from >>  6) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
		to[2] = ( (from >>  0) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
		return 3;

	} else if (from < 0x110000) {
		to[0] = ( (from >> 18) & ~UTF8_4BYTE_MASK ) | UTF8_4BYTE;
		to[1] = ( (from >> 12) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
		to[2] = ( (from >>  6) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
		to[3] = ( (from >>  0) & ~UTF8_NBYTE_MASK ) | UTF8_NBYTE;
		return 4;

	}

	return -1;
}

__attribute__((nonnull))
static int utf8toutf32(const char *orig_from, char32_t *to)
{
	const uint8_t *from = (const uint8_t *)orig_from;

	if (       (from[0] & UTF8_1BYTE_MASK) == UTF8_1BYTE) {
		/* 1 byte */
		*to = 0U;
		*to |= ((from[0] & ~UTF8_1BYTE_MASK) << 0);
		return 1;

	} else if ((from[0] & UTF8_2BYTE_MASK) == UTF8_2BYTE) {
		/* 2 byte */
		*to  = 0U;
		*to |= ((from[0] & ~UTF8_2BYTE_MASK) << 6);
		*to |= ((from[1] & ~UTF8_NBYTE_MASK) << 0);
		return 2;

	} else if ((from[0] & UTF8_3BYTE_MASK) == UTF8_3BYTE) {
		/* 3 byte */
		*to  = 0U;
		*to |= ((from[0] & ~UTF8_3BYTE_MASK) << 12);
		*to |= ((from[1] & ~UTF8_NBYTE_MASK) << 6);
		*to |= ((from[2] & ~UTF8_NBYTE_MASK) << 0);
		return 3;

	} else if ((from[0] & UTF8_4BYTE_MASK) == UTF8_4BYTE) {
		/* 4 byte */
		*to  = 0U;
		*to |= ((from[0] & ~UTF8_4BYTE_MASK) << 18);
		*to |= ((from[1] & ~UTF8_NBYTE_MASK) << 12);
		*to |= ((from[2] & ~UTF8_NBYTE_MASK) << 6);
		*to |= ((from[3] & ~UTF8_NBYTE_MASK) << 0);
		return 4;

	} else {
		errno = EINVAL;
		return -1;
	}
}

static size_t ascii_to_utf32(const char *src, char32_t *dst) { 
	const char *src_ptr = src;
	char32_t   *dst_ptr = dst;

	errno = EINVAL;

	if (*src_ptr <= 0)
		return -1;

	*dst_ptr++ = *src_ptr++;

	errno = 0;
	return 0;
}

static size_t utf32_to_ascii(const char32_t *src, char *dst) { 
	const char32_t *src_ptr = src;
	char           *dst_ptr = dst;

	errno = EINVAL;

	if (*src_ptr == 0)
		return -1;
	else if (*src_ptr > 0x7f)
		*dst_ptr++ = ' ';
	else
		*dst_ptr++ = *src_ptr;

	src_ptr++;

	errno = 0;
	return 0;
}

static size_t utf32_to_utf8(const char32_t *src, char *dst) {
	return utf32toutf8(*src, dst);
}

static size_t utf8_to_utf32(const char *src, char32_t *dst) {
	return utf8toutf32(src, dst);
}

static size_t utf32_in(const char *src, char32_t *dst) {
	*dst = *((char32_t *)src);
	return 4;
}

static size_t utf32_out(const char32_t *src, char *dst) {
	*(char32_t *)dst = *src;
	return 4;
}

//static int utf8_to_utf32(const char *src, char32_t *dst, size_t len) { return -1; }
//static int utf32_to_utf8(const char32_t *src, char *dst, size_t len) { return -1; }
//static int iso88591_to_utf32(const char *src, char32_t *dst, size_t len) { return -1; }
//static int utf32_to_iso88591(const char32_t *src, char *dst, size_t len) { return -1; }

static struct {
	const char *name;
	uint16_t    id;

	size_t   (*to_utf32)(const char     *src, char32_t *dst);
	size_t (*from_utf32)(const char32_t *src, char     *dst);
} iconv_codesets[] = {
	{ "ascii",       367, ascii_to_utf32,     utf32_to_ascii    },
	{ "us-ascii",    367, ascii_to_utf32,     utf32_to_ascii    },
	{ "cp367",       367, ascii_to_utf32,     utf32_to_ascii    },
	{ "iso646-us",   367, ascii_to_utf32,     utf32_to_ascii    },
	{ "ibm367",      367, ascii_to_utf32,     utf32_to_ascii    },
	{ "utf8",       1209, utf8_to_utf32,      utf32_to_utf8     },
	{ "utf-8",      1209, utf8_to_utf32,      utf32_to_utf8     },
	{ "utf32",         0, utf32_in,           utf32_out         },
	{ "utf-32",        0, utf32_in,           utf32_out         },
//	{ "iso-8859-1",  819, iso88591_to_utf32,  utf32_to_iso88591 },

	{NULL, 0, NULL, NULL}
};

static int find_iconv_codeset(const char *name)
{
	for (int i = 0; iconv_codesets[i].name; i++) {
		if (!(strcasecmp(name, iconv_codesets[i].name)))
			return i;
	}

	return -1;
}

struct iconv_private {
	int from;
	int to;

	char32_t buf[32];

	/* add additional state information here */
};

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
	struct iconv_private *state = cd;

	if (state == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* may need to update the iconv_codeset entries to include functions for 
	 * state reset & shift sequence */
	if ( inbuf == NULL || *inbuf == NULL ) {
		/* set init state ... */
		memset(state->buf, 0, sizeof(state->buf));
		printf("init ");

		if ( outbuf != NULL || (outbuf && *outbuf != NULL )) {
			printf("shift");
			/* ... and store shift sequence */
		}
		printf("\n");
		return 0;
	}

	/* normal conversion */

	char *src_ptr = *inbuf;
	char *dst_ptr = *outbuf;
	size_t done = 0;

	printf("loop start: %p[%x] -> %p[%x]\n", src_ptr, *inbytesleft, dst_ptr, *outbytesleft);
	while (*src_ptr && *inbytesleft && *outbytesleft) {
		printf("loop 0\n");

		done = iconv_codesets[state->from].to_utf32(src_ptr, state->buf);
		printf("loop 0 = %x\n", done);
		if (done == (size_t)-1)
			return -1;
		if (done == 0)
			break;

		src_ptr      += done;
		*inbytesleft -= done;

		printf("loop 1\n");

		done = iconv_codesets[state->to].from_utf32(state->buf, dst_ptr);
		if (done == (size_t)-1)
			return -1;
		if (done == 0)
			break;

		/* TODO undo src_ptr / inbytesleft if done == 0 ? */

		dst_ptr       += done;
		*outbytesleft -= done;
	}

	return 0;
}

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
	int to   = find_iconv_codeset(tocode);
	int from = find_iconv_codeset(fromcode);

	if ( to == -1 || from == -1 ) {
		errno = EINVAL;
		return (iconv_t)-1;
	}

	struct iconv_private *ret;

	if ((ret = calloc(1, sizeof(struct iconv_private))) == NULL)
		return (iconv_t)-1;

	ret->from = from;
	ret->to   = to;

	return ret;
}

int iconv_close(iconv_t cd)
{
	if (cd != NULL)
		free(cd);

	return 0;
}


int system(const char *command)
{
	pid_t child;
	int wstatus = 0;

	if (command == NULL) {
		if (access("/bin/sh", R_OK|X_OK) == 0)
			return 1;
		return 0;
	}

	struct sigaction *oldint = NULL, *oldquit = NULL;
	sigset_t oset, set;
	bool restore_chld = true;

	if ((sigprocmask(0, NULL, &oset)) == -1)
		return -1;
	
	if (sigismember(&oset, SIGCHLD))
		restore_chld = false;

	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, NULL);

	sigaction(SIGINT, &(struct sigaction) {.sa_handler = SIG_IGN}, oldint);
	sigaction(SIGQUIT, &(struct sigaction) {.sa_handler = SIG_IGN}, oldquit);

	if ((child = fork()) == 0) {
		if (execl("/bin/sh", "sh", "-c", command, NULL) == -1)
			_exit(127);
	} else if (child == -1) {
		return wstatus = -1;;
	} 

	if (wstatus == 0)
		waitpid(child, &wstatus, 0);

	if (restore_chld) {
		sigemptyset(&set);
		sigaddset(&set, SIGCHLD);
		sigprocmask(SIG_UNBLOCK, &set, NULL);
	}

	sigaction(SIGINT, oldint, NULL);
	sigaction(SIGQUIT, oldquit, NULL);

	return wstatus;
}

pid_t waitpid(pid_t pid, int *wstatus, int options)
{
	return syscall(__NR_wait4, pid, wstatus, options, NULL);
}

int execve(const char *path, char *const argv[], char *const envp[])
{
	int rc = 0;
	//printf("execve: path=%s, argv=%p, envp=%p, envp[0]=%p, envp[0]=%s\n", path, argv, envp, envp[0], envp[0]);
	rc = syscall(__NR_execve, (uint64_t)path, (uint64_t)argv, (uint64_t)envp);
	//printf("execve: returned %u\n", rc);
	return rc;
}

int execl(const char *path, const char *arg0, ...)
{
	int argc = 1;
	va_list ap;
	char *tmp, **argv;

	va_start(ap, arg0);
	while ((tmp = va_arg(ap, char *)) != NULL)
		argc++;
	va_end(ap);

	if ((argv = calloc(argc + 1, sizeof(char *))) == NULL)
		return -1;

	argv[0] = strdup(arg0);

	va_start(ap, arg0);
	for (int i = 1; i < argc; i++)
	{
		argv[i] = va_arg(ap, char *);
	}
	va_end(ap);

	return execve(path, argv, environ);
}

int getpriority(int which, id_t who)
{
	return syscall(__NR_getpriority, which, who);
}

__attribute__((nonnull))
static size_t _qsort_partition(void *base, size_t width, int (*comp)(const void *, const void *), 
        ssize_t begin, ssize_t end)
{
    void *pivot = base + (end * width);
    ssize_t i = (begin - 1);
    void *swap_temp = malloc(width);

    if (swap_temp == NULL)
        return begin - 1;

    for (ssize_t j = begin; j < end; j++)
    {
        if (comp(base + (j * width), pivot) <= 0) {
            i++;

            memcpy(swap_temp, base + (i * width), width);
            memcpy(base + (i * width), base + (j * width), width);
            memcpy(base + (j * width), swap_temp, width);
        }
    }

    i++;

    memcpy(swap_temp, base + (i * width), width);
    memcpy(base + (i * width), base + (end * width), width);
    memcpy(base + (end * width), swap_temp, width);

    return i;
}

__attribute__((nonnull))
static void _qsort(void *base, size_t width, int (*comp)(const void *, const void *),
        ssize_t begin, ssize_t end)
{
    if (begin < end) {
        ssize_t idx = _qsort_partition(base, width, comp, begin, end);
        if (idx < begin)
            return;

        _qsort(base, width, comp, begin, idx - 1);
        _qsort(base, width, comp, idx + 1, end);
    }
}

void qsort(void *base, size_t nel, size_t width, int (*comp)(const void *, const void *))
{
    if (base == NULL || comp == NULL || width == 0) {
        errno = EINVAL;
        return;
    }

    if (nel < 2)
        return;

    _qsort(base, width, comp, 0, nel - 1);
}

int setpriority(int which, id_t who, int pri)
{
	return syscall(__NR_setpriority, which, who, pri);
}

int nice(int inc)
{
	return setpriority(PRIO_PROCESS, 0, getpriority(PRIO_PROCESS, 0) + inc);
}

int execvp(const char *file, char *const argv[])
{
	//printf("execvp(%s, argv=%p)\n", file, argv);
	return execve(file, argv, environ);
}

__attribute__((noreturn))
void exit_group(int status) 
{
	syscall(__NR_exit_group, status);
	for (;;) __asm__ volatile("pause");
}

char *stpcpy(char *dest, const char *src)
{
    if (dest == NULL || src == NULL)
        goto fail;

    size_t i;
    for (i = 0; src[i]; i++)
        dest[i] = src[i];
    dest[i] = '\0';

    return &dest[i];

fail:
    return NULL;
}

char *strcpy(char *dest, const char *src)
{
	if (dest == NULL || src == NULL)
		goto fail;

	size_t i;
	for (i = 0; src[i]; i++)
		dest[i] = src[i];
	dest[i] = '\0';

fail:
	return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	if (dest == NULL || src == NULL)
		goto fail;

	size_t i;
	for (i = 0; src[i] && i < n; i++)
		dest[i] = src[i];
	dest[i] = '\0';

fail:
	return dest;
}

__attribute__((noreturn))
void _exit(int status)
{
	syscall(__NR_exit, status);
	for (;;) __asm__ volatile("pause");
}

__attribute__((noreturn))
void _Exit(int status)
{
	exit_group(status);
}

__attribute__((noreturn))
void exit(int status)
{
	check_mem();
	//dump_mem();
	_exit(status);
}

__attribute__((pure))
char *strchr(const char *const s, const int c)
{
	if (s == NULL) return NULL;

	const char *tmp;

	for (tmp = s; *tmp && *tmp != c; tmp++) ;
	if (!*tmp) return NULL;
	return (char *)tmp;
}

__attribute__((pure))
char *strrchr(const char *const s, const int c)
{
	if (s == NULL) return NULL;

	const char *tmp;

	tmp = (s + (strlen(s) - 1));

	while (tmp >= s && *tmp != c) tmp--;
	if (tmp < s) return NULL;
	return (char *)tmp;
}

static char *strtok_state;

char *strtok(char *restrict s, const char *restrict sep)
{
	return strtok_r(s, sep, &strtok_state);
}

char *strtok_r(char *restrict str, const char *restrict delim, char **restrict saveptr)
{
	char *tmp, *ret;

	if (saveptr == NULL || delim == NULL)
		return NULL;

	if (str)
		*saveptr = str;

	if (!*saveptr)
		return NULL;

	while (**saveptr && *(*saveptr+1) && strchr(delim, **saveptr))
		*(*saveptr)++ = '\0';

	tmp = *saveptr;

	while (*tmp && !strchr(delim, *tmp))
		tmp++;

	while (*tmp && *(tmp+1) && strchr(delim, *(tmp+1)))
		*tmp++ = '\0';

	if (tmp == *saveptr)
		return (*saveptr = NULL);

	if (!*tmp) {
		ret = *saveptr;
		*saveptr = NULL;
		return ret;
	}

	*tmp = '\0';
	ret = *saveptr;

	*saveptr = ++tmp;
	return ret;
}

ssize_t write(int fd, const void *buf, size_t count)
{
	return syscall(__NR_write, fd, buf, count);
}

ssize_t read(int fd, void *buf, size_t count)
{
	return syscall(__NR_read, fd, buf, count);
}

int symlink(const char *path1, const char *path2)
{
	return syscall(__NR_symlink, path1, path2);
}

int link(const char *path1, const char *path2)
{
	return syscall(__NR_link, path1, path2);
}

int open(const char *pathname, int flags, ...)
{
	mode_t mode = 0;

	if (flags & O_CREAT) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	return syscall(__NR_open, pathname, flags, mode);
}

int access(const char *pathname, int mode)
{
	return syscall(__NR_access, pathname, mode);
}

int close(int fd)
{
	return syscall(__NR_close, fd, 0 ,0 ,0 ,0 ,0, 0);
}

int chmod(const char *path, mode_t mode)
{
	return syscall(__NR_chmod, path, mode);
}

int lchown(const char *pathname, uid_t owner, gid_t group)
{
	return syscall(__NR_lchown, pathname, owner, group);
}

int chown(const char *pathname, uid_t owner, gid_t group)
{
	return syscall(__NR_chown, pathname, owner, group);
}

int lstat(const char *pathname, struct stat *statbuf)
{
	int fd, rc;

	if ((fd = open(pathname, O_RDONLY|O_NOFOLLOW)) == -1)
		return -1;

	rc = fstat(fd, statbuf);

	close(fd);

	return rc;
}

int fstat(int fd, struct stat *buf)
{
	return syscall(__NR_fstat, fd, buf);
}

int utime(const char *path, const struct utimbuf *times)
{
	return syscall(__NR_utime, path, times);
}

int utimes(const char *path, const struct timeval times[2])
{
	return syscall(__NR_utimes, path, times);
}

int unlink(const char *path)
{
	return syscall(__NR_unlink, path);
}

int stat(const char *restrict pathname, struct stat *restrict statbuf)
{
	return syscall(__NR_stat, pathname, statbuf);
}

off_t lseek(int fd, off_t offset, int whence)
{
	return syscall(__NR_lseek, fd, offset, whence);
}

int fileno(FILE *stream)
{
	if (stream == NULL || stream->mem) {
		errno = EBADF;
		return -1;
	}

	return stream->fd;
}

int fclose(FILE *stream)
{
    int ret = 0;

	if (!stream) {
		return 0;
    }

    if (!stream->mem && stream->fd != -1) {
        close(stream->fd);
    }

	if (stream->buf) {
		free(stream->buf);
    }
	free(stream);

	return ret;
}

__attribute__((nonnull))
static void itoa(char *buf, int base, unsigned long d, __attribute__((unused)) bool pad, __attribute__((unused)) int size)
{
	char *p = buf, *p1, *p2;
	unsigned long ud = d;
	unsigned long divisor = 10;
	unsigned long remainder;

	if (base=='d' && (long)d < 0)
	{
		*p++ = '-';
		buf++;
		ud = -d;
	} else if (base=='x') {
		divisor = 16;
	}

	do {
		remainder = ud % divisor;
		*p++ = (char)((remainder < 10) ? remainder + '0' : remainder + 'a' - 10);
	} while (ud /= divisor);

	*p = 0;

	p1 = buf;
	p2 = p - 1;

	while (p1<p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

int snprintf(char *restrict str, size_t size, const char *restrict format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsnprintf(str, size, format, ap);
	va_end(ap);
	return ret;
}

int sprintf(char *restrict s, const char *restrict format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsprintf(s, format, ap);
	va_end(ap);
	return ret;
}

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vfprintf(stream, format, ap);
	va_end(ap);
	return ret;
}

int printf(const char *restrict format, ...)
{
	if (stdout == NULL)
		return 0;

	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vfprintf(stdout, format, ap);
	va_end(ap);
	return ret;
}

int vfscanf(FILE *restrict stream, const char *restrict format, va_list arg)
{
	return vxscanf(NULL, stream, format, arg);
}

int vscanf(const char *restrict format, va_list arg)
{
	if (stdin == NULL)
		return 0;

	return vfscanf(stdin, format, arg);
}

int vsscanf(const char *restrict s, const char *restrict format, va_list arg)
{
	return vxscanf(s, NULL, format, arg);
}

int fscanf(FILE *restrict stream, const char *restrict format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vfscanf(stream, format, ap);
	va_end(ap);
	return ret;
}

int sscanf(const char *restrict s, const char *restrict format, ...)
{
	int ret;

	va_list ap;
	va_start(ap, format);
	ret = vsscanf(s, format, ap);
	va_end(ap);
	return ret;
}

int scanf(const char *restrict format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vscanf(format, ap);
	va_end(ap);
	return ret;
}

int vsprintf(char *restrict dst, const char *restrict format, va_list ap)
{
	return vxnprintf(dst, NULL, 0, format, ap);
}

int vsnprintf(char *restrict dst, size_t size, const char *restrict format, va_list ap)
{
	return vxnprintf(dst, NULL, size, format, ap);
}

int vfprintf(FILE *restrict stream, const char *format, va_list ap)
{
	return vxnprintf(NULL, stream, 0, format, ap);
}

int vprintf(const char *restrict format, va_list ap)
{
	return vfprintf(stdout, format, ap);
}

#define _CHAR   1
#define _SHORT	2
#define _INT	4
#define _LONG	8
#define _LLONG	16

static const char *restrict lastss = NULL;
static const char *restrict ss = NULL;
static bool ss_invert = false;

inline static bool is_valid_scanset(const char *restrict scanset, char c)
{
	if (scanset != lastss) {
		ss = lastss = scanset;
		/* do some parsing of ss here for speed */
		if (*lastss == '^') {
			ss++;
			ss_invert = true;
			//printf("is_valid_scanset: invert\n");
		}
	}

	//printf("is_valid_scanset(<%s>, <%c>)\n", scanset, c);

	return strchr(ss, c) ? !ss_invert : ss_invert;
}

static char *expand_scanset(char *orig)
{
	char *ret = NULL;
	size_t len;
	size_t off = 0;
	const char *sptr;
	char from, to;

	//printf("expand_scanset: orig=<%s>\n", orig);
		
	len = strlen(orig);
	if ((ret = malloc(len + 1)) == NULL)
		goto done;

	sptr = orig;
	if (*sptr && *sptr == '^')
		ret[off++] = *sptr++;

	while (*sptr)
	{
		if (*(sptr + 1) == '-' && *(sptr + 2)) {
			from = *sptr;
			to = *(sptr + 2);
			if ((ret = realloc(ret, len + (to - from))) == NULL)
				goto done;
			for (char t = from; t <= to; t++)
				ret[off++] = t;
			sptr += 3;
		} else
			ret[off++] = *sptr++;
	}

done:
	free(orig);
	return ret;
}

static int vxscanf(const char *restrict src, FILE *restrict stream, const char *restrict format, va_list ap)
{
	char c=0, chr_in=0;
	const char *restrict save;
	//const char *restrict p;
	char *scanset = NULL;
	char buf[64] = {0};
	bool is_file = stream ? true : false;
	int bytes_scanned = 0, rc = -1;
	unsigned buf_idx;

	//memset(buf, '0', sizeof(buf));

	// p = src;

	if (format == NULL || (src == NULL && stream == NULL))
		return -1;

	while ((c = *format++) != 0)
	{
		//printf("got: %c\n", c);

		if (stream && (feof(stream) || ferror(stream))) {
			goto fail;
		} else if (isspace(c)) {
			while (isspace(*format++)) ;
			format--;

			do {
				int tmp;
				if (is_file) {
					if ((tmp = fgetc(stream)) == EOF)
						break;
					chr_in = (char)tmp;
				} else {
					chr_in = *src++;
				}

				//printf(".ws?: %c\n", chr_in);

				if (chr_in == '\0')
					break;

				if (isspace(chr_in))
					continue;

				if (is_file)
					ungetc(chr_in, stream);
				else
					src--;

				break;
			} while(1);
		} else if (c != '%') {
			int tmp;

			if (is_file) {
				if ((tmp = fgetc(stream)) == EOF)
					break;
				chr_in = (char)tmp;
			} else {
				chr_in = *src++;
			}

			if (chr_in == '\0') {
				break;
			}
			if (c != chr_in)
				break;
		} else {
			int len = _INT, str_limit = 0, sub_read = 0;
			bool do_malloc = false;
			char *dst = NULL;
			int base = 10;
next:
			c = *format++;

			//printf(".got: %c\n", c);

			// p = buf;

			if (isdigit((unsigned char)c)) {
				str_limit *= 10;
				str_limit += c - '0';
				goto next;
			}

			switch(c)
			{
				case 'h':
					len = (len == _SHORT ? _CHAR : _SHORT);
					goto next;

				case 'l':
					len = (len == _LONG ? _LLONG : _LONG);
					goto next;

				case 'j':
					len = _INT;
					goto next;

				case 'z':
					len = _LONG;
					goto next;

				case 't':
					len = _LONG;
					goto next;

				case 'm':
					do_malloc = true;
					goto next;

				case 'd':
				case 'u':
				case 'x':
				case 'i':
					buf_idx = 0;
					do {
						/* read all the digits into buf & set last to \0 */
						int tmp;

						if (is_file) {
							if ((tmp = fgetc(stream)) == EOF)
								break;
							chr_in = (char)tmp;
						} else {
							chr_in = *src++;
						}

						if (chr_in == '\0')
							break;

						//printf(".d.got: %c\n", chr_in);

						buf[buf_idx++] = chr_in;
					} while(buf_idx < sizeof(buf)-1);
					/* TODO size modifiers {hh,h,l,ll,j,z,t} */

					base = 10;

					switch(c) {
						case 'x':
							base = 16;
							/* fall through */
						case 'u':
							switch (len) {
								case _CHAR:
									*(unsigned char*)(va_arg(ap, unsigned char *)) = strtoul(buf, NULL, base);
									bytes_scanned++;
									break;
								case _SHORT:
									*(unsigned short *)(va_arg(ap, unsigned short *)) = strtoul(buf, NULL, base);
									bytes_scanned++;
									break;
								case _INT:
									*(unsigned *)(va_arg(ap, unsigned *)) = strtoul(buf, NULL, base);
									bytes_scanned++;
									break;
								case _LONG:
									*(unsigned long *)(va_arg(ap, unsigned long*)) = strtoul(buf, NULL, base);
									bytes_scanned++;
									break;
								case _LLONG:
									*(unsigned long long*)(va_arg(ap, unsigned long long*)) = strtoull(buf, NULL, base);
									bytes_scanned++;
									break;
							}
							break;

						case 'i':
							base = 0;
							/* fall through */
						case 'd':
							switch (len) {
								case _CHAR:
									*(char *)(va_arg(ap, char *)) = strtol(buf, NULL, base);
									bytes_scanned++;
									break;
								case _SHORT:
									*(short *)(va_arg(ap, short *)) = strtol(buf, NULL, base);
									bytes_scanned++;
									break;
								case _INT:
									*(int *)(va_arg(ap, int *)) = strtol(buf, NULL, base);
									bytes_scanned++;
									break;
								case _LONG:
									*(long *)(va_arg(ap, long *)) = strtol(buf, NULL, base);
									bytes_scanned++;
									break;
								case _LLONG:
									*(long long *)(va_arg(ap, long long *)) = strtoll(buf, NULL, base);
									bytes_scanned++;
									break;
							}
							break;
					}
					break; /* case d: case u: */

				case '[':
					{
						if (scanset)
							free(scanset);

						/* read the scan set up to ] */
						save = format;

						if (*format == '^') format++;
						if (*format == ']') format++;

						while((c = *format++) != '\0')
							if (c == ']') 
								break;

						if (c == '\0')
							goto fail;

						if ((scanset = malloc(format - save + 1)) == NULL)
							goto fail;

						strncpy(scanset, save, format - save - 1);
						scanset = expand_scanset(scanset);

					}
					/* fall through */
				case 's':
					if (do_malloc) {
						dst = malloc(1024);
						*(char **)(va_arg(ap, char **)) = dst;
					} else {
						dst = (char *)(va_arg(ap, char *));
					}

					if (dst == NULL) {
						//warnx("dst NULL");
						goto fail;
					}

					/* this bit should apply to all 'read me some stuff' ? */
					sub_read = 0;
					do {
						int tmp;

						if (is_file) {
							if ((tmp = fgetc(stream)) == EOF)
								break;
							chr_in = (char)tmp;
						} else {
							chr_in = *src++;
						}

						if (chr_in == '\0') {
							//printf(".s.got: null\n");
							break;
						}

						if ( ( scanset && !is_valid_scanset(scanset, chr_in)) ||
							 (!scanset && isspace(chr_in))
						   ) {
							if (is_file)
								ungetc(chr_in, stream);
							else
								src--;

							//printf(".s.got: invalid '%c'\n", chr_in);

							break;
						} 
						//printf(".s.got: %c\n", chr_in);
						*dst++ = chr_in;
						sub_read++;

						if ((str_limit && sub_read >= str_limit) || sub_read > 1000)
							break;

					} while(1);
					*dst = '\0';

					if (scanset) {
						free(scanset);
						scanset = NULL;
					}

					bytes_scanned++;

					break; /* case 's' */
			} /* switch(c) */
		} /* } else { */
	} /* while ((c = *format++) != 0) */

	rc = bytes_scanned;

fail:
	if (scanset) {
		free(scanset);
		scanset = NULL;
	}

	return rc;
}

inline static long max(long a, long b)
{
	return a > b ? a : b;
}

inline static long min(long a, long b)
{
	return a < b ? a : b;
}

typedef enum { JUSTIFY_NONE = 0, JUSTIFY_LEFT = 1, JUSTIFY_RIGHT = 2 } justify_t;

__attribute__ ((access (write_only, 1), access (read_only, 4)))
static int vxnprintf(char *restrict dst, FILE *restrict stream, size_t size, const char *restrict format, va_list ap)
{
	char c;
	const char *p;
	char buf[64] = {0};
	ssize_t i, l;
	size_t off = 0, wrote = 0, remainder = 0;
	const bool is_file   = stream ? true : false;
	const bool is_string = dst    ? true : false;

	if (format == NULL)// || (dst == NULL && stream == NULL))
		return -1;

	//memset(buf2, '0', 63);
	//memset(buf,  '0', 63);

	while ((c = *format++) != 0 && (size == 0 || off < size))
	{
		if (is_string)
			dst[off] = '\0';

		if (is_file && (feof(stream) || ferror(stream)))
			return -1;

		if ( c!= '%' ) {
			const char *tformat = format - 1;
			while (*tformat && *tformat != '%') tformat++;
			ssize_t tlen = tformat - (format - 1);
			tlen = size ? min(size-off, tlen) : tlen;

			if (is_file)
				fwrite(format-1, tlen, 1, stream);
			else if (is_string)
				memcpy(dst + off, format-1, tlen);

			off   += tlen;
			wrote += tlen;
			format = tformat;
		} else {
			int     lenmod_size = _INT;
			ssize_t field_width = 0;
			ssize_t buflen = 0;
			ssize_t precision = 0;

			bool zero_pad  = false;
			bool done_flag = false;
			bool done_fwid = false;
			bool done_prec = false;
			bool has_prec  = false;

			bool printed = false;

			justify_t justify = JUSTIFY_RIGHT;

next:
			c = *format++;
			p = buf;

			if (done_prec)
				goto skip_prec;

			if (done_fwid)
				goto skip_fwid;

			if (done_flag)
				goto skip_flag;

			switch (c)
			{
				case '-':
					justify = JUSTIFY_LEFT;
					zero_pad = false;
					goto next;
				case '0':
					if (justify != JUSTIFY_LEFT) {
						zero_pad = true;
						justify = JUSTIFY_RIGHT;
					}
					goto next;
			}

			done_flag = true;
skip_flag:

			if (isdigit((unsigned char)c)) {
				field_width *= 10;
				field_width += c - '0';
				goto next;
			}

			done_fwid = true;
skip_fwid:

			if (c == '.') {
				has_prec  = true;
				goto next;
			} else if (!has_prec) {
				done_prec = true;
				goto skip_prec;
			} else if (isdigit(c)) {
				precision *= 10;
				precision += c - '0';
				goto next;
			}

			done_prec = true;

skip_prec:
			switch (c)
			{
				case '%':
					goto chr;

				case 'p':
					lenmod_size = _LONG;
					c = 'x';
					goto forcex;

				case 'h':
					lenmod_size = (lenmod_size == _SHORT ? _CHAR : _SHORT);
					goto next;

				case 'l':
					lenmod_size = (lenmod_size == _LONG ? _LLONG : _LONG);
					goto next;

				case 'j':
					lenmod_size = _INT;
					goto next;

				case 'z':
					lenmod_size = _LONG;
					goto next;

				case 't':
					lenmod_size = _LONG;
					goto next;

				case 'u':
				case 'x':
				case 'X': /* TODO upper case [A-F] */
forcex:
					switch(lenmod_size) {
						case _CHAR:
							itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), zero_pad, lenmod_size);
							break;
						case _SHORT:
							itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), zero_pad, lenmod_size);
							break;
						case _INT:
							itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), zero_pad, lenmod_size);
							break;
						case _LONG:
							itoa(buf,c,(unsigned long)va_arg(ap, unsigned long), zero_pad, lenmod_size);
							break;
						case _LLONG:
							errno = ENOSYS;
							return -1;
					}
					precision = 0;

					goto padcheck;

				case 'i':
				case 'd':
					switch(lenmod_size) {
						case _CHAR:
							itoa(buf,c,(unsigned long)va_arg(ap, int), zero_pad, lenmod_size);
							break;
						case _SHORT:
							itoa(buf,c,(unsigned long)va_arg(ap, int), zero_pad, lenmod_size);
							break;
						case _INT:
							itoa(buf,c,(unsigned long)va_arg(ap, int), zero_pad, lenmod_size);
							break;
						case _LONG:
							itoa(buf,c,(unsigned long)va_arg(ap, long), zero_pad, lenmod_size);
							break;
						case _LLONG:
							errno = ENOSYS;
							return -1;
							break;
					}

					precision = 0;
padcheck:
					if (justify == JUSTIFY_RIGHT) {
leftpadcheck:						
						buflen = strlen(p);
						char pad_chr = zero_pad ? '0' : ' ';
						/*
						   char tmp[64];
						   putchar('"'); puts(p); putchar('"');
						   puts(" field_width="); itoa(tmp,'d',field_width,false,8); puts(tmp);
						   puts(" tmplen="); itoa(tmp,'d',buflen,false,8); puts(tmp);
						   putchar('\n');
						   */

						/* TODO handle precision/field_width == 0 but zero_pad, so
						 * need to use lenmod_size is the default */

						for (i = 0, l = field_width - buflen; l && (i < l) && (!size || off < size); i++, off++, wrote++) {
							if (is_file)
								putc(pad_chr, stream);
							else if (is_string)
								dst[off] = pad_chr;
						}
						zero_pad = false;
						justify = JUSTIFY_RIGHT;
					}

					lenmod_size = _INT;
					if (!printed)
						goto string;
					break;

				case 's':
					p = va_arg(ap, const char *);
					goto padcheck;
string:
					if (printed)
						break;

					printed = true;

					int plen = p ? strlen(p) : 6;

					if (size) {
						remainder = size - off;
						remainder = (precision>0) ? (size_t)min((size_t)precision, remainder) : remainder;
						remainder = min(plen, remainder);
					} else
						remainder = (precision>0) ? min(precision, plen) : plen;

					if (p == NULL) { 
						/* handle the case our string is a NULL pointer */
						if (is_file) {
							fwrite("(null)", remainder, 1, stream);
						} else if (is_string) {
							strncpy(dst + off, "(null)", remainder); 
						}
						off   += remainder;
						wrote += remainder;
						if (justify == JUSTIFY_LEFT)
							goto leftpadcheck;
					} else {
						/*
						   char tmpb[64];
						   puts("strlen(p)="); itoa(tmpb,'d',strlen(p),false,8); puts(tmpb);
						   puts(" size="); itoa(tmpb,'d',size,false,8); puts(tmpb);
						   puts(" off="); itoa(tmpb,'d',off,false,8); puts(tmpb);
						   puts(" precision="); itoa(tmpb,'d',precision,false,8); puts(tmpb);
						   puts(" remainder="); itoa(tmpb,'d',remainder,false,8); puts(tmpb);
						   putchar('\n');
						   */
						if (is_file) {
							fwrite(p, remainder, 1, stream);
						} else if (is_string) {
							strncpy(dst + off, p, remainder); 
						}
						off   += remainder; /* TODO does this put off > size ? */
						wrote += remainder;
						if (justify == JUSTIFY_LEFT)
							goto leftpadcheck;
					}
					break;

				case 'c':
					c = va_arg(ap, int);
chr:
					if (is_file) {
						fputc(c, stream);//isprint(c) ? c : ' ', stream);
					} else if (is_string) {
						dst[off] = c;//isprint(c) ? c : ' ';
					}

					off++;
					wrote++;
					break;

				case 'n':
					*(va_arg(ap, int *)) = wrote;
					break;

				default:
					errno = ENOSYS;
					return -1;
			}
		}
	}

	if (is_string) {
		if (off == size)
			dst[off] = '\0';
		else {
			dst[off++] = '\0';
		}

		/* this looks like it might be off-by-1 in the case above ? FIXME */

		return off;
	} 

	return wrote;
}

#undef _LONG
#undef _SHORT
#undef _INT
#undef _LLONG
#undef _CHAR

int fcntl(int fd, int cmd, ...)
{
	unsigned long arg;
	va_list ap;

	va_start(ap, cmd);
	arg = va_arg(ap, unsigned long);
	va_end(ap);

	return syscall(__NR_fcntl, fd, cmd, arg);
}

__attribute__((nonnull(1), access(read_only, 1), access(write_only, 2)))
static int parse_fopen_flags(const char *mode, bool *seek_end)
{
	int want_flags = 0;
	if (seek_end)
		*seek_end  = false;
	
	if      (!strncmp(mode, "r+", 2)) want_flags = O_RDWR;
	else if (!strncmp(mode, "w+", 2)) want_flags = O_RDWR|O_TRUNC|O_CREAT;
	else if (!strncmp(mode, "a+", 2)) want_flags = O_RDWR|O_CREAT;
	else if (*mode == 'r')            want_flags = O_RDONLY;
	else if (*mode == 'w')            want_flags = O_WRONLY|O_TRUNC|O_CREAT;
	else if (*mode == 'a')           {want_flags = O_WRONLY|O_CREAT; if (seek_end) *seek_end = true;}

	return want_flags;
}

FILE *fdopen(int fd, const char *mode)
{
	if (!mode || fd < 0) {
		return NULL;
	}

	FILE *ret = calloc(1, sizeof(FILE));
	if (ret == NULL) {
		return NULL;
	}
	ret->fd = fd;

	int flags;

	if ((flags = fcntl(fd, F_GETFL)) == -1)
		goto fail;

	bool seek_end  = false;
	const int want_flags = parse_fopen_flags(mode, &seek_end);

	if (fcntl(fd, F_SETFL, want_flags) == -1)
		goto fail;

	if (seek_end)
		fseek(ret, 0, SEEK_END);

	return ret;
fail:
	if (ret)
		free(ret);

	return NULL;
}

int isdigit(int c)
{
	unsigned char ch = (unsigned char)c;

	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

int isxdigit(int c)
{
	//unsigned char ch = (unsigned char)c;

	if (c >= '0' && c <= '9')
		return true;
	if (c >= 'a' && c <= 'f')
		return true;
	if (c >= 'A' && c <= 'F')
		return true;

	return false;
}

int ispunct(int c)
{
	if (isalnum(c)) return false;
	if (iscntrl(c)) return false;
	if ((unsigned char)c == ' ') return false;

	return true;
}

int isalnum(int c)
{
	if (isalpha(c)) return true;
	if (isdigit(c)) return true;

	return false;
}

int isblank(int c)
{
	//unsigned char ch = (unsigned char)c;

	switch(c)
	{
		case ' ':
		case '\t':
			return true;
	}

	return false;
}

int iscntrl(int c)
{
	if (c < 0x20 || c == 0x7f) return true;

	return false;
}

int isprint(int c)
{
	return !iscntrl(c);
}

int isgraph(int c)
{
	if (isalnum(c)) return true;
	if (ispunct(c)) return true;

	return false;
}

int isalpha(int c)
{
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;

	return false;
}

int isupper(int c)
{
	if (c >= 'A' && c <= 'Z') return true;

	return false;
}

int islower(int c)
{
	if (c >= 'a' && c <= 'z') return true;

	return false;
}

int isspace(int c)
{
	//register unsigned char ch = (unsigned char)c;

	switch(c)
	{
		case ' ':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			return true;
		default:
			return false;
	}
}

int tolower(int c)
{
	if (!isupper(c)) return c;

	return(c - ('a' - 'A'));
}

int toupper(int c)
{
	if (!islower(c)) return c;

	return(c + ('a' - 'A'));
}

int ferror(FILE *stream)
{
	return(stream->error != 0);
}

int feof(FILE *stream)
{
	return(stream->eof);
}

FILE *fopen(const char *pathname, const char *modestr)
{
	int mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
	bool seek_end;

	if (!pathname || !modestr)
		return NULL;

	const int flags = parse_fopen_flags(modestr, &seek_end);
	int fd = open(pathname, flags, mode);
	if (fd < 0)
		goto fail;

	FILE *ret;
	if ((ret = calloc(1, sizeof(FILE))) == NULL)
		goto fail_close;

	if ((ret->buf = calloc(1, BUFSIZ)) == NULL)
		goto fail_free;

	if (isatty(fd))
		ret->buf_mode = _IOLBF;
	else
		ret->buf_mode = _IOFBF;

	ret->bpos = 0;
	ret->bhas = 0;
	ret->blen = BUFSIZ;
	ret->fd = fd;
    ret->flags = flags;
	return ret;

fail_free:
	free(ret);
fail_close:
	close(fd);
fail:
	return NULL;
}

FILE *fmemopen(void *buf, size_t size, const char *mode)
{
	errno = ENOMEM;

	if (buf == NULL)
		errno = EINVAL;

    FILE *ret;
    bool seek_end;

    if ((ret = calloc(1, sizeof(FILE))) == NULL)
        return NULL;

    if ((ret->buf = calloc(1, BUFSIZ)) == NULL)
        goto fail_free;

    ret->flags = parse_fopen_flags(mode, &seek_end);
    ret->buf_mode = _IOFBF;
    ret->bpos = 0;
    ret->bhas = 0;
    ret->mem = buf;
    ret->mem_size = size;
    ret->offset = 0;
    ret->fd = -1;

	return ret;

fail_free:
    free(ret);
    return NULL;
}

DIR *opendir(const char *dirname)
{
	DIR *ret;

	if (dirname == NULL)
		return NULL;

	int fd = open(dirname, O_RDONLY|O_DIRECTORY, 0);
	if (fd < 0)
		return NULL;

	if ((ret = calloc(1, sizeof(DIR))) == NULL) {
		close(fd);
		return NULL;
	}

	ret->fd = fd;
	ret->idx = NULL;
	ret->end = (struct dirent *)(ret->buf + sizeof(ret->buf));

	return ret;
}

ssize_t getdents64(int fd, void *dirp, size_t count)
{
	return (ssize_t)syscall(__NR_getdents64, fd, dirp, count);
}

struct dirent *readdir(DIR *dp)
{
	struct dirent *ret;

	if (dp == NULL) {
		errno = EBADF;
		return NULL;
	}

	errno = 0;
	ssize_t rc;

	if (dp->idx == NULL)
		goto get;

	if (dp->idx < dp->end && dp->idx->d_reclen) {
ok:
		ret = dp->idx;
		//printf("readdir: d_ino: %d d_off: %d d_reclen: %d d_type: %d: d_name: \"%s\"\n",
		//		ret->d_ino,
		//		ret->d_off,
		//		ret->d_reclen,
		//		ret->d_type,
		//		ret->d_name);
		dp->idx = (struct dirent *)(((char *)ret) + ret->d_reclen);
		return ret;
	}

get:
	//printf("readdir: getdents64()\n");
	rc = getdents64(dp->fd, dp->buf, sizeof(dp->buf));
	//printf("readdir: getdents64() returned %ld\n", rc);

	if (rc == -1) {
		//printf("readdir: rc<0\n");
		return NULL;
	} else if (rc == 0) {
		//printf("readdir: rc==0\n");
		return NULL;
	} else {
		dp->idx = (struct dirent *)dp->buf;
		//printf("readdir: rc=%d d_reclen:%d\n",
		//		rc,
		//		dp->idx->d_reclen);
		goto ok;
	}
}

int closedir(DIR *dir)
{
	if (close(dir->fd) == -1)
		return -1;

	free(dir);

	return 0;
}

size_t strlen(const char *s)
{
	if (s == NULL) return 0;
	size_t i;
	const char *t = s;
	for (i = 0; t[i]; i++) ;
	return i;
}

ssize_t getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream)
{
	return getdelim(lineptr, n, '\n', stream);
}

ssize_t getdelim(char **restrict lineptr, size_t *restrict n, int delimiter, FILE *restrict stream)
{
	if (!lineptr || !n || !stream) {
		errno = EINVAL;
		return -1;
	}

	char buf[LINE_MAX] = {0};

	if (fgets_delim(buf, sizeof(buf), stream, delimiter) == NULL)
		return ferror(stream) ? -1 : 0;

	size_t len = strlen(buf);

	if (*lineptr == NULL) {
		*lineptr = strdup(buf);
		*n = len;
		if (*lineptr == NULL)
			return -1;
	} else {
		if (*n < len) {
			*lineptr = realloc(*lineptr, len + 1);
			if (*lineptr == NULL)
				return -1;
		}
		strcpy(*lineptr, buf);
		*n = len;
	}

	return len;
}

int setsockopt(int fd, int level, int name, const void *value, socklen_t len)
{
	return syscall(__NR_setsockopt, fd, level, name, value, len);
}

int getsockopt(int fd, int level, int name, void *restrict value, socklen_t *restrict len)
{
	return syscall(__NR_getsockopt, fd, level, name, value, len);
}

int accept(int fd, struct sockaddr *restrict addr, socklen_t *restrict addrlen)
{
	return syscall(__NR_accept, fd, addr, addrlen);
}

int connect(int fd, const struct sockaddr *address, socklen_t len)
{
	return syscall(__NR_connect, fd, address, len);
}

int listen(int fd, int backlog)
{
	return syscall(__NR_listen, fd, backlog);
}

int socket(int domain, int type, int proto)
{
	return syscall(__NR_socket, domain, type, proto);
}

int bind(int fd, const struct sockaddr *saddr, socklen_t len)
{
	return syscall(__NR_bind, fd, saddr, len);
}

int abs(int i)
{
	return (i > 0) ? i : -i;
}

int pipe(int pipefd[2])
{
	return syscall(__NR_pipe, pipefd);
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	return syscall(__NR_select, nfds, readfds, writefds, exceptfds, timeout);
}

int gettimeofday(struct timeval *tv, void *tz)
{
	return syscall(__NR_gettimeofday, (long)tv, (long)tz, 0, 0, 0, 0, 0);
}

time_t time(time_t *tloc)
{
	return syscall(__NR_time, tloc);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	return 0;
}
#pragma GCC diagnostic pop

static char *const def_locale = "C";

struct locale_t {
    int mask;
    char *locales[LC_ALL + 2];
};

static char *current_locale[LC_ALL + 2] = {
	[0]           = NULL,

	[LC_ALL]      = def_locale,
	[LC_COLLATE]  = def_locale,
	[LC_CTYPE]    = def_locale,
	[LC_MESSAGES] = def_locale,
	[LC_MONETARY] = def_locale,
	[LC_NUMERIC]  = def_locale,
	[LC_TIME]     = def_locale,

	[LC_ALL + 1]  = NULL
};

static bool is_valid_locale(const char *locale)
{
    if (!strlen(locale)) return true;
    else if (!strcasecmp("C", locale)) return true;
    else if (!strcasecmp("POSIX", locale)) return true;

    return false;
}

locale_t newlocale(int category_mask, const char *locale, locale_t base)
{
    struct locale_t *ret = base;

    if (locale == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (~LC_ALL_MASK & category_mask) {
        errno = EINVAL;
        return NULL;
    }

    if (!is_valid_locale(locale)) {
        errno = ENOENT;
        return NULL;
    }

    if (ret == NULL && (ret = calloc(1, sizeof(struct locale_t))) == NULL)
        return NULL;

    ret->mask = category_mask;

    for (int i = 0; i < LC_ALL; i++) {
        if (category_mask & (1 << i)) {
            if ((ret->locales[i+1] = strdup(locale)) == NULL)
                goto fail;
        } else {
            if ((ret->locales[i+1] = strdup("C")) == NULL)
                goto fail;
        }
    }

    return ret;

fail:
    if (ret)
        freelocale(ret);

    return NULL;
}

void freelocale(locale_t locobj)
{
    struct locale_t *ret = locobj;

    for (int i = 0; i < LC_ALL + 2; i++)
        if (ret->locales[i])
            free(ret->locales[i]);

    free(locobj);
}

char *setlocale(int category, const char *locale)
{
	switch (category) {
		case LC_ALL:
		case LC_COLLATE:
		case LC_CTYPE:
		case LC_MESSAGES:
		case LC_MONETARY:
		case LC_NUMERIC:
		case LC_TIME:
			break;
		default:
			return NULL;
	}

	if (locale == NULL) {
done:
		return current_locale[category];

	} else if (!strlen(locale)) {
        if (current_locale[category] && current_locale[category] != def_locale)
            free(current_locale[category]);
        current_locale[category] = def_locale;
        goto done;

	} else if (!strcasecmp("C", locale) || !strcasecmp("POSIX", locale)) {
        if (current_locale[category] && current_locale[category] != def_locale)
            free(current_locale[category]);
        current_locale[category] = def_locale;
        goto done;

	}

	return NULL;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return syscall(__NR_nanosleep, req, rem);
}

static const char *const wday_name[7] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *const mon_name[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


size_t strftime(char *restrict s, size_t max, const char *restrict fmt, const struct tm *restrict tm)
{
	const char *restrict src = fmt;
	char *restrict dst = s, *restrict end = (s + max);

	//printf("strftime: fmt=<%s> max=%d\n", fmt, max);

	while (dst < (s + max) && *src)
	{
		//printf("checking: %c s=<%s> dst=<%p>\n", *src, s, dst);

		if (*src == '%') {
			if (*++src == 0) {
				return -1;
			}

			int remain = end - dst;
			int add = 0;

			switch(*src) {
				case 'a':
					add = snprintf(dst, remain, "%s", wday_name[tm->tm_wday]);
					break;
				case 'b':
					add = snprintf(dst, remain, "%s", mon_name[tm->tm_mon]);
					break;
				case 'e':
					add = snprintf(dst, remain, "%02u", tm->tm_mday);
					break;
				case 'H':
					add = snprintf(dst, remain, "%02u", tm->tm_hour);
					break;
				case 'M':
					add = snprintf(dst, remain, "%02u", tm->tm_min);
					break;
				case 'S':
					add = snprintf(dst, remain, "%02u", tm->tm_sec);
					break;
				case 'Z':
					add = snprintf(dst, remain, "UTC");
					break;
				case 'Y':
					add = snprintf(dst, remain, "%4u", tm->tm_year + 1900);
					break;
				case 'z':
					add = snprintf(dst, remain, "+0000"); /* TODO */
					break;
				case 'F':
					add = snprintf(dst, remain, "%04u-%02u-%02u", tm->tm_year, tm->tm_mon, tm->tm_mday);
					break;
				case 'c':
					add = snprintf(dst, remain, "%4u-%02u-%02uT%02u:%02u:%02u%s",
							tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
							tm->tm_hour, tm->tm_min, tm->tm_sec,
							"+0000");
					break;
				default:
					printf("UNKNOWN: %c\n", *src);
			}

			//printf("add=%d\n", add);

			dst += add - 1;
			src++;
		} else {
			*dst++ = *src++;
		}
	}

	return (dst-s);
}

/* TODO buffering? */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	const char *tmp_ptr = ptr;
	ssize_t res;

	if (ptr == NULL || stream == NULL || size == 0 || nmemb == 0)
		return 0;

	for (ret = 0; ret < nmemb; ret++)
	{
        if ( stream->mem ) {
            /* TODO */
            stream->error = ENOSYS;
            return ret;
        } else if ( (res = write(stream->fd, tmp_ptr, size)) != (ssize_t)size ) {
			if (res >= 0)
				stream->eof = true;
			else
				stream->error = errno;
			return ret;
		}
		tmp_ptr += size;
	}

	return ret;
}

#if 0
//static size_t _fread(void *ptr, size_t size, FILE *stream)
//{
//	size_t rem = size;
//	char *dst = ptr;
//
//	while (rem)
//	{
//		if (!stream->blen) {
//		}
//
//		memcpy(dst, stream->fd
//
//				}
//				}
#endif

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	char *tmp_ptr = ptr;
	ssize_t res;
	ssize_t to_read;
	ssize_t tmp;
    size_t mem_left;

	if (ptr == NULL || stream == NULL || size == 0 || nmemb == 0)
		return 0;

	for (ret = 0; ret < nmemb; ret++)
	{
		to_read = size;

		if ( stream->has_unwind ) {
			*tmp_ptr++ = stream->unwind;
			stream->has_unwind = false;
			to_read--;
		}

		if (!to_read)
			goto done;
more:
        mem_left = stream->mem_size - stream->offset;

		if (stream->buf) {
			/* get the most we can, that we need, from the buffer */
			tmp = min(to_read, stream->bhas - stream->bpos);

			/* if the buffer has any, deliver it */
			if (tmp) {
				memcpy(tmp_ptr, stream->buf + stream->bpos, tmp);
				tmp_ptr += tmp;
				to_read -= tmp;
				stream->bpos += tmp;
			}

			/* have we exhausted the buffer ? */
			if (stream->bpos >= stream->bhas) {

                /* handle fmemopen() */
                if (stream->mem) {
                    size_t mem_read = stream->blen;

                    if (mem_read > mem_left) {
                        stream->eof = 1;
                        if (mem_left == 0)
                            return 0;
                        mem_read = mem_left;
                    }

                    memcpy(stream->buf, stream->mem + stream->offset, mem_read);
                    stream->offset += mem_read;

                    res = mem_read;

                } else /* handle fopen() */
                    if ((res = read(stream->fd, stream->buf, stream->blen)) <= 0) {
					if (res == -1)
						stream->error = 1;
					else
						stream->eof = 1;
					return ret;
				}

				stream->bpos = 0;
				stream->bhas = res;
			}

			/* have we read a full record? */
			if (to_read)
				goto more;

			continue;
		} /* else, no buffering */

        /* handle fmemopen() */
        if (stream->mem) {
            size_t mem_read = to_read;

            if (mem_read > mem_left) {
                stream->eof = 1;
                if (mem_left == 0)
                    return 0;
                mem_read = mem_left;
            }

            memcpy(tmp_ptr, stream->mem + stream->offset, mem_read);
            stream->offset += mem_read;

            if (mem_read != (size_t)to_read)
                return ret; /* FIXME is this correct for nmemb ? */

        } else /* handle fopen() */
            if ((res = read(stream->fd, tmp_ptr, to_read)) != to_read) {
			if (res == 0)
				stream->eof = true;
			else
				stream->error = errno;
			return ret;
		}

done:
		tmp_ptr += to_read;
	}

	return ret;
}

int fflush(FILE *stream)
{
    if (stream == NULL) {
        errno = EINVAL;
        return -1;
    }
	return 0;
}

int fputs(const char *s, FILE *stream)
{
	return fwrite(s, strlen(s), 1, stream);
}

int puts(const char *s)
{
	if (fputs(s, stdout) == EOF)
		return EOF;
	if (putc('\n', stdout) == EOF)
		return EOF;

	return 0;
}

char *fgets(char *restrict s, int size, FILE *restrict stream)
{
	if (!s || !stream)
		return NULL;

	return fgets_delim(s, size, stream, '\n');
}

int statvfs(const char *restrict path, struct statvfs *restrict buf)
{
	return syscall(__NR_statfs, path, buf);
}

int fstatvfs(int fd, struct statvfs *buf)
{
	return syscall(__NR_fstatfs, fd, buf);
}

int fgetc(FILE *stream)
{
	char ch;

	if ( fread(&ch, 1, 1, stream) != 1 )
		return EOF;

	return (int)(ch);
}

int ungetc(int c, FILE *stream)
{
	if (!stream)
		return EOF;

	stream->has_unwind = true;
	stream->unwind = c;

	return c;
}

int getc(FILE *stream)
{
	return fgetc(stream);
}

int getchar(void)
{
	return getc(stdin);
}

char *strcat(char *dest, const char *src)
{
	if (dest == NULL || src == NULL) return dest;

	size_t dest_len,i;
	dest_len = strlen(dest);

	for (i = 0; src[i] != '\0'; i++)
		dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';

	return dest;
}

char *strncat(char *dest, const char *src, size_t n)
{
	if (dest == NULL || src == NULL) return dest;

	size_t i, dest_len;

	dest_len = strlen(dest);

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
		dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';

	return dest;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	if (s1 == NULL || s2 == NULL) return 0;

	size_t i = 0;
	while (i < n)
	{
		if (s1[i] != s2[i]) return 1;
		if (s1[i] == '\0') break;
		i++;
	}
	return 0;
}

int strcmp(const char *s1, const char *s2)
{
	if (s1 == NULL || s2 == NULL) return 0;

	size_t i = 0;
	while (true)
	{
		if (s1[i] != s2[i]) return 1;
		if (s1[i] == '\0' || s2[i] == '\0') break;
		i++;
	}
	return 0;
}

int strcasecmp(const char *s1, const char *s2)
{
	if (s1 == NULL || s2 == NULL) return 0;

	size_t i = 0;

	while (true)
	{
		if (tolower(s1[i]) != tolower(s2[i])) return 1;
		if (s1[i] == '\0' || s2[i] == '\0') break;
		i++;
	}

	return 0;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	if (s1 == NULL || s2 == NULL) return 0;

	size_t i = 0;

	while (i < n)
	{
		if (tolower(s1[i]) != tolower(s2[i])) return 1;
		if (s1[i] == '\0' || s2[i] == '\0') break;
		i++;
	}

	return 0;
}

char *strstr(const char *heystack, const char *needle)
{
	if (heystack == NULL || needle == NULL) return NULL;

	const char *ret = heystack;
	size_t len = strlen(needle);

	while (*ret)
	{
		if (*ret != *needle) {
			ret++;
			continue;
		}

		if (!strncmp(needle, ret, len))
			return (char *)ret;

		ret++;
	}

	return NULL;
}

void free(void *ptr)
{
	if (ptr == NULL) return;

	struct mem_alloc *buf = (struct mem_alloc *)((char *)ptr - sizeof(struct mem_alloc));

	if (buf < first || buf > last)
		exit(100);
	if ((buf->flags & MF_FREE) == 1)
		exit(101);

	//printf("free: [%ld] = %p\n", buf->len, ptr);
	free_alloc(buf);
}

__attribute__((malloc(free,1)))
void *malloc(size_t size)
{
	//printf("malloc.start:   (%ld)\n", size);
	if (size <= 0)
		return NULL;

	struct mem_alloc *ret = NULL;
	if ((ret = alloc_mem(size)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	//printf("malloc.end:  [%ld] = %p\n", size, ret);
	return (((char *)ret->start) + sizeof(struct mem_alloc));
}

void *realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
		return malloc(size);
	void *new = malloc(size);
	memcpy(new, ptr, size);
	return new;
}

void *memset(void *s, int _c, size_t _n)
{
	const char c = _c;
	register size_t n = _n;

	if (s == NULL) return s;

	register unsigned long long *restrict l_ptr;
	unsigned long long blah;
	char *s_ptr;

	if (n > sizeof(blah) ) {
		memset(&blah, (char)c, sizeof(blah));
		l_ptr = s;
		for (;n > sizeof(blah); n -= sizeof(blah))
			*(l_ptr++) = blah;

		s_ptr = (void *)l_ptr;
	} else
		s_ptr = s;


	for (size_t i = 0; i < n; i++)
		*(s_ptr++) = c;

	return s;
}

__attribute__((malloc))
void *calloc(size_t nmemb, size_t size)
{
	void *ret;
	size_t len = nmemb * size;
	if (len <= 0)
		return NULL;
	ret = malloc(len);
	if (ret == NULL)
		return NULL;
	memset(ret, 0, len);

	return ret;
}

int fputc(int c, FILE *stream)
{
	unsigned char ch = c;
    return fwrite(&ch, 1, 1, stream);
}

int mkdir(const char *path, mode_t mode)
{
	return syscall(__NR_mkdir, path, mode);
}

int mknod(const char *pathname, mode_t mode, dev_t dev)
{
	return syscall(__NR_mknod, pathname, mode, dev);
}

pid_t setsid(void)
{
	return syscall(__NR_setsid);
}

int mkfifo(const char *path, mode_t mode)
{
	/* TODO & ~(something) for mode? */
	return mknod(path, S_IFIFO|mode, 0);
}

int dup(int oldfd)
{
	return syscall(__NR_dup, oldfd);
}

int putchar(int c)
{
	return putc(c, stdout);
}

int putchar_unlocked(int c)
{
	return(putchar(c));
}

int atexit(void (*function)(void))
{
	if (function == NULL) {
		errno = EINVAL;
		return -1;
	}

	struct atexit_fun *node;
	if ((node = calloc(1, sizeof(struct atexit_fun))) == NULL) {
		return -1;
	}

	node->next = global_atexit_list;
	node->function = function;
	global_atexit_list = node;
	return 0;
}

int kill(pid_t pid, int sig)
{
	return syscall(__NR_kill, pid, sig);
}

pid_t getpid(void)
{
	return syscall(__NR_getpid);
}

uid_t getuid(void)
{
	return syscall(__NR_getuid);
}

uid_t geteuid(void)
{
	return syscall(__NR_getuid);
}

gid_t getgid(void)
{
	return syscall(__NR_getgid);
}

gid_t getegid(void)
{
	return syscall(__NR_getegid);
}

int raise(int sig)
{
    return pthread_kill(pthread_self(), sig);
	//return kill(getpid(), sig);
}

static FILE *pw = NULL;
static struct passwd pass = {
	.pw_name   = NULL,
	.pw_passwd = NULL,
	.pw_gecos  = NULL,
	.pw_shell  = NULL,
	.pw_dir    = NULL
};

inline static void free_pwnam() 
{
	if (pass.pw_name)   { free(pass.pw_name);   pass.pw_name   = NULL; }
	if (pass.pw_passwd) { free(pass.pw_passwd); pass.pw_passwd = NULL; }
	if (pass.pw_gecos)  { free(pass.pw_gecos);  pass.pw_gecos  = NULL; }
	if (pass.pw_shell)  { free(pass.pw_shell);  pass.pw_shell  = NULL; }
	if (pass.pw_dir)    { free(pass.pw_dir);    pass.pw_dir    = NULL; }
}

inline static struct passwd *getpw(const char *name, uid_t uid)
{
	if (pw == NULL) {
		if ((pw = fopen("/etc/passwd","r")) == NULL)
			return NULL;
	}

	rewind(pw);

	size_t   len   = 0;
	ssize_t  bytes = 0;
	char    *line  = NULL;
	int      rc;

	do {
		bytes = getline(&line, &len, pw);

		if (line == NULL || len <=0 || bytes <= 0 || feof(pw) || ferror(pw)) {
			if (ferror(pw)) {
				fclose(pw);
				pw = NULL;
			}

			if (line) {
				free(line);
				line = NULL;
			}
			return NULL;
		}

		free_pwnam();	
		rc = sscanf(line, " %ms:%ms:%d:%d:%ms:%ms:%ms ",
				&pass.pw_name,
				&pass.pw_passwd,
				&pass.pw_uid,
				&pass.pw_gid,
				&pass.pw_gecos,
				&pass.pw_dir,
				&pass.pw_shell
				);

		free(line);
		line = NULL;

		if (rc == EOF && ferror(pw))
			goto skip;

		if (rc < 4)
			goto skip;

		if (name) {
			if (!strcmp(name, pass.pw_name))
				return &pass;
		} else {
			if (uid == pass.pw_uid)
				return &pass;
		}
skip:
		free_pwnam();
	} while(1);

	//fail:
	free_pwnam();
	if (pw)
		fclose(pw);
	if (line)
		free(line);
	return NULL;
}

struct passwd *getpwnam(const char *name)
{
	return getpw(name, 0);
}

struct passwd *getpwuid(uid_t uid)
{
	return getpw(NULL, uid);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
struct group *getgrnam(const char *name)
{
	/* TODO */
	return NULL;
}

struct group *getgrgid(gid_t gid)
{
	/* TODO */
	return NULL;
}

int getgroups(int size, gid_t list[])
{
	errno = ENOMEM;
	if (list == NULL)
		errno = EINVAL;
	return -1;
}
#pragma GCC diagnostic pop

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	return syscall(__NR_sigprocmask, how, set, oldset, sizeof(sigset_t));
}

__attribute__((noreturn))
void abort(void)
{
	sigset_t signal_mask;

	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGABRT);
	sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);

	raise(SIGABRT);

    const struct sigaction sa = {
        .sa_handler = SIG_DFL,
        .sa_mask = 0,
        .sa_flags = 0
    };

	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGABRT);
	sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
    sigaction(SIGABRT, &sa, NULL);
    raise(SIGABRT);

	/* FIXME this should restore the defaul signal handler for SIGABRT,
	 * then raise again */
	exit(1);
}

int sigismember(const sigset_t *set, int signum)
{
	return *set & (1 << signum);
}

int sigemptyset(sigset_t *set)
{
	if (!set)
		return -1;

	*set = 0;
	return 0;
}

int sigfillset(sigset_t *set)
{
	*set = ~0UL;
	return 0;
}

int sigaddset(sigset_t *set, int signo)
{
	if (!set || signo <= 0 || signo > 63) {
		errno = EINVAL;
		return -1;
	}

	*set |= (1 << signo);

	return 0;
}

int sigdelset(sigset_t *set, int signo)
{
	if (!set || signo <= 0 || signo > 63) {
		errno = EINVAL;
		return -1;
	}

	*set &= ~(1 << signo);

	return 0;
}

void perror(const char *s)
{
	if (s && *s)
		fprintf(stdout, "%s: %s\n", strerror(errno), s);
	else
		fprintf(stdout, "%s\n", strerror(errno));
}

char *strerror(int errnum)
{
	static char buf[64];

	switch(errnum)
	{
		case 0:
			return "ENONE";
		case EPERM:
			return "EPERM";
		case ENOENT:
			return "ENOENT";
		case ESRCH:
			return "ESRCH";
		case EINTR:
			return "EINTR";
		case EBADF:
			return "EBADF";
		case ENOMEM:
			return "ENOMEM";
		case EACCES:
			return "EACCES";
		case EBUSY:
			return "EBUSY";
		case EEXIST:
			return "EEXIST";
		case EINVAL:
			return "EINVAL";
		case ENOTTY:
			return "ENOTTY";
		case EDEADLK:
			return "EDEADLK";
		case ENOSYS:
			return "ENOSYS";
		case EOVERFLOW:
			return "EOVERFLOW";
		case EXDEV:
			return "EXDEV";
		case ECONNREFUSED:
			return "ECONNREFUSED";
		case ENODEV:
			return "ENODEV";
		case ENOTDIR:
			return "ENOTDIR";
		default:
			return "EUNKNOWN!";
			errno = EINVAL;
			return buf;
	}
}

/*
void *memcpy(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++)
		((char *)dest)[i] = ((char *)src)[i];
	return dest;
}
*/

void *memccpy(void *dest, const void *src, int c, size_t n)
{
    register const unsigned char *s_ptr = src;
    register const unsigned char cmp = (unsigned char)c;
    register unsigned char *d_ptr = dest;
    register size_t cnt = n;

    while (*s_ptr && cnt)
    {
        if (*s_ptr == cmp)
            return ++d_ptr;
        *d_ptr++ = *s_ptr++;
        cnt--;
    }

    return NULL;
}

void *memmove(void *dest, const void *src, size_t n)
{
    void *tmp;

    if ((tmp = malloc(n)) == NULL)
        return NULL;

    memcpy(tmp, src, n);
    memcpy(dest, tmp, n);

    free(tmp);
    return dest;
}

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
    size_t cnt;

    for (cnt = 0; cnt < n; cnt++)
        dest[cnt] = src[cnt];

    return dest;
}

wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n)
{
    wchar_t *tmp;

    if ((tmp = malloc(n * sizeof(wchar_t))) == NULL)
        return NULL;

    wmemcpy(tmp, src, n);
    wmemcpy(dest, tmp, n);

    free(tmp);
    return dest;
}

#define LONG_SIZE sizeof(unsigned long long)
void *memcpy(void *dest, const void *src, size_t n)
{
	register size_t todo = n;

	if (dest == NULL)
		return dest;

	register const unsigned long long *src_ptr;
	register unsigned long long *dst_ptr;
	const unsigned char *s_ptr;
	unsigned char *d_ptr;

	s_ptr = src;
	d_ptr = dest;

	src_ptr = src;
	dst_ptr = dest;

	if (todo > LONG_SIZE) {
		for (;todo > LONG_SIZE; todo -= LONG_SIZE) {
			*(dst_ptr++) = *(src_ptr++);
		}

		s_ptr += (n - todo);
		d_ptr += (n - todo);
	}

	for (size_t i = 0; i < todo; i++) {
		*(d_ptr++) = *(s_ptr++);
	}

	return dest;
}
#undef LONG_SIZE

char *strdup(const char *s)
{
	char *ret;
	size_t len = strlen(s) + 1;

	if ((ret = malloc(len)) == NULL)
		return NULL;

	memcpy(ret, s, len);
	return ret;
}

char *strndup(const char *s, size_t n)
{
	char *ret;
	size_t len = strlen(s);
	if (len > n) len = n;
	n++;

	if ((ret = malloc(len)) == NULL)
		return NULL;

	ret[len] = '\0';

	memcpy(ret, s, len);
	return ret;
}

FILE *setmntent(const char *file, const char *type) {
    FILE *ret;

    if ((ret = fopen(file, type)) == NULL)
        return NULL;

    return ret;
}

static void free_mntent(struct mntent *me)
{
    if (me->mnt_fsname)
        free(me->mnt_fsname);
    if (me->mnt_opts)
        free(me->mnt_opts);
    if (me->mnt_type)
        free(me->mnt_type);
    if (me->mnt_dir)
        free(me->mnt_dir);
}

static struct mntent mntent_ret;

struct mntent *getmntent(FILE *stream) {
    char *lineptr;
    size_t len;

    errno = 0;

    if (stream == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (feof(stream))
        return NULL;

    if (ferror(stream)) {
        errno = EBADF;
        return NULL;
    }

    lineptr = NULL;

    while(true)
    {
        if (lineptr) {
            free(lineptr);
            lineptr = NULL;
        }

        len = 0;

        if ((getline(&lineptr, &len, stream)) == -1)
            goto done;

        if (!lineptr || !*lineptr || *lineptr == '#')
            continue;

        free_mntent(&mntent_ret);
        memset(&mntent_ret, 0, sizeof(mntent_ret));

        if (sscanf(lineptr, "%ms %ms %ms %ms %d %d",
                       &mntent_ret.mnt_fsname,
                       &mntent_ret.mnt_dir,
                       &mntent_ret.mnt_type,
                       &mntent_ret.mnt_opts,
                       &mntent_ret.mnt_freq,
                       &mntent_ret.mnt_passno) < 4) {
            continue;
        }

        /* TODO expand escaped characters */
        
        break;
    }
done:
    if (lineptr)
        free(lineptr);
    
    return &mntent_ret;
}

int addmntent(FILE *stream, const struct mntent *mnt)
{
    if (stream == NULL || mnt == NULL || mnt->mnt_fsname == NULL || 
            mnt->mnt_opts == NULL || mnt->mnt_dir == NULL || mnt->mnt_type == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* TODO */

    errno = ENOSYS;
    return -1;
}

int endmntent(FILE *stream)
{
    /* TODO spec compliant? */
    free_mntent(&mntent_ret);

    if (stream)
        return fclose(stream);
    else {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

/* replace back with stderr here and perror etc. */

void err(int eval, const char *fmt, ...)
{
	int en = errno;
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stdout, fmt, ap);
		va_end(ap);
		fprintf(stdout, ": ");
	}
	fprintf(stdout, "%s\n", strerror(en));
	exit(eval);
}

void errx(int eval, const char *fmt, ...)
{
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stdout, fmt, ap);
		va_end(ap);
	}
	fprintf(stdout, "\n");
	exit(eval);
}

void warn(const char *fmt, ...)
{
	int en = errno;
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stdout, fmt, ap);
		va_end(ap);
		fprintf(stdout, ": ");
	}
	fprintf(stdout, "%s\n", strerror(en));
}

void warnx(const char *fmt, ...)
{
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stdout, fmt, ap);
		va_end(ap);
	}
	fprintf(stdout, "\n");
}

int pthread_kill(pthread_t thread, int sig)
{
    return syscall(__NR_tkill, thread->my_tid, sig);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
	return ENOMEM;
}

int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr)
{
	memset(rwlock, 0, sizeof(pthread_rwlock_t));
	return ENOMEM;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
	return ENOMEM;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
	return EBUSY;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
	return ENOMEM;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
	return EBUSY;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
	return ENOMEM;
}
#pragma GCC diagnostic pop

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return (void *)syscall(__NR_mmap, addr, length, prot, flags, fd, offset);
}

#define STACK_SIZE (1024 * 1024)

/* invoked from clone.S */
int __start_thread(int (*fn)(void *), void *arg)
{
	_exit(fn(arg));
}

pid_t fork(void)
{
	pid_t ret;
	//printf("fork: pre environ=%p environ[0]=%p environ[0]=%s\n", environ, environ[0], environ[0]);
	ret = (pid_t)syscall(__NR_fork);
	//printf("fork: post[%d] environ=%p environ[0]=%p environ[0]=%s\n", ret, environ, environ[0], environ[0]);
	return ret;
}

extern int _clone(unsigned long flags, void *stack, void *parent_id, 
		void *child_tid, unsigned long newtls, int (*fn)(void *), void *arg);

/* the c-library wrapper */
int clone(int (*fn)(void *), void *stack, int flags, void *arg, ...)
{
	int parent_id = gettid();
	int child_tid = 0;
	int ret;

	ret = _clone(
			flags, 
			stack, 
			&parent_id, 
			&child_tid, 
			0,
			fn,
			arg
			);

	if (ret < 0) {
		return ret;
	} else if (ret == 0) {
		for (;;) ;
	} else
		return child_tid;
}

pthread_t pthread_self(void)
{
    return __pthread_self();
}

static const pthread_attr_t default_pthread_attr = {
    .stackaddr = NULL,
    .stacksize = STACK_SIZE,
};

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
	int clone_flags = CLONE_VM|CLONE_FS|CLONE_FILES
		|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS
		|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID|CLONE_DETACHED;

	__attribute__((unused)) struct __pthread *self;
	struct __pthread *new;
	void *stack;
    const pthread_attr_t *at = attr ? attr : &default_pthread_attr;

	if ((new = malloc(sizeof(struct __pthread))) == NULL) {
		errno = ENOMEM;
		return -1;
	}

    memcpy(&new->attrs, at, sizeof(pthread_attr_t));

    if (at->stackaddr) {
        stack = at->stackaddr;
    } else if ((stack = mmap(NULL, at->stacksize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON|MAP_STACK, -1, 0)) == MAP_FAILED) {
		free(new);
		return -1;
	}

	self = __pthread_self();
	new->self = new;
	new->errnum = 0;
	new->start_routine = start_routine;
	new->start_arg = arg;
	new->stack = stack;
	new->stack_size = STACK_SIZE;
	new->parent_tid = gettid();
	new->my_tid = 0;
    new->attrs.stackaddr = stack;
	*thread = new;

	_Pragma("GCC diagnostic push")
        _Pragma("GCC diagnostic ignored \"-Wincompatible-pointer-types\"")
		int ret = _clone(
				clone_flags, 
				(char *)stack + STACK_SIZE, 
				&new->parent_tid,
				&new->my_tid,
				(unsigned long)new,
				/* i have no idea how to handle the fact clone wants int return.
				 * but pthread create void return */
				start_routine,
				arg
				);
	_Pragma("GCC diagnostic pop")

        if (ret < 0) {
			*thread = NULL;
			return ret;
		}

	return 0;
}

int tcsendbreak(int fd, int duration)
{
    return ioctl(fd, TCSBRKP, duration);
}

void cfmakeraw(struct termios *tio)
{
    tio->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio->c_oflag &= ~OPOST;
    tio->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio->c_cflag &= ~(CSIZE | PARENB);
    tio->c_cflag |= CS8;
}

int tcgetattr(int fd, struct termios *tio)
{
	return ioctl(fd, TCGETS, tio);
}

speed_t cfgetospeed(const struct termios *tio)
{
    return tio->c_ospeed;
}

int tcsetattr(int fd, int optional_actions, const struct termios *tio)
{
	int sc;

	switch (optional_actions)
	{
        case 0:
		case TCSANOW:
			sc = TCSETS;
			break;
		case TCSADRAIN:
			sc = TCSETSW;
			break;
		case TCSAFLUSH:
			sc = TCSETSF;
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	return ioctl(fd, sc, tio);
}

/* in the abscence of a standard, use Linux's */
int mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data)
{
	return syscall(__NR_mount, source, target, fstype, flags, data);
}

static char ttyname_string[NAME_MAX];

int isatty(int fd)
{
	struct termios tio;
	const int rc = tcgetattr(fd, &tio);

	if (rc == -1 && errno == ENOTTY)
		return 0;
	else if (rc == -1)
		return -1;
	else
		return 0;
}

char *ttyname(int fd)
{
	if (!isatty(fd)) {
		errno = ENOTTY;
		return NULL;
	}

	char buf[64];
	ssize_t len;

	snprintf(buf, sizeof(buf), "/proc/self/fd/%d", fd);

	if ((len = readlink(buf, ttyname_string, sizeof(ttyname_string))) == -1)
		return NULL;

	ttyname_string[len] = 0;
	return ttyname_string;
}

void closelog(void)
{
}

static int unix_socket = -1;
static int sl_options  = 0;
static int sl_facility = LOG_USER;
static int sl_mask     = 0;
static const char *sl_ident = NULL;

static void open_syslog(void)
{
	if ((unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
		return;

	struct sockaddr_un sun = {
		.sun_family = AF_UNIX,
		.sun_path   = "/dev/log"
	};

	if (connect(unix_socket, (struct sockaddr *)&sun, sizeof(sun)) == -1) {
		close(unix_socket);
		unix_socket = -1;
		return;
	}
}

void openlog(const char *ident, int logopt, int facility)
{
	sl_facility = facility;
	sl_options  = logopt;

	if (sl_ident)
		free((void *)sl_ident);

	sl_ident = strdup(ident);

	if ((logopt & LOG_NDELAY))
		open_syslog();
}

int setlogmask(int maskpri)
{
	int old_mask = sl_mask;
	sl_mask = maskpri;
	return old_mask;
}

void syslog(int priority, const char *message, ...)
{
	va_list vararg;
	va_start(vararg, message);
	vsyslog(priority, message, vararg);
	va_end(vararg);
}

void vsyslog(int priority, const char *message, va_list ap)
{
	if (unix_socket == -1)
		open_syslog();

	if (unix_socket == -1 && !(sl_options & LOG_CONS))
		return;

	int fd = unix_socket;
    int con_fd = -1;

    if (unix_socket == -1 || (sl_options & LOG_CONS))
		if ((con_fd = open("/dev/console", O_WRONLY)) == -1) {
			perror("open /dev/console");
			return;
		}

	if (unix_socket == -1)
        fd = con_fd;

	char t_mess[BUFSIZ];
	char t_date[18]; /* Apr 10 00:00:00 */
	char t_log[sizeof(t_mess) + sizeof(t_date) + 1];

	struct tm *tmp;
	time_t t = time(NULL);
	tmp = localtime(&t);
	
	if (!tmp)
		return;

	strftime(t_date, sizeof(t_date), "%b %e %H:%M:%S", tmp);
	vsnprintf(t_mess, sizeof(t_log), message, ap);

	if ((sl_options & LOG_PID))
		snprintf(t_log, PATH_MAX, "<%u>%s %s[%lu]: %s\n", sl_facility|priority, t_date, sl_ident, getpid(), t_mess);
	else
		snprintf(t_log, PATH_MAX, "<%u>%s %s: %s\n",      sl_facility|priority, t_date, sl_ident,           t_mess);

    size_t len = strlen(t_log);

	if (write(fd, t_log, len) == -1 || unix_socket == -1)
		close(fd);

    if ((sl_options & LOG_CONS) && (fd != con_fd)) {
        write(con_fd, t_log, len);
        close(con_fd);
    }
}

ssize_t readlink(const char *pathname, char *buf, size_t siz)
{
	return syscall(__NR_readlink, pathname, buf, siz);
}

long sysconf(int name)
{
	switch(name)
	{
		case _SC_CLK_TCK:
			return 100;
		case _SC_NGROUPS_MAX:
			return NGROUPS_MAX;
		default:
			errno = EINVAL;
			return -1;
	}
}

int killpg(int pgrp, int sig)
{
	return syscall(__NR_kill, -pgrp, sig);
}

int pthread_setcancelstate(int state, int *oldstate)
{
    pthread_t self = pthread_self();

    switch (state)
    {
        case PTHREAD_CANCEL_ENABLE:
        case PTHREAD_CANCEL_DISABLE:
            if (oldstate)
                *oldstate = self->attrs.cancelstate;
            self->attrs.cancelstate = state;
            return 0;

        default:
            return EINVAL;
    }

}

int pthread_setcanceltype(int type, int *oldtype)
{
    pthread_t self = pthread_self();

    switch(type)
    {
        case PTHREAD_CANCEL_DEFERRED:
        case PTHREAD_CANCEL_ASYNCHRONOUS:
            if (oldtype)
                *oldtype = self->attrs.canceltype;
            self->attrs.canceltype = type;
            return 0;

        default:
            return EINVAL;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int pthread_join(pthread_t thread, void **retval)
{
	return ESRCH;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    memcpy(attr, &default_pthread_attr, sizeof(pthread_attr_t));
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    memset(attr, 0, sizeof(pthread_attr_t));
    return 0;
}

int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr)
{
	memset(mutex, 0, sizeof(pthread_mutex_t));
	return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	return 0;
}
#pragma GCC diagnostic pop

int strerror_r(int errnum, char *buf, size_t buflen)
{

	char *tmp = strerror(errnum);

	if (tmp) {
		strncpy(buf, strerror(errnum), buflen);
		return 0;
	}

	errno = EINVAL;
	return -1;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
double sinh(double x)
{
	return 0;
}

double cosh(double x)
{
	return 0;
}

double tanh(double x)
{
	return 0;
}
#pragma GCC diagnostic pop

double fmod(double x, double y)
{
	if (isnan(x) || isnan(y))
		return NAN;

	if (isinf(x)) {
		errno = EDOM;
		return NAN;
	}

	if (y == 0.0) {
		errno = EDOM;
		return NAN;
	}

	return x - (x/y) * y;
}

double sin(double x)
{
	if (isnan(x))
		return NAN;

	if (isinf(x)) {
		errno = EDOM;
		return NAN;
	}

	x = fmod(x,  (2 * M_PI));

	while (x < 0)
		x += 2 * M_PI;

	while (x > 2 * M_PI)
		x -= 2 * M_PI;

	double epsilon = 0.1e-16;
	double sinus = 0.0;
	double sign = 1.0;
	double term = x;
	double n = 1;

	while (term > epsilon) {
		sinus += sign * term;
		sign = -sign;
		term *= x * x / (n+1) / (n+2);
		n += 2;
	}

	return sinus;
}

static inline double fac(int a)
{
    double ret = 1;
    for (int i = 1; i <= a; i++)
        ret *= i;
    return ret;
}

double cos(double x)
{
	x = fmod(x,  (2 * M_PI));

	while (x < 0)
		x += 2 * M_PI;

	while (x > 2 * M_PI)
		x -= 2 * M_PI;

	double epsilon = 0.1e-16;
	double cosus = 1.0;
	double sign = 1.0;
	double term = 0;
	double n = 2;

	do {
		cosus += sign * term;
		sign = -sign;
		term = pow(x, n) / fac(n);
		n += 2;
	} while (term > epsilon);

	return cosus;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
double acos(double x)
{
	return 0;
}

double atan(double x)
{
	return 0;
}

double asin(double x)
{
	return 0;
}

double atan2(double y, double x)
{
	return 0;
}

double fabs(double x)
{
	double res;
	__asm__("fabs" : "=t" (res) : "0" (x));
	return res;
}

/* log e */
double log(double x)
{
	return 0;
}

double log10(double x)
{
	return 0;
}
#pragma GCC diagnostic pop

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x

double copysign(double x, double y)
{
	__HI(x) = (__HI(x)&0x7fffffff)|(__HI(y)&0x80000000);
	return x;
}

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
		twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
		huge   = 1.0e+300,
		tiny   = 1.0e-300;

double scalbn (double x, int n)
{
	int  k,hx,lx;
	hx = __HI(x);
	lx = __LO(x);
	k = (hx&0x7ff00000)>>20;		/* extract exponent */
	if (k==0) {				/* 0 or subnormal x */
		if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
		x *= two54; 
		hx = __HI(x);
		k = ((hx&0x7ff00000)>>20) - 54; 
		if (n< -50000) return tiny*x; 	/*underflow*/
	}
	if (k==0x7ff) return x+x;		/* NaN or Inf */
	k = k+n; 
	if (k >  0x7fe) return huge*copysign(huge,x); /* overflow  */
	if (k > 0) 				/* normal result */
	{__HI(x) = (hx&0x800fffff)|(k<<20); return x;}
	if (k <= -54) {
		if (n > 50000) { 	/* in case integer overflow in n+k */
			return huge*copysign(huge,x);	/*overflow*/
		} else return tiny*copysign(tiny,x); 	/*underflow*/
	}
	k += 54;				/* subnormal result */
	__HI(x) = (hx&0x800fffff)|(k<<20);
	return x*twom54;
}

static const double
bp[] = {1.0, 1.5,},
	dp_h[] = { 0.0, 5.84962487220764160156e-01,}, /* 0x3FE2B803, 0x40000000 */
	dp_l[] = { 0.0, 1.35003920212974897128e-08,}, /* 0x3E4CFDEB, 0x43CFD006 */
	zero    =  0.0,
	one	=  1.0,
	two	=  2.0,
	two53	=  9007199254740992.0,	/* 0x43400000, 0x00000000 */
	/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
	L1  =  5.99999999999994648725e-01, /* 0x3FE33333, 0x33333303 */
	L2  =  4.28571428578550184252e-01, /* 0x3FDB6DB6, 0xDB6FABFF */
	L3  =  3.33333329818377432918e-01, /* 0x3FD55555, 0x518F264D */
	L4  =  2.72728123808534006489e-01, /* 0x3FD17460, 0xA91D4101 */
	L5  =  2.30660745775561754067e-01, /* 0x3FCD864A, 0x93C9DB65 */
	L6  =  2.06975017800338417784e-01, /* 0x3FCA7E28, 0x4A454EEF */
	P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
	P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
	P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
	P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
	P5   =  4.13813679705723846039e-08, /* 0x3E663769, 0x72BEA4D0 */
	lg2  =  6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
	lg2_h  =  6.93147182464599609375e-01, /* 0x3FE62E43, 0x00000000 */
	lg2_l  = -1.90465429995776804525e-09, /* 0xBE205C61, 0x0CA86C39 */
	ovt =  8.0085662595372944372e-0017, /* -(1024-log2(ovfl+.5ulp)) */
	cp    =  9.61796693925975554329e-01, /* 0x3FEEC709, 0xDC3A03FD =2/(3ln2) */
	cp_h  =  9.61796700954437255859e-01, /* 0x3FEEC709, 0xE0000000 =(float)cp */
	cp_l  = -7.02846165095275826516e-09, /* 0xBE3E2FE0, 0x145B01F5 =tail of cp_h*/
	ivln2    =  1.44269504088896338700e+00, /* 0x3FF71547, 0x652B82FE =1/ln2 */
	ivln2_h  =  1.44269502162933349609e+00, /* 0x3FF71547, 0x60000000 =24b 1/ln2*/
	ivln2_l  =  1.92596299112661746887e-08; /* 0x3E54AE0B, 0xF85DDF44 =1/ln2 tail*/

double sqrt(double x)
{
	double ret = 0.0;
	__asm__ ("fsqrt" : "=t" (ret) : "0" (x));
	return ret;
}

static double __ieee754_pow(double x, double y)
{
	double z,ax,z_h,z_l,p_h,p_l;
	double y1,t1,t2,r,s,t,u,v,w;
	int i0,__attribute__((unused)) i1,i,j,k,yisint,n;
	int hx,hy,ix,iy;
	unsigned lx,ly;

	i0 = ((*(int*)&one)>>29)^1; i1=1-i0;
	hx = __HI(x); lx = __LO(x);
	hy = __HI(y); ly = __LO(y);
	ix = hx&0x7fffffff;  iy = hy&0x7fffffff;

	/* y==zero: x**0 = 1 */
	if ((iy|ly)==0) return one;

	/* +-NaN return x+y */
	if (ix > 0x7ff00000 || ((ix==0x7ff00000)&&(lx!=0)) ||
			iy > 0x7ff00000 || ((iy==0x7ff00000)&&(ly!=0)))
		return x+y;

	/* determine if y is an odd int when x < 0
	 * yisint = 0	... y is not an integer
	 * yisint = 1	... y is an odd int
	 * yisint = 2	... y is an even int
	 */
	yisint  = 0;
	if (hx<0) {
		if (iy>=0x43400000) yisint = 2; /* even integer y */
		else if (iy>=0x3ff00000) {
			k = (iy>>20)-0x3ff;	   /* exponent */
			if (k>20) {
				j = ly>>(52-k);
				if ((j<<(52-k))==ly) yisint = 2-(j&1);
			} else if (ly==0) {
				j = iy>>(20-k);
				if ((j<<(20-k))==iy) yisint = 2-(j&1);
			}
		}
	}

	/* special value of y */
	if (ly==0) {
		if (iy==0x7ff00000) {	/* y is +-inf */
			if (((ix-0x3ff00000)|lx)==0)
				return  y - y;	/* inf**+-1 is NaN */
			else if (ix >= 0x3ff00000)/* (|x|>1)**+-inf = inf,0 */
				return (hy>=0)? y: zero;
			else			/* (|x|<1)**-,+inf = inf,0 */
				return (hy<0)?-y: zero;
		}
		if (iy==0x3ff00000) {	/* y is  +-1 */
			if (hy<0) return one/x; else return x;
		}
		if (hy==0x40000000) return x*x; /* y is  2 */
		if (hy==0x3fe00000) {	/* y is  0.5 */
			if (hx>=0)	/* x >= +0 */
				return sqrt(x);
		}
	}

	ax   = fabs(x);
	/* special value of x */
	if (lx==0) {
		if (ix==0x7ff00000||ix==0||ix==0x3ff00000){
			z = ax;			/*x is +-0,+-inf,+-1*/
			if (hy<0) z = one/z;	/* z = (1/|x|) */
			if (hx<0) {
				if (((ix-0x3ff00000)|yisint)==0) {
					z = (z-z)/(z-z); /* (-1)**non-int is NaN */
				} else if (yisint==1)
					z = -z;		/* (x<0)**odd = -(|x|**odd) */
			}
			return z;
		}
	}

	n = (hx>>31)+1;

	/* (x<0)**(non-int) is NaN */
	if ((n|yisint)==0) return NAN;//(x-x)/(x-x);

	s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
	if ((n|(yisint-1))==0) s = -one;/* (-ve)**(odd int) */

	/* |y| is huge */
	if (iy>0x41e00000) { /* if |y| > 2**31 */
		if (iy>0x43f00000){	/* if |y| > 2**64, must o/uflow */
			if (ix<=0x3fefffff) return (hy<0)? huge*huge:tiny*tiny;
			if (ix>=0x3ff00000) return (hy>0)? huge*huge:tiny*tiny;
		}
		/* over/underflow if x is not close to one */
		if (ix<0x3fefffff) return (hy<0)? s*huge*huge:s*tiny*tiny;
		if (ix>0x3ff00000) return (hy>0)? s*huge*huge:s*tiny*tiny;
		/* now |1-x| is tiny <= 2**-20, suffice to compute
		   log(x) by x-x^2/2+x^3/3-x^4/4 */
		t = ax-one;		/* t has 20 trailing zeros */
		w = (t*t)*(0.5-t*(0.3333333333333333333333-t*0.25));
		u = ivln2_h*t;	/* ivln2_h has 21 sig. bits */
		v = t*ivln2_l-w*ivln2;
		t1 = u+v;
		__LO(t1) = 0;
		t2 = v-(t1-u);
	} else {
		double ss,s2,s_h,s_l,t_h,t_l;
		n = 0;
		/* take care subnormal number */
		if (ix<0x00100000)
		{ax *= two53; n -= 53; ix = __HI(ax); }
		n  += ((ix)>>20)-0x3ff;
		j  = ix&0x000fffff;
		/* determine interval */
		ix = j|0x3ff00000;		/* normalize ix */
		if (j<=0x3988E) k=0;		/* |x|<sqrt(3/2) */
		else if (j<0xBB67A) k=1;	/* |x|<sqrt(3)   */
		else {k=0;n+=1;ix -= 0x00100000;}
		__HI(ax) = ix;

		/* compute ss = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
		u = ax-bp[k];		/* bp[0]=1.0, bp[1]=1.5 */
		v = one/(ax+bp[k]);
		ss = u*v;
		s_h = ss;
		__LO(s_h) = 0;
		/* t_h=ax+bp[k] High */
		t_h = zero;
		__HI(t_h)=((ix>>1)|0x20000000)+0x00080000+(k<<18);
		t_l = ax - (t_h-bp[k]);
		s_l = v*((u-s_h*t_h)-s_h*t_l);
		/* compute log(ax) */
		s2 = ss*ss;
		r = s2*s2*(L1+s2*(L2+s2*(L3+s2*(L4+s2*(L5+s2*L6)))));
		r += s_l*(s_h+ss);
		s2  = s_h*s_h;
		t_h = 3.0+s2+r;
		__LO(t_h) = 0;
		t_l = r-((t_h-3.0)-s2);
		/* u+v = ss*(1+...) */
		u = s_h*t_h;
		v = s_l*t_h+t_l*ss;
		/* 2/(3log2)*(ss+...) */
		p_h = u+v;
		__LO(p_h) = 0;
		p_l = v-(p_h-u);
		z_h = cp_h*p_h;		/* cp_h+cp_l = 2/(3*log2) */
		z_l = cp_l*p_h+p_l*cp+dp_l[k];
		/* log2(ax) = (ss+..)*2/(3*log2) = n + dp_h + z_h + z_l */
		t = (double)n;
		t1 = (((z_h+z_l)+dp_h[k])+t);
		__LO(t1) = 0;
		t2 = z_l-(((t1-t)-dp_h[k])-z_h);
	}

	/* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
	y1  = y;
	__LO(y1) = 0;
	p_l = (y-y1)*t1+y*t2;
	p_h = y1*t1;
	z = p_l+p_h;
	j = __HI(z);
	i = __LO(z);
	if (j>=0x40900000) {				/* z >= 1024 */
		if (((j-0x40900000)|i)!=0)			/* if z > 1024 */
			return s*huge*huge;			/* overflow */
		else {
			if (p_l+ovt>z-p_h) return s*huge*huge;	/* overflow */
		}
	} else if ((j&0x7fffffff)>=0x4090cc00 ) {	/* z <= -1075 */
		if (((j-0xc090cc00)|i)!=0) 		/* z < -1075 */
			return s*tiny*tiny;		/* underflow */
		else {
			if (p_l<=z-p_h) return s*tiny*tiny;	/* underflow */
		}
	}
	/*
	 * compute 2**(p_h+p_l)
	 */
	i = j&0x7fffffff;
	k = (i>>20)-0x3ff;
	n = 0;
	if (i>0x3fe00000) {		/* if |z| > 0.5, set n = [z+0.5] */
		n = j+(0x00100000>>(k+1));
		k = ((n&0x7fffffff)>>20)-0x3ff;	/* new k for n */
		t = zero;
		__HI(t) = (n&~(0x000fffff>>k));
		n = ((n&0x000fffff)|0x00100000)>>(20-k);
		if (j<0) n = -n;
		p_h -= t;
	}
	t = p_l+p_h;
	__LO(t) = 0;
	u = t*lg2_h;
	v = (p_l-(t-p_h))*lg2+t*lg2_l;
	z = u+v;
	w = v-(z-u);
	t  = z*z;
	t1  = z - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	r  = (z*t1)/(t1-two)-(w+z*w);
	z  = one-(r-z);
	j  = __HI(z);
	j += (n<<20);
	if ((j>>20)<=0) z = scalbn(z,n);	/* subnormal output */
	else __HI(z) += (n<<20);
	return s*z;
}

#pragma GCC diagnostic pop

double pow(double x, double y)
{
	return __ieee754_pow(x, y);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
double log1p(double x)
{
	return 0;
}

double exp(double x)
{
	return 0;
}

double hypot(double x, double y)
{
	return 0;
}
#pragma GCC diagnostic pop

void __assert_fail(char *assertion, char *file, int line, char *func)
{
	fprintf(stdout, "assert (%s) failed in %s at %s:%d\n", 
			assertion, func, file, line);
	abort();
}

uint32_t htonl(uint32_t hostlong)
{
	unsigned char data[4] = {0};
	uint32_t ret;

	data[0] = hostlong >> 24;
	data[1] = hostlong >> 16;
	data[2] = hostlong >> 8;
	data[3] = hostlong;

	memcpy(&ret, &data, sizeof(data));

	return ret;
}

uint16_t htons(uint16_t hostshort)
{
	unsigned char data[2] = {0};
	uint16_t ret;

	data[0] = hostshort >> 8;
	data[1] = hostshort;

	memcpy(&ret, &data, sizeof(data));

	return ret;
}

uint32_t ntohl(uint32_t net)
{
	return ((net & 0x000000ff) << 24)
		|  ((net & 0x0000ff00) << 8)
		|  ((net & 0x00ff0000) >> 8)
		|  ((net & 0xff000000) >> 24);
	/*
	   unsigned char data[4] = {};
	   memcpy(&data, &net, sizeof(data));

	   return ((uint32_t) data[3] << 0)
	   | ((uint32_t) data[2] << 8)
	   | ((uint32_t) data[1] << 16)
	   | ((uint32_t) data[0] << 24);*/
}

uint16_t ntohs(uint16_t net)
{
	return ((net & 0x00ff) << 8)
		|  ((net & 0xff00) >> 8);
}

int brk(void *addr)
{
	void *newbrk = (void *)syscall(__NR_brk, (long)addr);
	if (newbrk == NULL || newbrk == _data_end || newbrk != addr) {
		errno = ENOMEM;
		return -1;
	}
	_data_end = newbrk;
	return 0;
}

void *sbrk(intptr_t increment)
{
	void *ret = _data_end;

	if (increment == 0)
		return _data_end;
	if (brk((char *)_data_end + increment))
		return NULL;
	return ret;
}

char *optarg = NULL;
int opterr, optind = 1, optopt;

int getopt(int argc, char *const argv[], const char *optstring)
{
	if (optind >= argc) return -1;
	if (optind == 0) return -1;

	if (argv[optind] == NULL) return -1;
	if (*argv[optind] != '-') return -1;
	if (!strcmp(argv[optind], "-")) return -1;

	if (!strcmp(argv[optind], "--")) {
		optind++;
		return -1;
	}

	char opt = argv[optind][1];

	if (opt == 0)
		return -1;

	char *match;
	char ret;

	if ((match = strchr(optstring, opt)) == NULL) {
		ret = '?';
		goto done;
	}

	ret = *match;
	if (*(optstring+1) == ':') {
		// TODO
	}

done:
	optind++;
	return ret;
}

char *basename(char *path)
{
	int i = strlen(path) - 1;

	while (i > 0)
	{
		if (path[i-1] == '/')
			return path + i;
		i--;
	}

	return path;
}

char *dirname(char *path)
{
	int i = strlen(path) - 1;
	while (i > 0)
	{
		if (path[i] == '/') {
			path[i] = '\0';
			return path;
		}
		i--;
	}

	return ".";
}

int ioctl(int fd, int request, ...)
{
	int ret;
	void *arg;

	va_list ap;
	va_start(ap, request);
	arg = va_arg(ap, void *);
	va_end(ap);

	ret = (int)syscall(__NR_ioctl, (long)fd, (long)request, (long)arg, 0, 0, 0, 0, 0);

	return ret;
}

typedef union {
  float f;
  struct {
    unsigned int mantisa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } parts;
} float_t;

typedef union {
  double d;
  struct {
    unsigned long mantisa : 52;
    unsigned int exponent : 11;
    unsigned int sign : 1;
  } parts;
} double_t;

typedef union {
  long double ld;
  struct {
    unsigned long mantisa : 63;
	unsigned int integer : 1;
    unsigned int exponent : 15;
    unsigned int sign : 1;
  } parts;
} long_double_t;

double strtod(const char *restrict nptr, char **restrict endptr)
{
	return 0;
}

long strtol(const char *restrict nptr, char **restrict endptr, int base)
{
	long ret = 0;
	long neg = 1;

	if (nptr == NULL || base < 0 || base == 1 || base > 36) {
		errno = EINVAL;
		return 0;
	}

	const char *ptr = nptr;

	while (isspace(*ptr)) ptr++;

	if (*ptr == '-' || *ptr == '+') {
		neg = *ptr == '-' ? -1 : 1;
		ptr++;
	}

	if (base == 0) {
		base = calc_base(&ptr);
		if (base == 0)
			return -1;
	}

	while (*ptr)
	{
		char c = tolower(*ptr);

		if (isdigit(c)) c = c - '0';
		else if (isalpha(c)) c = c - 'a';
		else break;

		ret *= base;
		ret += c;

		ptr++;
	}

	return ret * neg;
}	

unsigned long strtoul(const char *restrict nptr, char **restrict endptr, int base)
{
	long ret = 0;
	//long neg = 1;

	if (nptr == NULL || base < 0 || base == 1 || base > 36) {
		errno = EINVAL;
		return 0;
	}

	const char *ptr = nptr;

	while (isspace(*ptr)) ptr++;

	if (base == 0) {
		base = calc_base(&ptr);
		if (base == 0)
			return -1;
	}

	while (*ptr)
	{
		char c = tolower(*ptr);

		if (isdigit(c)) 
			c = c - '0';
		else if (isalpha(c)) 
			c = c - 'a';
		else 
			break;

		ret *= base;
		ret += c;

		ptr++;
	}

	return ret;// * neg;
}

long long strtoll(const char *restrict nptr, char **restrict endptr, int base)
{
	return strtol(nptr, endptr, base);
}

unsigned long long strtoull(const char *restrict nptr, char **restrict endptr, int base)
{
	return strtoul(nptr, endptr, base);
}

int atoi(const char *nptr)
{
	return(strtol(nptr, NULL, 10));
}

long atol(const char *nptr)
{
	return(strtol(nptr, NULL, 10));
}

long long atoll(const char *nptr)
{
	return strtoll(nptr, NULL, 10);
}

__attribute__((nonnull(1), access(read_only, 1), access(write_only, 2)))
static int findenv(const char *name, size_t *nlen)
{
	int i;
	size_t len;

	if (environ == NULL)
		return -1;

	len = strlen(name);
	if (nlen)
		*nlen = len;

	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], name, len)) 
			continue;

		if (environ[i][len] != '=') 
			continue;

		return i;
	}

	return -1;
}

char *getenv(const char *name)
{
	int i;
	size_t len;

	if (name == NULL) {
		errno = EINVAL;
		return NULL;
	}

	if ((i = findenv(name, &len)) == -1)
		return NULL;

	return (environ[i] + len + 1);
}

inline static int environ_size(void)
{
	if (environ == NULL)
		return 0;

	int ret = 0;

	while (environ[ret]) 
		ret++;
	return ret;
}

int setenv(const char *name, const char *value, int overwrite)
{
	if (name == NULL) {
		errno = EINVAL;
		return -1;
	}
	
	int i, es;
	char *new, **tmp;
	size_t len;

	if ((i = findenv(name, &len)) == -1) {
		if ((tmp = realloc(environ, ((es = environ_size()) + 2) * sizeof(char *))) == NULL)
			return -1;
		environ = tmp;
		i = es;
		environ[i]   = NULL;
		environ[i+1] = NULL;
		goto set;
	} else {
set:
		if (overwrite == 0)
			return 0;
		len = len + ((value != NULL) ? strlen(value) : 0) + 2;
		if ((new = calloc(1, len)) == NULL)
			return -1;
		snprintf(new, len, "%s=%s", name, value ? value : "");
		environ[i] = new;
	}

	return 0;
}

int unsetenv(const char *name)
{
	if (name == NULL) {
		errno = EINVAL;
		return -1;
	}

	int i,j;

	if ((i = findenv(name, NULL)) == -1)
		return 0;

	for (j = i + 1; environ[j]; i++, j++)
		environ[i] = environ[j];

	environ[i] = NULL;

	char **tmp;
	if ((tmp = realloc(environ, (environ_size() + 2) * sizeof(char *))) == NULL)
		return -1;

	environ = tmp;
	return 0;
}

int chdir(const char *path)
{
	return syscall(__NR_chdir, path);
}

mode_t umask(mode_t mask)
{
	return syscall(__NR_umask, mask);
}

char *getcwd(char *buf, size_t size)
{
	return (char *)syscall(__NR_getcwd, buf, size);
}

int putenv(char *string)
{
	if (string == NULL)
		return -1;

	char *eq, **tmp;
	size_t len, i;

	if ((eq = strchr(string, '=')) == NULL)
		return -1;

	len = eq - string;

	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], string, len))
			continue;

		if (environ[i][len] != '=')
			continue;

		environ[i] = string;
		return 0;
	}

	if ((tmp = realloc(environ, (i+2) * sizeof(char *))) == NULL)
		return -1;

	environ = tmp;
	environ[i] = string;
	environ[i+1] = NULL;

	return 0;
}

void clearerr(FILE *fp)
{
	fp->eof = false;
	fp->error = 0;
}

void rewind(FILE *fp)
{
	fseek(fp, 0L, SEEK_SET);
	clearerr(fp);
}

int fseek(FILE *fp, long offset, int whence)
{
    if (fp->mem) {
        off_t newoff;

        switch(whence)
        {
            case SEEK_SET:
                newoff = offset;
                break;
            case SEEK_CUR:
                newoff = fp->offset + offset;
                break;
            case SEEK_END:
                newoff = fp->mem_size;
                break;
            default:
                errno = EINVAL;
                return -1;
        }

        if (newoff < 0L || (size_t)newoff > fp->mem_size) {
            errno = EINVAL;
            return -1;
        }

        fp->offset = newoff;
        return 0;
    }

    /* real file */

	off_t rc;

	if ((rc = lseek(fp->fd, offset, whence)) == -1) {
		fp->error = errno;
		return -1;
	}

	fp->offset = rc;
	return 0;
}

static FILE *utx  = NULL;
static int utx_rw = 0;

static int try_open_utx()
{
	if (utx != NULL) {
		if (ferror(utx)) {
			fclose(utx);
			utx = NULL;
			goto try;
		}

		return 0;
	}

try:
	if ((utx = fopen("/run/utmp", "r")) == NULL)
		return -1;

	utx_rw = 0;

	return 0;
}

void setutxent()
{
	if (try_open_utx() == -1)
		return;

	rewind(utx);
}

void endutxent()
{
	if (utx) {
		fclose(utx);
		utx = NULL;
		utx_rw = 0;
	}
}

static struct utmpx utmpx_tmp;

struct utmpx *getutxent()
{
	if (try_open_utx() == -1)
		return NULL;


	if (fread(&utmpx_tmp, 1, sizeof(utmpx_tmp), utx) != sizeof(utmpx_tmp))
		return NULL;

	return &utmpx_tmp;
}

static struct tm localtime_tmp;
static struct tm gmtime_tmp;
static char asctime_tmp[27];

char *asctime(const struct tm *tm)
{
	if (tm->tm_wday > 6 || tm->tm_wday < 0 || tm->tm_mon > 11 || tm->tm_mon < 0)
		return NULL;

	snprintf(asctime_tmp, 
			sizeof(asctime_tmp), 
			"%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
			wday_name[tm->tm_wday],
			mon_name[tm->tm_mon],
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec,
			1900 + tm->tm_year);

	return asctime_tmp;
}

/* Unix time: number of seconds since 1970-01-01T00:00:00Z */

struct tm *gmtime(const time_t *const now)
{
	long secs, days, years, hours, mins, rem_secs;
	long yday, mday = 0;

	if (*now > UINT_MAX || *now < 0) {
		errno = EOVERFLOW;
		return NULL;
	}

	secs = *now;

	/* figure out the values not impacted by the year */
	days = secs / 86400;
	rem_secs = secs % 86400;

	hours = rem_secs / 3600;
	rem_secs = rem_secs % 3600;

	mins = rem_secs / 60;
	rem_secs = rem_secs % 60;

	/* initial view of the year ignoring leap days */
	years = days / 365;
	yday = days - (years * 365);
	years += 1970;

	/* correct for additional leap days */
	yday -= (years-1)/4   - (1970-1)/4;
	yday += (years-1)/100 - (1970-1)/100;
	yday -= (years-1)/400 - (1970-1)/400;

	long cnt = 0, month = 0;
	bool leap;

	/* handle the case we've overshot the year */
	if (yday >= 0) {
		leap = (!(years % 4) || !(years % 100)) && (years % 400);
	} else while (yday < 0) {
		years--;
		/* as the year has changed, recalculate if it is a leap year */
		leap = (!(years % 4) || !(years % 100)) && (years % 400);

		yday = 365 + yday + (leap ? 1 : 0);
	}

	/* this is here due to February */
	const long d_in_m[12] = {31,leap ? 29 : 28,31,30,31,30,31,31,30,31,30,31};

	/* figure out which calendar month we're in */
	for (long i = 0; i < 12; i++, month++ ) {
		if (cnt + d_in_m[i] > yday) break;
		cnt += d_in_m[i];
	}

	/* now figure out the day of the month as the 'remainder' */
	mday = yday - cnt + 1;

	/* populate structure to return */
	gmtime_tmp.tm_yday  = yday;
	gmtime_tmp.tm_mday  = mday;
	gmtime_tmp.tm_mon   = month;
	gmtime_tmp.tm_hour  = hours;
	gmtime_tmp.tm_min   = mins;
	gmtime_tmp.tm_year  = years - 1900;
	gmtime_tmp.tm_sec   = rem_secs;
	gmtime_tmp.tm_isdst = 0;

	/* done! */
	return &gmtime_tmp;
}

struct tm *localtime(const time_t *now)
{
	struct tm *gmt = gmtime(now);

	memcpy(&localtime_tmp, gmt, sizeof(struct tm));

	return &localtime_tmp;
}

unsigned int sleep(unsigned seconds)
{
	/* TODO */

	struct timespec rem, req = {
		.tv_sec  = seconds,
		.tv_nsec = 0
	};

	if (nanosleep(&req, &rem) == 0)
		return 0;

	return rem.tv_sec;
}

int setpgid(pid_t pid, pid_t pgid)
{
	return syscall(__NR_setpgid, pid, pgid);
}

pid_t setpgrp(void)
{
	setpgid(0, 0);
	return getpgrp();
}

pid_t getpgrp(void)
{
	return syscall(__NR_getpgrp);
}

int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact)
{
	return syscall(__NR_sigaction, sig, act, oact);
}

__sighandler_t signal(int num, __sighandler_t func)
{
	struct sigaction osa, sa = {
		.sa_handler = func,
		.sa_flags = 0,
		.sa_mask = 0,
		.sa_sigaction = NULL
	};

	if (sigaction(num, &sa, &osa) == -1)
		return NULL;

	return osa.sa_handler;
}

static const char *regerrors[] = {
    [REG_SUCCESS]  = "REG_SUCCESS",
    [REG_NOMATCH]  = "REG_NOMATCH",
    [REG_BADPAT]   = "REG_BADPAT",
    [REG_ECOLLATE] = "REG_ECOLLATE",
    [REG_ECTYPE]   = "REG_ECTYPE",
    [REG_EESCAPE]  = "REG_EESCAPE",
    [REG_ESUBREG]  = "REG_ESUBREG",
    [REG_EBRACK]   = "REG_EBRACK",
    [REG_EPAREN]   = "REG_EPAREN",
    [REG_EBRACE]   = "REG_EBRACE",
    [REG_BADBR]    = "REG_BADBR",
    [REG_ERANGE]   = "REG_ERANGE",
    [REG_ESPACE]   = "REG_ESPACE",
    [REG_BADRPT]   = "REG_BADRPT",
    NULL
};
static const int num_regerrors = sizeof(regerrors)/sizeof(const char *);

size_t regerror(int errcode, const regex_t *restrict preg __attribute__((unused)), char *restrict errbuf, size_t size)
{
    int ret;

    if (errcode < 0 || errcode > num_regerrors)
        return 0;

    ret = regerrors[errcode] ? strlen(regerrors[errcode]) : 0;

    if (size && errbuf && ret)
        snprintf(errbuf, size, "%s", regerrors[errcode]);

    return ret;
}

pid_t gettid(void)
{
	return syscall(__NR_gettid);
}

/* Linux specific */
int arch_prctl(int code, unsigned long addr)
{
	return syscall(__NR_arch_prctl, code, addr);
}

int sigsuspend(const sigset_t *mask)
{
    return syscall(__NR_sigsuspend, mask, (size_t)sizeof(sigset_t));
}

int sigpending(sigset_t *set)
{
    return syscall(__NR_sigpending, set, (size_t)sizeof(sigset_t));
}

int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *timeout)
{
    return syscall(__NR_sigtimedwait, set, info, timeout, (size_t)sizeof(sigset_t));
}

int sigwait(const sigset_t *set, int *sig)
{
    int rc;
    rc = sigtimedwait(set, NULL, NULL);

    if (rc >= 0) {
        if (sig)
            *sig = rc;

        return 0;
    }

    return errno;
}

int sigwaitinfo(const sigset_t *set, siginfo_t *info)
{
    return sigtimedwait(set, info, NULL);
}

int sigqueue(pid_t pid, int sig, const union sigval value)
{
    siginfo_t uinfo = {
        .si_signo = sig,
        .si_pid   = getpid(),
        .si_uid   = getuid(),
        .si_value = value,
        .si_code  = SI_QUEUE
    };

    return syscall(__NR_sigqueueinfo, pid, sig, &uinfo);
}

size_t mbstowcs(wchar_t *restrict pwcs, const char *restrict s, size_t n)
{
    /* TODO */
    return (size_t)-1;
}

/* End of public library routines */

inline static int calc_base(const char **ptr)
{
	int base = 0;

	if (!**ptr) 
		return 0;

	if (**ptr == '0' && isdigit(*(*ptr)+1)) {
		base = 8; 
		(*ptr)++;
	} else if (**ptr == '0' && tolower(*(*ptr)+1) == 'x') {
		base = 16; 
		(*ptr) += 2;
	} else 
		base = 10;

	return base;
}

#define SBRK_GROW_SIZE	(1<<21)

inline static void init_mem()
{
	const size_t len = SBRK_GROW_SIZE;

	if ( (tmp_first = first = last = sbrk(len)) == NULL ) {
		printf("init_mem: unable to sbrk(%d)\n", len);
		_exit(2);
	}

	first->next = NULL;
	first->prev = NULL;
	first->start = first;
	first->end = ((char *)first->start) + len;
	first->flags |= MF_FREE;
	first->len = len;
	first->magic = MEM_MAGIC;
}


inline static void mem_compress()
{
	struct mem_alloc *buf, *next;

	for (buf = first; buf; buf = buf->next)
	{
		if (!(buf->flags & MF_FREE))
			continue;

		next = buf->next;

		if (next && (next->flags & MF_FREE)) {

			if (next == tmp_first)
				tmp_first = buf;

			buf->len  = (buf->len + next->len);
			buf->end  = next->end;
			buf->next = next->next;

			if (next->next)
				next->next->prev = buf;

			memset(next, 0, sizeof(struct mem_alloc));

			if (buf->next == NULL)
				last = buf;
		}

	}
}


inline static void free_alloc(struct mem_alloc *tmp)
{
	//struct mem_alloc *buf = tmp;
	//static int cnt = 0;

	if (tmp) {
		tmp->flags |= MF_FREE;
		if (tmp < tmp_first)
			tmp_first = tmp;
	}
	//printf("free_alloc: [%d] @ %p\n", tmp->len, tmp);
	//if (!(cnt++ % 256)) mem_compress();

	//return;
}

inline static struct mem_alloc *grow_pool()
{
	mem_compress();

	const size_t len = SBRK_GROW_SIZE;
	struct mem_alloc *new_last;
	struct mem_alloc *old_last = last;

	if ((new_last = sbrk(len)) == NULL)
		exit(3);

	if ((last->flags & MF_FREE)) {
		last->end = (char *)last->end + len;
		last->len += len;
	} else {
		old_last->next = new_last;

		new_last->prev = old_last;
		new_last->next = NULL;
		new_last->len = len;
		new_last->magic = MEM_MAGIC;
		new_last->flags |= MF_FREE;
		new_last->start = new_last;

		new_last->end = (char *)new_last->start + len;

		last = new_last;
	}
	return last;
}

static void check_mem()
{
	if (first == NULL) {
		warnx("check_mem: first is null");
		_exit(1);
	}
	if (last == NULL) {
		warnx("last is null");
		_exit(1);
	}

	struct mem_alloc *tmp, *prev;

	for (tmp = first, prev = NULL; tmp; prev = tmp, tmp = tmp->next)
	{
		if (tmp < first || tmp > last) {
			warnx("check_mem: %p out of range [prev=%p]", tmp, prev);
			_exit(1);
		}
		if (tmp->magic != MEM_MAGIC) {
			warnx("check_mem: %p has bad magic [prev=%p]", tmp, prev);
            return;
			_exit(1);
		}
		if (tmp->next == tmp || tmp->prev == tmp) {
			warnx("check_mem: %p circular {<%p,%p>}", tmp, tmp->prev, tmp->next);
			_exit(1);
		}
	}
}

inline static struct mem_alloc *find_free(const size_t size)
{
	register struct mem_alloc *restrict tmp;
	register const size_t seek = size + (sizeof(struct mem_alloc) * 2);

	//dump_mem();
	check_mem();
	//printf("find_free:   looking for %ld[%ld]\n", size, seek);

	for (tmp = tmp_first; tmp; tmp = tmp->next)
	{
		/*
		   if (tmp < first || tmp > last)
		   exit(200);
		   if (tmp->next == tmp)
		   exit(201);
		   if (tmp->magic != MEM_MAGIC)
		   exit(202);
		   */

		if ((tmp->flags & MF_FREE) && tmp->len >= seek) {
			return tmp;
		}
	}

	//printf("find_free:   growing pool\n");
	return grow_pool();
}

inline static struct mem_alloc *split_alloc(struct mem_alloc *old, size_t size)
{
	size_t seek;
	struct mem_alloc *rem; 

	if (old == NULL || size == 0)
		return NULL;

	seek = size + (sizeof(struct mem_alloc) * 2);

	if (!size)
		return NULL;
	if (!old || !(old->flags & MF_FREE) || !old->len)
		return NULL;
	if (old->len < seek)
		return NULL;

	if (!old->next && last != old)
		exit(42);
	if (!old->prev && first != old)
		exit(43);
	if (old != old->start)
		exit(40);
	if ((char *)old->start + old->len != old->end)
		exit(41);
	if (old->magic != MEM_MAGIC)
		exit(44);

	//printf("split_alloc: old=%p[%ld@%p] {<%p,%p>} size=%ld\n", old, old->len, old->start, old->prev, old->next, size);
	//printf("split_alloc: %p + %ld + %ld\n", old->start, sizeof(struct mem_alloc), size);

	rem = (struct mem_alloc *)(((char *)old->start) + sizeof(struct mem_alloc) + size);
	if (rem == NULL || (void *)rem < _data_start || (void *)rem > _data_end) {
		printf("mem_alloc: corruption\n");
		abort();
	}
	//printf("split_alloc: rem=%p\n", rem);
	rem->magic = MEM_MAGIC;

	if (old->next)
		old->next->prev = rem;

	rem->prev = old;
	rem->next = old->next;
	old->next = rem;

	rem->flags |= MF_FREE;
	old->flags = ~(MF_FREE);

	rem->len = old->len - (sizeof(struct mem_alloc) + size);
	rem->start = rem;
	rem->end = (char *)rem->start + rem->len;

	old->len = sizeof(struct mem_alloc) + size;
	old->end = ((char *)old->start) + old->len;

	if (old->prev == NULL)
		first = old;
	if (rem->next == NULL)
		last = rem;

	//printf("split_alloc: old=%p[%ld] rem=%p[%ld]\n", old, old->len, rem, rem->len);

	return old;
}

inline static struct mem_alloc *alloc_mem(size_t size)
{
	static int cnt = 0;
	struct mem_alloc *ret;

	if (cnt++ % 64 == 0)
		mem_compress();

	if (size <= 0)
		return NULL;

	if ((ret = find_free(size)) == NULL)
		return NULL;

	if ((split_alloc(ret, size)) == NULL)
		return NULL;

	tmp_first = ret->next;

	//printf("ret=%p ret->next=%p ret->next->next:%p\n", ret, ret->next, ret->next->next);

	return ret;
}

inline static struct __pthread *__pthread_self(void)
{
	struct __pthread *ret = NULL;
	__asm__("movq %%fs:0, %0":"=r" (ret));
	return ret;
}

__attribute__((nonnull, access(write_only, 1)))
static char *fgets_delim(char *const restrict s, const int size, FILE *const restrict stream, const int delim)
{
	if (feof(stream) || ferror(stream))
		return NULL;

	int len = 0;
	char in;

	while (len < (size - 1))
	{
		if ((in = fgetc(stream)) == EOF)
			break;

		if ((s[len++] = in) == delim) {
			s[len] = '\0';
			break;
		}
	}
	if (len == 0 || ferror(stream) )
		return NULL;
	return s;
}


/* Special functions */

int *__errno_location(void)
{
	return &__pthread_self()->errnum;
}

__attribute__((constructor))
void init(void)
{
}

__attribute__((destructor))
void fini(void)
{
}

typedef struct {
	long a_type;
	union {
		long a_val;
		void *a_ptr;
		void (*a_fnc)();
	} a_un;
} __attribute__((packed)) auxv_t;

/*
AT_NULL 0 ignored
AT_IGNORE 1 ignored
AT_EXECFD 2 a_val
AT_PHDR 3 a_ptr
AT_PHENT 4 a_val
AT_PHNUM 5 a_val
AT_PAGESZ 6 a_val
AT_BASE 7 a_ptr
AT_FLAGS 8 a_val
AT_ENTRY 9 a_ptr
AT_NOTELF 10 a_val
AT_UID 11 a_val
AT_EUID 12 a_val
AT_GID 13 a_val
AT_EGID 14 a_val
*/
#define AT_NULL 0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_PAGESZ 6
#define AT_BASE 7
#define AT_FLAGS 8
#define AT_ENTRY 9
#define AT_NOTELF 10
#define AT_UID 11
#define AT_EUID 12
#define AT_GID 13
#define AT_EGID 14

/* Linux extensions? */
#define AT_PLATFORM 15
#define AT_HWCAP 16
#define AT_CLKTCK 17
#define AT_SECURE 23
#define AT_BASE_PLATFORM 24
#define AT_RANDOM 25
#define AT_HWCAP2 26
#define AT_EXECFN 31
#define AT_SYSINFO_EHDR 33

__attribute__((unused))
static void debug_aux(const auxv_t *aux)
{
		switch (aux->a_type)
		{
			case AT_IGNORE:
				printf("AT_IGNORE\n");
				break;
			case AT_EXECFD:
				printf("AT_EXECFD: %ld\n", aux->a_un.a_val);
				break;
			case AT_PAGESZ:
				printf("AT_PAGESZ: %ld\n", aux->a_un.a_val);
				break;
			case AT_PHDR:
				printf("AT_PHDR:   0x%p\n", aux->a_un.a_ptr);
				break;
			case AT_PHENT:
				printf("AT_PHENT:  %ld\n", aux->a_un.a_val);
				break;
			case AT_PHNUM:
				printf("AT_PHNUM:  %ld\n", aux->a_un.a_val);
				break;
			case AT_BASE:
				printf("AT_BASE:   0x%p\n", aux->a_un.a_ptr);
				break;
			case AT_FLAGS:
				printf("AT_FLAGS:  %ld\n", aux->a_un.a_val);
				break;
			case AT_ENTRY:
				printf("AT_ENTRY:  0x%p\n", aux->a_un.a_ptr);
				break;
			case AT_NOTELF:
				printf("AT_NOTELF: %ld\n", aux->a_un.a_val);
				break;
			case AT_UID:
				printf("AT_UID:    %ld\n", aux->a_un.a_val);
				break;
			case AT_EUID:
				printf("AT_EUID:   %ld\n", aux->a_un.a_val);
				break;
			case AT_GID:
				printf("AT_GID:    %ld\n", aux->a_un.a_val);
				break;
			case AT_EGID:
				printf("AT_EGID:   %ld\n", aux->a_un.a_val);
				break;

			/* Linux specific outside of ABI specification */
			case AT_SECURE:
				printf("AT_SECURE: %ld\n", aux->a_un.a_val);
				break;
			case AT_PLATFORM:
				printf("AT_PLTFRM: %s\n", aux->a_un.a_ptr);
				break;
			case AT_EXECFN:
				printf("AT_EXECFN: %s\n", aux->a_un.a_ptr);
				break;
			case AT_HWCAP:
				printf("AT_HWCAP:  %lu\n", aux->a_un.a_val);
				break;
			case AT_HWCAP2:
				printf("AT_HWCAP2: %lu\n", aux->a_un.a_val);
				break;
			case AT_CLKTCK:
				printf("AT_CLKTCK: %ld\n", aux->a_un.a_val);
				break;
			case AT_RANDOM:
				printf("AT_RANDOM: 0x%p\n", aux->a_un.a_ptr);
				break;

			/* Linux x86_64 specific outside of ABI specification */
			case AT_SYSINFO_EHDR:
				printf("AT_SYSINF: 0x%p\n", aux->a_un.a_ptr);
				break;

			default:
				printf("Unknown:   %d.0x%lx\n", aux->a_type, aux->a_un.a_val);
				break;
		}
}

__attribute__((noreturn))
void __libc_start_main(int ac, char *av[], char **envp, auxv_t *aux)
{
	struct __pthread tmp = {
		.errnum = 0,
		.parent_tid = 0,
		.self = &tmp
	};

	syscall(__NR_arch_prctl, ARCH_SET_FS, (uint64_t)&tmp);

	if (__pthread_self() != &tmp)
		_exit(1);

	_data_start = _data_end = (void *)syscall(__NR_brk, 0);
	if (_data_end == (void *)-1UL)
		_Exit(EXIT_FAILURE);

	global_atexit_list = NULL;

	first = NULL;
	last  = NULL;

	init_mem();

	int i = 0;
	while (aux[i].a_type)
	{
		/* TODO process AUX here */
		
		i++;
	}

	struct __pthread *npt = malloc(sizeof(struct __pthread));

	if (npt == NULL) 
		_Exit(EXIT_FAILURE);

	npt->parent_tid = 0;
	npt->self = npt;
	npt->errnum = 0;

	arch_prctl(ARCH_SET_FS, (uint64_t)npt);

	npt->my_tid = gettid();
	environ = envp;

	check_mem();

	exit(main(ac, av, envp));
}
