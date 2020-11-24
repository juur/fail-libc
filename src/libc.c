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

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;    
};

struct atexit_fun {
	struct atexit_fun *next;
	void (*function)(void);
};

#define MEM_MAGIC	0x61666c69

struct mem_alloc {
	struct mem_alloc *next;
	struct mem_alloc *prev;
	unsigned is_free : 1;
	size_t len;
	uint32_t magic;
	void *start;
	void *end;
};

/* global variables */

char **environ;

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
 * %rdi    - number  -> %rax
 * %rsi    - 1st arg -> %rdi
 * %rdx    - 2nd arg -> %rsi
 * %rcx    - 3rd arg -> %rdx
 * %r8     - 4th arg -> %r10 (why?)
 * %r9     - 5th arg -> %r8
 * 8(%rsp) - 6th arg -> %r9
 */
extern long syscall(long number, long arg0, long arg1, long arg2, long arg3,
		long arg4, long arg5, long arg6);
extern int main(int, char *[], char *[]);

/* library declarations */

pid_t gettid(void);
int *__errno_location (void);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t strlen(const char *s);
FILE *fdopen(int fd, const char *mode);
void *sbrk(intptr_t increment);
void exit_group(int) __attribute__ ((noreturn));
void _exit(int) __attribute__ ((noreturn));
void exit(int) __attribute__ ((noreturn));
void abort() __attribute__ ((noreturn));
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocarray(void *ptr, size_t nmemb, size_t size);
void *memset(void *s, int c, size_t n);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vdprintf(int fd, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
void *memcpy(void *dest, const void *src, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
int isalnum(int c);
int isalpha(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int isascii(int c);
int isblank(int c);
int execve(const char *path, char *const argv[], char *const envp[]);

/* local declarations */

static struct mem_alloc *alloc_mem(size_t size);
static void free_alloc(struct mem_alloc *buf);
void check_mem();
static struct __pthread *__pthread_self(void);
static void sys_exit(int) __attribute__ ((noreturn));

/* local variables */

static struct mem_alloc *first;
static struct mem_alloc *last;
static void *_data_end;
static struct atexit_fun *global_atexit_list;

#if 0
static void dump_one_mem(const struct mem_alloc *const mem)
{
	printf("mem @ %p [prev=%p,next=%p,free=%d,len=%d",
			mem,
			mem->prev, mem->next,
			mem->is_free,
			(int)mem->len);
}

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
#endif

int execve(const char *path, char *const argv[], char *const envp[])
{
	if (path == NULL || argv == NULL || envp == NULL) {
		errno = EINVAL;
		return -1;
	}
	return syscall(__NR_execve, (uint64_t)path, (uint64_t)argv, (uint64_t)envp, 0, 0, 0, 0);
}

void exit_group(int status) 
{
	for (;;) syscall(__NR_exit_group, status, 0, 0, 0, 0, 0, 0);
}

char *strcpy(char *dest, const char *src)
{
	size_t i;
	for (i = 0; src[i]; i++)
		dest[i] = src[i];
	dest[i] = '\0';

	return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	size_t i;
	for (i = 0; src[i] && i < n; i++)
		dest[i] = src[i];
	dest[i] = '\0';

	return dest;
}

static void sys_exit(int status)
{
	syscall(__NR_exit, status, 0, 0, 0, 0, 0, 0);
	for(;;) ;
}

void _exit(int status)
{
	exit_group(status);
}

void exit(int status)
{
	check_mem();
	//dump_mem();
	_exit(status);
}

ssize_t write(int fd, const void *buf, size_t count)
{
	return syscall(__NR_write, fd, (long)buf, count, 0, 0, 0, 0);
}

ssize_t read(int fd, void *buf, size_t count)
{
	return syscall(__NR_read, fd, (long)buf, count, 0, 0, 0, 0);
}

int open(const char *pathname, int flags, ...)
{
	mode_t mode = 0;
	return syscall(__NR_open, (long)pathname, flags, mode, 0 ,0 ,0 ,0);
}

int access(const char *pathname, int mode)
{
	return syscall(__NR_access, (long)pathname, mode, 0, 0, 0, 0, 0);
}

int close(int fd)
{
	return syscall(__NR_close, fd, 0 ,0 ,0 ,0 ,0, 0);
}

int stat(const char *pathname, struct stat *statbuf)
{
	errno = ENOMEM;
	return -1;
}

int fclose(FILE *stream)
{
	if (!stream)
		return 0;
	int ret = close(stream->fd);
	free(stream);
	return ret;
}

static void itoa(char *buf, int base, unsigned long d, bool pad, int size)
{
    char *p = buf, *p1, *p2;
    unsigned long ud = d;
    unsigned long divisor = 10;
    unsigned long remainder;

    if(base=='d' && (long)d < 0)
    {
        *p++ = '-';
        buf++;
        ud = -d;
    } else if(base=='x') {
        divisor = 16;
    }

    do {
        remainder = ud % divisor;
        *p++ = (char)((remainder < 10) ? remainder + '0' : remainder + 'a' - 10);
    } while (ud /= divisor);

    *p = 0;

    p1 = buf;
    p2 = p - 1;

    while(p1<p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

#define _INT	4
#define _SHORT	2
#define _LONG	8

int vfprintf(FILE *stream, const char *format, va_list ap)
{
	char c;
    char *p;
    char buf[64],buf2[64];
    int len = _INT;
    bool pad = false;
    int i,l;
	int wrote = 0;

	if (!stream)
		return 0;

    memset(buf2, '0', 63);
    memset(buf, '0', 63);

    while ((c = *format++) != 0)
    {
        if ( c!= '%' ) {
            putc(c, stream);
			wrote++;
        } else {
next:
            c = *format++;
            p = buf;
			if (isdigit((unsigned char)c))
				goto next;
            switch(c)
            {
				case '-':
					goto next;
                case 'p':
                    len = _LONG;
                    c = 'x';
                    goto forcex;
                case '0':
                    pad = true;
                    goto next;
                case 'h':
                    len = _SHORT;
                    goto next;
                case 'l':
                    len = _LONG;
                    goto next;
                case 'u':
                case 'x':
forcex:
                    switch(len) {
                        case _SHORT:
                            itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), pad, len);
                            break;
                        case _INT:
                            itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), pad, len);
                            break;
                        case _LONG:
                            itoa(buf,c,(unsigned long)va_arg(ap, unsigned long), pad, len);
                            break;
                    }
                    goto padcheck;
                case 'd':
                    switch(len) {
                        case _SHORT:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), pad, len);
                            break;
                        case _INT:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), pad, len);
                            break;
                        case _LONG:
                            itoa(buf,c,(unsigned long)va_arg(ap, long), pad, len);
                            break;
                    }

padcheck:
                    if(pad)
                        for(i=0,l=(len<<2)-strlen(buf) ; l && i < l ; i++)
							putc('0', stream);
                    len = _INT;
                    pad = false;
                    goto string;
                case 's':
                    p = va_arg(ap, char *);
string:
                    if(!p) fputs("(null)", stream);
					else fputs(p, stream);
                    break;
                case 'c':
                    c = va_arg(ap, int);
                    if(c>=' ' && c<='~')
						putc(c, stream);
                    break;
            }
        }
    }

	return wrote;
}

