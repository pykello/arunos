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
	SYSCALL_EXIT,
	SYSCALL_GETPID,
	SYSCALL_FORK,
	SYSCALL_EXEC,
	SYSCALL_YIELD,
	SYSCALL_WAIT
};

int syscall0(enum SystemCallCode code);
int syscall1(enum SystemCallCode code, int arg1);
int syscall2(enum SystemCallCode code, int arg1, int arg2);

void putch(int c);
int getch(void);
void exit(int code);
int getpid(void);
int fork(void);
void exec(int id);
void yield(void);
void wait(int id);

#endif
