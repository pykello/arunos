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

typedef void (*entry_function)(void);
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
	entry_function entry;
	struct SectionTableEntry *vm;
	uint32_t heap_size;
	char *user_stack;
	char *kernel_stack;
	int context[12];
};

#define ELF_MAGIC 0x464C457FU

struct ElfHeader {
	uint32_t magic;
	char elf[12];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t phoff;
	uint32_t shoff;
	uint32_t flags;
	uint16_t ehsize;
	uint16_t phentsize;
	uint16_t phnum;
	uint16_t shentsize;
	uint16_t shnum;
	uint16_t shstrndx;
};

void proc_init(void);
struct Process *proc_create(void);
void proc_free(struct Process *proc);
void proc_expand_memory(struct Process *proc, int page_count);
void proc_load(struct Process *proc, void *start, void *end);
void proc_switch(struct Process *proc);

#endif