int snprintf(char *str, size_t size, const char *format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsnprintf(str, size, format, ap);
	va_end(ap);
	return ret;
}

int fprintf(FILE *stream, const char *format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vfprintf(stream, format, ap);
	va_end(ap);
	return ret;
}

int printf(const char *format, ...)
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

int vsnprintf(char *dst, size_t size, const char *format, va_list ap)
{
	char c;
    char *p;
    char buf[64],buf2[64];
    int len = _INT;
    bool pad = false;
    int i,l;
	size_t off = 0;

    memset(buf2, '0', 63);
    memset(buf, '0', 63);

    while ((c = *format++) != 0 && off < size)
    {
		dst[off] = '\0';
        if ( c!= '%' ) {
            dst[off++] = c;
        } else {
next:
            c = *format++;
            p = buf;
            switch(c)
            {
                case 'p':
                    len = _LONG;
                    c = 'x';
                    goto forcex;
                case '0':
                    pad = true;
                    goto next;
                case 'h':
                    len = _SHORT;
                    goto next;
                case 'l':
                    len = _LONG;
                    goto next;
                case 'u':
                case 'x':
forcex:
                    switch(len) {
                        case _SHORT:
                            itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), pad, len);
                            break;
                        case _INT:
                            itoa(buf,c,(unsigned long)va_arg(ap, unsigned int), pad, len);
                            break;
                        case _LONG:
                            itoa(buf,c,(unsigned long)va_arg(ap, unsigned long), pad, len);
                            break;
                    }
                    goto padcheck;
                case 'd':
                    switch(len) {
                        case _SHORT:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), pad, len);
                            break;
                        case _INT:
                            itoa(buf,c,(unsigned long)va_arg(ap, int), pad, len);
                            break;
                        case _LONG:
                            itoa(buf,c,(unsigned long)va_arg(ap, long), pad, len);
                            break;
                    }
