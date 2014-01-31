#ifndef STDIO_H
#define STDIO_H

#include "lib/stdarg.h"

#define BUFFER_MAX_LENGTH 128


extern char * gets(char *buffer);
extern int printf(const char *format, ...);
extern int sprintf(char *target, const char *format, ...);
extern int vsprintf(char *target, const char *format, va_list ap);

#endif
