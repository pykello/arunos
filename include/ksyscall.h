/*
 * ksyscall.h
 *
 * Type and function declarations for system call handling.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef KSYSCALL_H
#define KSYSCALL_H

#include <lib/syscall.h>

int handle_syscall(int code, int arg1, int arg2, int arg3);

#endif
