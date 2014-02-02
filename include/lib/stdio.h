/*
 * lib/stdio.h
 *
 * Type and function declarations for standard input/output.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef STDIO_H
#define STDIO_H

#include <lib/stdarg.h>

#define BUFFER_MAX_LENGTH 256

char *gets(char *buffer);
int printf(const char *format, ...);
int sprintf(char *target, const char *format, ...);
int vsprintf(char *target, const char *format, va_list ap);

#endif
