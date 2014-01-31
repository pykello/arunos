#ifndef STRING_H
#define STRING_H

#include "types.h"


extern char * strchr(const char *str, int character);
extern int strcmp(const char *s1, const char *s2);
extern size_t strlen(const char *str);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char * strtok (char *str, const char *delimiters);

#endif