padcheck:
                    if(pad)
                        for(i=0,l=(len<<2)-strlen(buf) ; l && i < l && off < size; i++)
                            dst[off++] = '0';
                    len = _INT;
                    pad = false;
                    goto string;
                case 's':
                    p = va_arg(ap, char *);
string:
                    if(!p) { strncat(dst+off, "(null)", size-off); off += 6; }
					else { strncat(dst+off, p, size-off); off+= strlen(p);   }
                    break;
                case 'c':
                    c = va_arg(ap, int);
                    if(c>=' ' && c<='~')
                        dst[off++] = c;
                    break;
            }
        }
    }
	if (off == size)
		dst[off-1] = '\0';
	else
		dst[off++] = '\0';

	return off;
}

#undef _LONG
#undef _SHORT
#undef _INT

FILE *fdopen(int fd, const char *mode)
{
	FILE *ret = calloc(1, sizeof(FILE));
	if (ret == NULL) {
		return NULL;
	}
	ret->fd = fd;
	return ret;
}

int isdigit(int c)
{
	unsigned char ch = (unsigned char)c;

	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

FILE *fopen(const char *pathname, const char *modestr)
{
	int flags = 0;
	int mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;

	if      (!strcmp(modestr, "r" )) flags = O_RDONLY;
	else if (!strcmp(modestr, "r+")) flags = O_RDWR;
	else if (!strcmp(modestr, "w" )) flags = O_WRONLY|O_CREAT|O_TRUNC;
	else if (!strcmp(modestr, "w+")) flags = O_RDWR|O_CREAT|O_TRUNC;
	else if (!strcmp(modestr, "a" )) flags = O_WRONLY|O_CREAT|O_APPEND;
	else if (!strcmp(modestr, "a+")) flags = O_RDWR|O_CREAT|O_APPEND;

	int fd = open(pathname, flags, mode);
	if (fd < 0)
		return NULL;

	FILE *ret;

	if ((ret = calloc(1, sizeof(FILE))) == NULL) {
		close(fd);
		return NULL;
	}

	ret->fd = fd;
	return ret;
}

FILE *fmemopen(void *buf, size_t size, const char *mode)
{
	errno = ENOMEM;
	return NULL;
}

size_t strlen(const char *s)
{
	size_t i;
	const char *t = s;
	for (i = 0; t[i]; i++) ;
	return i;
}

int gettimeofday(struct timeval *tv, void *tz)
{
	return syscall(__NR_gettimeofday, (long)tv, (long)tz, 0, 0, 0, 0, 0);
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return syscall(__NR_nanosleep, (long)req, (long)rem, 0, 0, 0, 0, 0);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	const void *tmp_ptr = ptr;
	ssize_t res;

	for (ret = 0; ret < nmemb; ret++)
	{
		if ( (res = write(stream->fd, tmp_ptr, size)) != (ssize_t)size ) {
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

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret;
	void *tmp_ptr = ptr;
	ssize_t res;

	for (ret = 0; ret < nmemb; ret++)
	{
		if ( (res = read(stream->fd, tmp_ptr, size)) != (ssize_t)size ) {
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

int fputs(const char *s, FILE *stream)
{
	return fwrite(s, 1, strlen(s), stream);
}

int puts(const char *s)
{
	return fputs(s, stdout);
}

char *fgets(char *s, int size, FILE *stream)
{
	return NULL; // TODO
}

int fgetc(FILE *stream)
{
	unsigned char ch;
	size_t ret;

	if ( (ret = fread(&ch, 1, 1, stream)) != 1 )
		return EOF;

	return (int)(ch);
}

int getc(FILE *stream)
{
	return fgetc(stream);
}

int getchar(void)
{
	return getc(stdin);
}

char *strchr(const char *s, int c)
{
	const char *ret = s;
	while(*ret)
	{
		if (*ret == c)
			return (char *)ret;
		ret++;
	}
	return NULL;
}

char *strcat(char *dest, const char *src)
{
	size_t dest_len,i;
	dest_len = strlen(dest);

	for (i = 0; src[i] != '\0'; i++)
		dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';

	return dest;
}

char *strncat(char *dest, const char *src, size_t n)
{
	size_t i, dest_len;
	
	dest_len = strlen(dest);

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
		dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';

	return dest;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
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
	size_t i = 0;
	while (true)
	{
		if (s1[i] != s2[i]) return 1;
		if (s1[i] == '\0') break;
		i++;
	}
	return 0;
}

char *strstr(const char *heystack, const char *needle)
{
	const char *ret = heystack;
	size_t len = strlen(needle);

	while(*ret)
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
	struct mem_alloc *buf = ptr - sizeof(struct mem_alloc);
	if (buf < first || buf > last)
		exit(100);
	if (buf->is_free == 1)
		exit(101);

	free_alloc(buf);
}

void *malloc(size_t size)
{
	//printf("*** malloc(%d)\n", size);
	if (size == 0)
		return NULL;

	struct mem_alloc *ret = NULL;
	if ((ret = alloc_mem(size)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	//printf("*** alloc_mem = %p\n", ret);
	return (ret->start + sizeof(struct mem_alloc));
}

void *realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
		return malloc(size);
	void *new = malloc(size);
	memcpy(new, ptr, size);
	return new;
}

void *memset(void *s, int c, size_t n)
{
	for (size_t i = 0; i < n; i++)
		((unsigned char *)s)[i] = (unsigned char)c;
	return s;
}

void *calloc(size_t nmemb, size_t size)
{
	void *ret;
	size_t len = nmemb * size;
	if (len == 0)
		return NULL;
	ret = malloc(len);
	if (ret == NULL)
		return NULL;
	memset(ret, 0, len);

	return ret;
}

int putc(int c, FILE *stream)
{
	unsigned char ch = c;
	return fwrite(&ch, 1, 1, stream);
}

int putchar(int c)
{
	return putc(c, stdout);
}

int atexit(void (*function)(void))
{
	struct atexit_fun *node;
	if ((node = calloc(1, sizeof(struct atexit_fun))) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	node->next = global_atexit_list;
	node->function = function;
	global_atexit_list = node;
	return 0;
}

int kill(pid_t pid, int sig)
{
	return syscall(__NR_kill, pid, sig, 0, 0, 0, 0, 0);
}

pid_t getpid(void)
{
	return syscall(__NR_getpid, 0, 0, 0, 0, 0, 0, 0);
}

int raise(int sig)
{
	return kill(getpid(), sig);
}

void abort(void)
{
	raise(SIGABRT);
	exit(1);
}

char *strerror(int errnum)
{
	switch(errnum)
	{
		case 0:
			return "ENONE";
		case 3:
			return "ESRCH";
		case 12:
			return "ENOMEM";
		case 13:
			return "EACCES";
		default:
			return "EUNKWN";
	}
}

void *memcpy(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++)
		((char *)dest)[i] = ((char *)src)[i];
	return dest;
}

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

	memcpy(ret, s, len);
	return ret;
}

void err(int eval, const char *fmt, ...)
{
	int en = errno;
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
	fprintf(stderr, "%s\n", strerror(en));
	exit(eval);
}

void errx(int eval, const char *fmt, ...)
{
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
	fprintf(stderr, "\n");
	exit(eval);
}

void warn(const char *fmt, ...)
{
	int en = errno;
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
		fprintf(stderr, ": ");
	}
	fprintf(stderr, "%s\n", strerror(en));
}

void warnx(const char *fmt, ...)
{
	if (fmt != NULL) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
	fprintf(stderr, "\n");
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
	return ENOMEM;
}

int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr)
{
	memset(rwlock, 0, sizeof(pthread_rwlock_t));
	return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
	return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
	return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
	return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
	return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
	return 0;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	unsigned long ret = syscall(__NR_mmap, (long)addr, length, prot, flags, fd, offset, 0);
	return (void *)ret;
}

#define STACK_SIZE (1024 * 1024)

int start_thread(int (*fn)(void *), void *arg)
{
	sys_exit(fn(arg));
}

pid_t fork(void)
{
	return (pid_t)syscall(__NR_fork,0,0,0,0,0,0,0);
}

extern int _clone(unsigned long flags, void *stack, void *parent_id, 
		void *child_tid, unsigned long newtls, void *fn, void *arg);

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
		for(;;) ;
	} else
		return child_tid;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
	int clone_flags = CLONE_VM|CLONE_FS|CLONE_FILES
		|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS
		|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID|CLONE_DETACHED;

	struct __pthread *self;
	struct __pthread *new;
	void *stack;

	if ((new = malloc(sizeof(struct __pthread))) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	if ((stack = mmap(NULL, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON|MAP_STACK, -1, 0)) == MAP_FAILED) {
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

	int ret = _clone(
			clone_flags, 
			stack + STACK_SIZE, 
			&new->parent_tid,
			&new->my_tid,
			(unsigned long)new,
			start_routine,
			arg
			);

	if (ret < 0) {
		*thread = NULL;
		return ret;
	}

	*thread = new;
	return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
	return ESRCH;
}

int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr)
{
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

int strerror_r(int errnum, char *buf, size_t buflen)
{
	char *tmp = NULL;

	switch(errnum)
	{
	}

	if (tmp)
		strncpy(buf, tmp, buflen);
	else
		errno = EINVAL;

	return -1;
}

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

double sin(double x)
{
	return 0;
}

double cos(double x)
{
	return 0;
}

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

double log(double x)
{
	return 0;
}

double log10(double x)
{
	return 0;
}

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

double __ieee754_pow(double x, double y)
{
	double z,ax,z_h,z_l,p_h,p_l;
	double y1,t1,t2,r,s,t,u,v,w;
	int i0,i1,i,j,k,yisint,n;
	int hx,hy,ix,iy;
	unsigned lx,ly;

	i0 = ((*(int*)&one)>>29)^1; i1=1-i0;
	hx = __HI(x); lx = __LO(x);
	hy = __HI(y); ly = __LO(y);
	ix = hx&0x7fffffff;  iy = hy&0x7fffffff;

    /* y==zero: x**0 = 1 */
	if((iy|ly)==0) return one;

    /* +-NaN return x+y */
	if(ix > 0x7ff00000 || ((ix==0x7ff00000)&&(lx!=0)) ||
	   iy > 0x7ff00000 || ((iy==0x7ff00000)&&(ly!=0)))
		return x+y;

    /* determine if y is an odd int when x < 0
     * yisint = 0	... y is not an integer
     * yisint = 1	... y is an odd int
     * yisint = 2	... y is an even int
     */
	yisint  = 0;
	if(hx<0) {
	    if(iy>=0x43400000) yisint = 2; /* even integer y */
	    else if(iy>=0x3ff00000) {
		k = (iy>>20)-0x3ff;	   /* exponent */
		if(k>20) {
		    j = ly>>(52-k);
		    if((j<<(52-k))==ly) yisint = 2-(j&1);
		} else if(ly==0) {
		    j = iy>>(20-k);
		    if((j<<(20-k))==iy) yisint = 2-(j&1);
		}
	    }
	}

    /* special value of y */
	if(ly==0) {
	    if (iy==0x7ff00000) {	/* y is +-inf */
	        if(((ix-0x3ff00000)|lx)==0)
		    return  y - y;	/* inf**+-1 is NaN */
	        else if (ix >= 0x3ff00000)/* (|x|>1)**+-inf = inf,0 */
		    return (hy>=0)? y: zero;
	        else			/* (|x|<1)**-,+inf = inf,0 */
		    return (hy<0)?-y: zero;
	    }
	    if(iy==0x3ff00000) {	/* y is  +-1 */
		if(hy<0) return one/x; else return x;
	    }
	    if(hy==0x40000000) return x*x; /* y is  2 */
	    if(hy==0x3fe00000) {	/* y is  0.5 */
		if(hx>=0)	/* x >= +0 */
		return sqrt(x);
	    }
	}

	ax   = fabs(x);
    /* special value of x */
	if(lx==0) {
	    if(ix==0x7ff00000||ix==0||ix==0x3ff00000){
		z = ax;			/*x is +-0,+-inf,+-1*/
		if(hy<0) z = one/z;	/* z = (1/|x|) */
		if(hx<0) {
		    if(((ix-0x3ff00000)|yisint)==0) {
			z = (z-z)/(z-z); /* (-1)**non-int is NaN */
		    } else if(yisint==1)
			z = -z;		/* (x<0)**odd = -(|x|**odd) */
		}
		return z;
	    }
	}

	n = (hx>>31)+1;

    /* (x<0)**(non-int) is NaN */
	if((n|yisint)==0) return NAN;//(x-x)/(x-x);

	s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
	if((n|(yisint-1))==0) s = -one;/* (-ve)**(odd int) */

    /* |y| is huge */
	if(iy>0x41e00000) { /* if |y| > 2**31 */
	    if(iy>0x43f00000){	/* if |y| > 2**64, must o/uflow */
		if(ix<=0x3fefffff) return (hy<0)? huge*huge:tiny*tiny;
		if(ix>=0x3ff00000) return (hy>0)? huge*huge:tiny*tiny;
	    }
	/* over/underflow if x is not close to one */
	    if(ix<0x3fefffff) return (hy<0)? s*huge*huge:s*tiny*tiny;
	    if(ix>0x3ff00000) return (hy>0)? s*huge*huge:s*tiny*tiny;
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
	    if(ix<0x00100000)
		{ax *= two53; n -= 53; ix = __HI(ax); }
	    n  += ((ix)>>20)-0x3ff;
	    j  = ix&0x000fffff;
	/* determine interval */
	    ix = j|0x3ff00000;		/* normalize ix */
	    if(j<=0x3988E) k=0;		/* |x|<sqrt(3/2) */
	    else if(j<0xBB67A) k=1;	/* |x|<sqrt(3)   */
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
	    if(((j-0x40900000)|i)!=0)			/* if z > 1024 */
		return s*huge*huge;			/* overflow */
	    else {
		if(p_l+ovt>z-p_h) return s*huge*huge;	/* overflow */
	    }
	} else if((j&0x7fffffff)>=0x4090cc00 ) {	/* z <= -1075 */
	    if(((j-0xc090cc00)|i)!=0) 		/* z < -1075 */
		return s*tiny*tiny;		/* underflow */
	    else {
		if(p_l<=z-p_h) return s*tiny*tiny;	/* underflow */
	    }
	}
    /*
     * compute 2**(p_h+p_l)
     */
	i = j&0x7fffffff;
	k = (i>>20)-0x3ff;
	n = 0;
	if(i>0x3fe00000) {		/* if |z| > 0.5, set n = [z+0.5] */
	    n = j+(0x00100000>>(k+1));
	    k = ((n&0x7fffffff)>>20)-0x3ff;	/* new k for n */
	    t = zero;
	    __HI(t) = (n&~(0x000fffff>>k));
	    n = ((n&0x000fffff)|0x00100000)>>(20-k);
	    if(j<0) n = -n;
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
	if((j>>20)<=0) z = scalbn(z,n);	/* subnormal output */
	else __HI(z) += (n<<20);
	return s*z;
}

double pow(double x, double y)
{
	return __ieee754_pow(x, y);
}

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

void __assert_fail(char *assertion, char *file, int line, char *func)
{
	fprintf(stderr, "assert (%s) failed in %s at %s:%d\n", 
			assertion, func, file, line);
	abort();
}

uint32_t htonl(uint32_t hostlong)
{
	unsigned char data[4] = {};
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
	unsigned char data[2] = {};
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
	void *newbrk = (void *)syscall(__NR_brk, (long)addr, 0, 0, 0, 0, 0, 0);
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
	if (brk(_data_end + increment))
		return NULL;
	return ret;
}


static void init_mem()
{
	const size_t len = (1<<20);

	if ( (first = last = sbrk(len)) == NULL ) {
		exit(2);
	}

	first->next = NULL;
	first->prev = NULL;
	first->start = first;
	first->end = first->start + len;
	first->is_free = 1;
	first->len = len;
	first->magic = MEM_MAGIC;
}

static void mem_compress()
{
	struct mem_alloc *buf;

	for(buf = first; buf; buf = buf->next)
	{
		if(!buf->is_free)
			continue;

		struct mem_alloc *next = buf->next;

		if(next && next->is_free) {
			
			buf->len  = (buf->len + next->len);
			buf->end  = next->end;
			buf->next = next->next;
			
			if(next->next)
				next->next->prev = buf;

			memset(next, 0, sizeof(struct mem_alloc));

			if(buf->next == NULL)
				last = buf;
		}

	}
}

static void free_alloc(struct mem_alloc *tmp)
{
	struct mem_alloc *buf = tmp;

	if (!buf)
		return;

	buf->is_free = 1;

	mem_compress();

	return;
}

static struct mem_alloc *grow_pool()
{
	const size_t len = (1<<20);
	struct mem_alloc *new_last;
	struct mem_alloc *old_last = last;

	if ((new_last = sbrk(len)) == NULL)
		exit(3);

	//printf("grow_pool: new_last = %p\n", new_last);

	if (last->is_free) {
		last->end += len;
		last->len += len;
	} else {
		old_last->next = new_last;
		new_last->prev = old_last;

		new_last->len = len;
		new_last->magic = MEM_MAGIC;
		new_last->is_free = 1;
		new_last->start = new_last;
		new_last->end = new_last->start + len;

		last = new_last;
	}
	return last;
}

void check_mem()
{
	if (first == NULL) {
		printf("first is null\n");
		_exit(1);
	}
	if (last == NULL) {
		printf("last is null\n");
		_exit(1);
	}

	struct mem_alloc *tmp, *prev;

	for (tmp = first; tmp; prev = tmp, tmp = tmp->next)
	{
		if (tmp < first || tmp > last) {
			printf("%p out of range [prev=%p]\n", tmp, prev);
			_exit(1);
		}
		if (tmp->magic != MEM_MAGIC) {
			printf("%p has bad magic [prev=%p]\n", tmp, prev);
			_exit(1);
		}
		if (tmp->next == tmp || tmp->prev == tmp) {
			printf("%p circular {<%p,%p>}\n", tmp, tmp->prev, tmp->next);
			_exit(1);
		}
	}
}

static struct mem_alloc *find_free(size_t size)
{
	const struct mem_alloc *tmp;
	const size_t seek = size + (sizeof(struct mem_alloc) * 2);

	check_mem();

	//printf("find_free:   looking for %d[%d]\n", size, seek);

	for (tmp = first; tmp; tmp = tmp->next)
	{
		if (tmp < first || tmp > last)
			exit(200);
		if (tmp->next == tmp)
			exit(201);
		if (tmp->magic != MEM_MAGIC)
			exit(202);
		if (tmp->is_free && tmp->len >= seek) {
			//printf("find_free:   found @ %p[%d]\n", tmp, tmp->len);
			return (struct mem_alloc *)tmp;
		}
	}

	//printf("find_free:   growing pool\n");
	return grow_pool();
}

static struct mem_alloc *split_alloc(struct mem_alloc *old, size_t size)
{
	size_t seek;
	struct mem_alloc *rem; 

	seek = size + (sizeof(struct mem_alloc) * 2);

	if (!size)
		return NULL;
	if (!old || !old->is_free || !old->len)
		return NULL;
	if (old->len < seek)
		return NULL;

	if (!old->next && last != old)
		exit(42);
	if (!old->prev && first != old)
		exit(43);
	if (old != old->start)
		exit(40);
	if (old->start + old->len != old->end)
		exit(41);
	if (old->magic != MEM_MAGIC)
		exit(44);

	//printf("split_alloc: old=%p[%d] {<%p,%p>} size=%d\n", old, old->len, old->prev, old->next, size);

	rem = (old->start + sizeof(struct mem_alloc) + size);
	rem->magic = MEM_MAGIC;

	if (old->next)
		old->next->prev = rem;

	rem->prev = old;
	rem->next = old->next;
	old->next = rem;

	rem->is_free = 1;
	old->is_free = 0;

	rem->len = old->len - (sizeof(struct mem_alloc) + size);
	rem->start = rem;
	rem->end = rem->start + rem->len;
	
	old->len = sizeof(struct mem_alloc) + size;
	old->end = old->start + old->len;

	if (old->prev == NULL)
		first = old;
	if (rem->next == NULL)
		last = rem;

	//printf("split_alloc: old=%p[%d] rem=%p[%d]\n", old, old->len, rem, rem->len);

	return rem;
}

static struct mem_alloc *alloc_mem(size_t size)
{
	struct mem_alloc *ret;

	if (size <= 0)
		return NULL;

	if ((ret = find_free(size)) == NULL)
		return NULL;

	if ((split_alloc(ret, size)) == NULL)
		return NULL;

	return ret;
}

inline static struct __pthread *__pthread_self(void)
{
	struct __pthread *ret = NULL;
	__asm__("movq %%fs:0, %0":"=r" (ret));
	return ret;
}

int *__errno_location(void)
{
	return &__pthread_self()->errnum;
}

pid_t gettid(void)
{
	return syscall(__NR_gettid, 0, 0, 0, 0, 0, 0, 0);
}

int arch_prctl(int code, unsigned long addr)
{
	return syscall(__NR_arch_prctl, code, addr, 0,0,0,0,0);
}

void __libc_start_main(int ac, char *av[], char **envp)
{
	struct __pthread tmp;

	syscall(__NR_arch_prctl, ARCH_SET_FS, (uint64_t)&tmp, 0, 0, 0, 0, 0);

	_data_end = (void *)syscall(__NR_brk, 0, 0, 0, 0, 0, 0, 0);
	if (_data_end == (void *)-1UL)
		return;

	global_atexit_list = NULL;

	first = NULL;
	last = NULL;

	init_mem();

	/*
	printf("first = %p\nlast = %p\n_data_end = %p\n",
			first,
			last,
			_data_end);

	printf("first->start = %p\nfirst->end = %p\nfirst->len = %d\n",
			first->start,
			first->end,
			first->len);
			*/

	struct __pthread *npt = malloc(sizeof(struct __pthread));

	if (npt == NULL) exit(1);

	npt->parent_tid = 0;
	npt->self = npt;
	npt->errnum = 0;

	arch_prctl(ARCH_SET_FS, (unsigned long)npt);

	npt->my_tid = gettid();

	environ = envp;

	check_mem();
	//dump_mem();

	exit(main(ac, av, envp));
}
