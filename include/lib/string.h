/*
 * lib/string.h
 *
 * Type and function declarations for string operations.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef STRING_H
#define STRING_H

#include <types.h>

void *memcpy(void *target, const void *source, size_t n);
char *strchr(const char *str, int character);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *target, const char *source);
size_t strlen(const char *str);
int strncmp(const char *s1, const char *s2, size_t n);
char *strtok (char *str, const char *delimiters);

#endif
