#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

#define bool _Bool
#define true 1
#define false 0
#define NULL (void *)(0)

#define ESRCH	3
#define ENOMEM 12
#define EACCES 13
#define EINVAL 22

#define SIGABRT 6

#define __NR_read           0
#define __NR_write          1
#define __NR_open		    2
#define __NR_close          3
#define __NR_stat           4
#define __NR_brk           12
#define __NR_access		   21
#define __NR_nanosleep     35
#define __NR_getpid		   39
#define __NR_kill		   62
#define __NR_gettimeofday  96
#define __NR_arch_prctl	  158	
#define __NR_gettid       186
#define __NR_exit_group   231

#define ARCH_SET_FS	0x1002

#define O_RDONLY	   00
#define O_WRONLY	   01
#define O_RDWR		   02
#define O_CREAT		 0100
#define O_TRUNC		01000
#define O_APPEND	02000

#define S_IRUSR		0400
#define S_IWUSR		0200
#define S_IXUSR		0100
#define S_IRGRP		0040
#define S_IWGRP		0020
#define S_IXGRP		0010
#define S_IROTH		0004
#define S_IWOTH		0002
#define S_IXOTH		0001

int *__errno_location (void);
#define errno (*__errno_location())

typedef unsigned long		size_t;
typedef unsigned long		intptr_t;
typedef signed long			ssize_t;
typedef int					time_t;
typedef unsigned int		pid_t;
typedef unsigned int		pthread_t;
typedef unsigned			mode_t;
typedef long				suseconds_t;
typedef unsigned int		uint32_t;
typedef unsigned short		uint16_t;
typedef __builtin_va_list	va_list;

struct __pthread {
	struct __pthread *self;
	int tid;
	int errnum;
};

typedef struct {
	bool eof;
	int  fd;
	int  error;
} FILE;

struct timespec {
	time_t tv_sec;
	long   tv_nsec;
};

struct timeval {
	time_t      tv_sec;
	suseconds_t tv_usec;
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;    
};

struct stat {
};

typedef struct {
} pthread_rwlock_t;

typedef struct {
} pthread_mutex_t;

typedef struct {
} pthread_mutexattr_t;

typedef struct {
} pthread_rwlockattr_t;

typedef struct {
} pthread_attr_t;

struct atexit_fun {
	struct atexit_fun *next;
	void (*function)(void);
};

FILE *stdin  = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

extern long int syscall(long number, ...);

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

void exit_group(int status) {
	for (;;) syscall(__NR_exit_group, status);
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

void _exit(int status)
{
	exit_group(status);
}

void exit(int status)
{
	_exit(status);
}

ssize_t write(int fd, const void *buf, size_t count)
{
	return syscall(__NR_write, fd, buf, count);;
}

ssize_t read(int fd, void *buf, size_t count)
{
	return syscall(__NR_read, fd, buf, count);
}

int open(const char *pathname, int flags, mode_t mode)
{
	return syscall(__NR_open, pathname, flags, mode);
}

int access(const char *pathname, int mode)
{
	return syscall(__NR_access, pathname, mode);
}

int close(int fd)
{
	return syscall(__NR_close, fd);
}

int stat(const char *pathname, struct stat *statbuf)
{
	errno = ENOMEM;
	return -1;
}

int fclose(FILE *stream)
{
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
	if (fd == -1)
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

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	return syscall(__NR_gettimeofday, tv, tz);
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return syscall(__NR_nanosleep, req, rem);
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
}

void *malloc(size_t size)
{
	void *new;
	void *old = sbrk(0);
	if ((new = sbrk(size)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	return old;
}

void *realloc(void *ptr, size_t size)
{
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
	ret = malloc(nmemb * size);
	if (ret == NULL)
		return NULL;
	memset(ret, 0, nmemb * size);

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

static struct atexit_fun *global_atexit_list;

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
	return syscall(__NR_kill, pid, sig);
}

pid_t getpid(void)
{
	return syscall(__NR_getpid);
}

void raise(int sig)
{
	kill(getpid(), sig);
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

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
	return ENOMEM;
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

int __xpg_strerror_r(int errnum, char *buf, size_t buflen)
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

double log(double x)
{
	return 0;
}

double log10(double x)
{
	return 0;
}

double pow(double x, double y)
{
	return 0;
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

void __assert_fail()
{
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
	unsigned char data[4] = {};
    memcpy(&data, &net, sizeof(data));

    return ((uint32_t) data[3] << 0)
         | ((uint32_t) data[2] << 8)
         | ((uint32_t) data[1] << 16)
         | ((uint32_t) data[0] << 24);
}

uint16_t ntohs(uint16_t net)
{
	unsigned char data[2] = {};
    memcpy(&data, &net, sizeof(data));

    return ((uint32_t) data[1] << 0)
         | ((uint32_t) data[0] << 8);
}

static void *_data_end;

int brk(void *addr)
{
	void *newbrk = (void *)syscall(__NR_brk, addr);
	if (newbrk == NULL || newbrk == _data_end || newbrk != addr) {
		errno = ENOMEM;
		return -1;
	}
	_data_end = newbrk;
	return 0;
}

void *sbrk(intptr_t increment)
{
	if (increment == 0)
		return _data_end;
	if (brk(_data_end + increment))
		return NULL;
	return _data_end;
}

struct mem_alloc {
	struct mem_alloc *next;
	struct mem_pool  *head;
	unsigned is_free : 1;
	size_t len;
	void *start;
	void *end;
};

struct mem_pool {
	struct mem_pool *next;
	struct mem_alloc *first;
	size_t len;
	void *start;
	void *end;
};

struct mem_pool *pools;

static void init_mem()
{
	void *chunk;
	struct mem_alloc *first;

	if ( (chunk = sbrk( (1<<24) + sizeof(struct mem_pool) + sizeof(struct mem_alloc))) == NULL )
		exit(1);

	pools = chunk;
	first = chunk + sizeof(struct mem_pool);

	pools->next = NULL;
	pools->first = first;
	pools->len = (1<<24);
	pools->start = first + sizeof(struct mem_alloc);
	pools->end = pools->start + pools->len;

	first->next = NULL;
	first->head = pools;
	first->is_free = true;
	first->len = (1<<24);
	first->start = pools->start;
	first->end = pools->end;
}

extern int main(int, char *[], char *[]);

char **environ;

inline static struct __pthread *__pthread_self(void)
{
	struct __pthread *ret = NULL;
	__asm__("mov %%fs:0,%0" : "=r" (ret));
	return ret;
}

int *__errno_location(void)
{
	return &__pthread_self()->errnum;
}

int set_fs(void *dst)
{
	return syscall(__NR_arch_prctl, ARCH_SET_FS, (unsigned long)dst);
}

static pid_t gettid(void)
{
	return syscall(__NR_gettid);
}

void __libc_start_main(int ac, char *av[], char **envp)
{
	_data_end = (void *)syscall(__NR_brk, NULL);
	global_atexit_list = NULL;
	init_mem();

	stdin = fdopen(0, "r");
	stdout = fdopen(1, "w");
	stderr = fdopen(2, "w");

	struct __pthread *npt = malloc(sizeof(struct __pthread));

	if (npt == NULL) exit(1);

	npt->tid = gettid();
	npt->self = npt;
	npt->errnum = 0;

	set_fs(npt);

	environ = envp;

	exit(main(ac, av, envp));
}
