#ifndef _FMTMSG
#define _FMTMSG

/* one of */
#define MM_HARD      0x0001
#define MM_SOFT      0x0002
#define MM_FIRM      0x0003

/* one of */
#define MM_APPL      0x0010
#define MM_UTIL      0x0020
#define MM_OPSYS     0x0030

/* one of */
#define MM_RECOVER   0x0100
#define MM_NRECOV    0x0200

/* any number of */
#define MM_PRINT     0x1000
#define MM_CONSOLE   0x2000

/* or, instead */
#define MM_NULLMC    0x0000

#define MM_HALT 4
#define MM_ERROR 3
#define MM_WARNING 2
#define MM_INFO 1
#define MM_NOSEV 0

#define MM_OK 0
#define MM_NOTOK -1
#define MM_NOMSG -2
#define MM_NOCON -3

int fmtmsg(long, const char *, int, const char *, const char *, const char *);

#endif
