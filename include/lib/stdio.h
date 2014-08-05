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
#include <lib/syscall.h>

#define BUFFER_MAX_LENGTH 256

char *gets_base(int (*getch)(void), void (*putch)(int), char *buffer);
int printf_base(void (*putch)(int), const char *format, ...);
int sprintf(char *target, const char *format, ...);
int vsprintf(char *target, const char *format, va_list ap);

#define gets(buffer) gets_base(getch, putch, buffer)
#define printf(...) printf_base(putch, ##__VA_ARGS__)

#endif
