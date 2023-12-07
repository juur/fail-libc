#ifndef STDIO_H
#define STDIO_H

#include <features.h>

#define L_ctermid 9

#define EOF (-1)
#define BUFSIZ 8192

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 3

#define putc(x,y) fputc(x,y)

#include <sys/types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

typedef struct {
	char  *buf;
    void  *mem;
    size_t mem_size;
	off_t  offset;
	size_t blen;
	int    buf_mode;
	int    fd;
	int    error;
	int    unwind;
	int    bpos;
	int    bhas;
    int    flags;
	bool   has_unwind;
	bool   eof;
} FILE;

typedef union {
	long long data;
} fpos_t;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern void     clearerr(FILE *);
extern char    *ctermid(char *);
extern int      dprintf(int, const char *, ...);
extern int      fclose(FILE *);
extern FILE    *fdopen(int, const char *);
extern int      feof(FILE *);
extern int      ferror(FILE *);
extern int      fflush(FILE *);
extern int      fgetc(FILE *);
extern int      fgetpos(FILE *, fpos_t *);
extern char    *fgets(char *, int, FILE *);
extern int      fileno(FILE *);
extern void     flockfile(FILE *);
extern FILE    *fmemopen(void *, size_t, const char *); 
extern FILE    *fopen(const char *, const char *);
extern int      fprintf(FILE *, const char *, ...);
extern int      fputc(int, FILE *);
extern int      fputs(const char *, FILE *);
extern size_t   fread(void *, size_t, size_t, FILE *);
extern FILE    *freopen(const char *, const char *, FILE *);
extern int      fscanf(FILE *, const char *, ...);
extern int      fseek(FILE *, long, int);
extern int      fseeko(FILE *, off_t, int);
extern int      fsetpos(FILE *, const fpos_t *);
extern long     ftell(FILE *);
extern off_t    ftello(FILE *);
extern int      ftrylockfile(FILE *);
extern void     funlockfile(FILE *);
extern size_t   fwrite(const void *, size_t, size_t, FILE *);
extern int      getc(FILE *);
extern int      getchar(void);
extern int      getc_unlocked(FILE *);
extern int      getchar_unlocked(void);
extern ssize_t  getdelim(char **, size_t *, int, FILE *);
extern ssize_t  getline(char **, size_t *, FILE *);
extern char    *gets(char *);
extern FILE    *open_memstream(char **, size_t *);
extern int      pclose(FILE *);
extern void     perror(const char *);
extern FILE    *popen(const char *, const char *);
extern int      printf(const char *, ...);
extern int      putchar(int);
extern int      putc_unlocked(int, FILE *);
extern int      putchar_unlocked(int);
extern int      puts(const char *);
extern int      remove(const char *);
extern int      rename(const char *, const char *);
extern int      renameat(int, const char *, int, const char *);
extern void     rewind(FILE *);
extern int      scanf(const char *, ...);
extern void     setbuf(FILE *, char *);
extern int      setvbuf(FILE *, char *, int, size_t);
extern int      snprintf(char *, size_t, const char *, ...);
extern int      sprintf(char *, const char *, ...);
extern int      sscanf(const char *, const char *, ...);
extern char    *tempnam(const char *, const char *);
extern FILE    *tmpfile(void);
extern char    *tmpnam(char *);
extern int      ungetc(int, FILE *);
extern int      vdprintf(int, const char *, va_list);
extern int      vfprintf(FILE *, const char *, va_list);
extern int      vfscanf(FILE *, const char *, va_list);
extern int      vprintf(const char *, va_list);
extern int      vscanf(const char *, va_list);
extern int      vsnprintf(char *, size_t, const char *, va_list);
extern int      vsprintf(char *, const char *, va_list);
extern int      vsscanf(const char *, const char *, va_list);

#endif

// vim: set ft=c:
