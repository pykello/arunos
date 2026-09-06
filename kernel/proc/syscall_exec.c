#include <fat.h>
#include <lib/string.h>
#include <proc.h>

/* Bound sparse/malformed images before they can exhaust kernel memory. */
#define IMAGE_LIMIT (1024 * 1024)
#define HEADER_LIMIT 16

static bool load_elf(struct Process *proc, const struct FatFile *file)
{
	struct ElfHeader elf;
	struct ElfProgramHeader ph[HEADER_LIMIT];
	uint32_t i, end = 0;
	bool entry_found = false;

	if (!fat_read(file, 0, &elf, sizeof(elf)) ||
	    elf.magic != ELF_MAGIC || elf.elf[0] != 1 ||
	    elf.elf[1] != 1 || elf.elf[2] != 1 ||
	    elf.type != ELFTYPE_EXECUTABLE || elf.machine != 40 ||
	    elf.version != 1 || elf.ehsize != sizeof(elf) ||
	    elf.phentsize != sizeof(ph[0]) || !elf.phnum ||
	    elf.phnum > HEADER_LIMIT || (elf.entry & 3) ||
	    !fat_read(file, elf.phoff, ph, elf.phnum * sizeof(ph[0])))
		return false;

	for (i = 0; i < elf.phnum; i++) {
		/* Dynamic linking and interpreters are not supported. */
		if (ph[i].type == 2 || ph[i].type == 3)
			return false;
		if (ph[i].type != 1)
			continue;
		if (ph[i].filesz > ph[i].memsz ||
		    ph[i].vaddr > IMAGE_LIMIT ||
		    ph[i].memsz > IMAGE_LIMIT - ph[i].vaddr ||
		    ph[i].off > file->size ||
		    ph[i].filesz > file->size - ph[i].off)
			return false;
		if (ph[i].vaddr + ph[i].memsz > end)
			end = ph[i].vaddr + ph[i].memsz;
		if ((ph[i].flags & 1) && elf.entry >= ph[i].vaddr &&
		    elf.entry - ph[i].vaddr < ph[i].filesz)
			entry_found = true;
	}
	if (!entry_found)
		return false;
	proc_expand_memory(proc, ROUND_UP(end, PAGE_SIZE) / PAGE_SIZE);
	for (i = 0; i < elf.phnum; i++) {
		uint32_t offset = 0;
		if (ph[i].type != 1)
			continue;
		while (offset < ph[i].memsz) {
			uint32_t addr = ph[i].vaddr + offset;
			uint32_t n = PAGE_SIZE - addr % PAGE_SIZE;
			void *dst = (void *)P2V(resolve_physical_address(
						proc->vm, addr));
			if (n > ph[i].memsz - offset)
				n = ph[i].memsz - offset;
			if (offset < ph[i].filesz) {
				if (n > ph[i].filesz - offset)
					n = ph[i].filesz - offset;
				if (!fat_read(file, ph[i].off + offset, dst, n))
					return false;
			} else {
				memset(dst, 0, n);
			}
			offset += n;
		}
	}
	proc->entry = (entry_function)elf.entry;
	memset(proc->context, 0, sizeof(proc->context));
	proc->context[CPSR] = 0x10;
	proc->context[RESTART_ADDR] = elf.entry;
	proc->context[SP] = USER_STACK_BOTTOM + PAGE_SIZE;
	return true;
}

/* Load privately so missing files and read errors preserve the caller. */
bool proc_load_program(struct Process *proc, const char *name)
{
	struct FatFile file;
	struct Process *image;
	uint32_t i, old_heap;
	char *old_kernel_stack, *old_user_stack;

	if (!proc || !fat_open(name, &file))
		return false;
	image = proc_create();
	if (!image)
		return false;
	if (!load_elf(image, &file)) {
		proc_free(image);
		return false;
	}
	/* Keep page-table ownership tied to process slots. */
	for (i = 0; i < 4096; i++) {
		struct SectionTableEntry old = proc->vm[i];
		proc->vm[i] = image->vm[i];
		image->vm[i] = old;
	}
	old_heap = proc->heap_size;
	old_kernel_stack = proc->kernel_stack;
	old_user_stack = proc->user_stack;
	proc->heap_size = image->heap_size;
	proc->kernel_stack = image->kernel_stack;
	proc->user_stack = image->user_stack;
	proc->entry = image->entry;
	memcpy(proc->context, image->context, sizeof(proc->context));
	proc->state = READY;
	image->heap_size = old_heap;
	image->kernel_stack = old_kernel_stack;
	image->user_stack = old_user_stack;
	proc_free(image);
	return true;
}

int syscall_exec(const char *user_name)
{
	char name[13];
	uint32_t i, addr = (uint32_t)user_name;

	if (!addr)
		return -1;
	for (i = 0; i < sizeof(name); i++, addr++) {
		/* Only the user heap and stack contain readable user pages. */
		if (addr >= current_process->heap_size &&
		    (addr < USER_STACK_BOTTOM ||
		     addr >= USER_STACK_BOTTOM + PAGE_SIZE))
			return -1;
		name[i] = *(char *)P2V(resolve_physical_address(
					current_process->vm, addr));
		if (!name[i]) {
			if (proc_load_program(current_process, name))
				proc_start(current_process);
			return -1;
		}
	}
	return -1;
}
