#ifndef _FNMATCH_H
#define _FNMATCH_H

#define FNM_PATHNAME 0x1
#define FNM_NOMATCH  0x1
#define FNM_NOESCAPE 0x2
#define FNM_PERIOD   0x4

int fnmatch(const char *, const char *, int);

#endif
