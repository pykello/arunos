/*
 * system.h
 *
 * Type and function declarations for system functions.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef SYSTEM_H
#define SYSTEM_H

enum {
	CPU_MODE_USER       = 0x10,
	CPU_MODE_FIQ        = 0x11,
	CPU_MODE_IRQ        = 0x12,
	CPU_MODE_SUPERVISOR = 0x13,
	CPU_MODE_ABORT      = 0x17,
	CPU_MODE_UNDEFINED  = 0x1b,
	CPU_MODE_SYSTEM     = 0x1f
};

void enable_interrupts(void);
char *get_stack_pointer(int cpu_mode);
int read_cpsr(void);

#endif
