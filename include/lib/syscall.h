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
	SYSCALL_GETCH,
	SYSCALL_EXIT
};

int syscall0(enum SystemCallCode code);
int syscall1(enum SystemCallCode code, int arg1);
int syscall2(enum SystemCallCode code, int arg1, int arg2);

#endif