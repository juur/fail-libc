#ifndef STRING_H
#define STRING_H
#include <sys/types.h>
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strdup(const char *s);
char *strndup(const char *s, size_t n);
char *strstr(const char *haystack, const char *needle);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
void *memcpy(void *dest, const void *src, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
char *strerror(int errnum);
int strerror_r(int errnum, char *buf, size_t buflen);
void *memset(void *, int, size_t);
#endif
// vim: set ft=c:
