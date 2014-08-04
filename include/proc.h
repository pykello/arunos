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

extern struct Process *current_process;

#define ELF_MAGIC 0x464C457FU

enum ElfType {
	ELFTYPE_NONE        = 0,
	ELFTYPE_RELOCATABLE = 1,
	ELFTYPE_EXECUTABLE  = 2
};

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

struct ElfProgramHeader {
  uint32_t type;
  uint32_t off;
  uint32_t vaddr;
  uint32_t paddr;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags;
  uint32_t align;
};

void proc_init(void);
struct Process *proc_create(void);
void proc_free(struct Process *proc);
void proc_expand_memory(struct Process *proc, int page_count);
bool proc_load(struct Process *proc, char **proc_image, int page_count);
void proc_switch(struct Process *proc);

#endif
