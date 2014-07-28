/*
 * proc.h
 *
 * Type and function declarations for process management.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef PROC_H
#define PROC_H

#include <memory.h>
#include <types.h>

#define PROCESS_COUNT_MAX 32

enum ProcessState {
	UNUSED,
	CREATED,
	SLEEPING,
	READY,
	RUNNING,
	TERMINATED
};

struct Process {
	enum ProcessState state;
	int pid;
	struct SectionTableEntry *vm;
	uint32_t heap_size;
	char *user_stack;
	char *kernel_stack;
	int context[12];
};

void proc_init(void);
struct Process *proc_create(void);
void proc_free(struct Process *proc);
void proc_expand_memory(struct Process *proc, int page_count);
void proc_load(struct Process *proc, void *start, void *end);
void proc_switch(struct Process *proc);

#endif