/*
 * lib/syscall.h
 *
 * Type and function declarations for system calls.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef SYSCALL_H
#define SYSCALL_H

enum SystemCallCode {
	SYSCALL_PUTCH,
	SYSCALL_EXIT
};

void syscall1(enum SystemCallCode code, int arg1);
void syscall2(enum SystemCallCode code, int arg1, int arg2);

#endif
