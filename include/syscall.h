/*
 * syscall.h
 *
 * Type and function declarations for system call handling.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>

enum SystemCallCode {
	SYSCALL_PRINT,
	SYSCALL_EXIT
};

void handle_syscall(void);

#endif
