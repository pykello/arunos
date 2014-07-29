/*
 * klib.h
 *
 * Type and function declarations for kernel library functions.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef KLIB_H
#define KLIB_H

char *kgets(char *buffer);
int kprintf(const char *format, ...);

#endif
