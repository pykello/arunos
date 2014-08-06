#include <memory.h>
#include <proc.h>
#include <kalloc.h>
#include <klib.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <system.h>
#include <types.h>

static struct Process process_table[PROCESS_COUNT_MAX];
__attribute__((__aligned__(SECTION_TABLE_ALIGNMENT)))
struct SectionTableEntry process_vm[PROCESS_COUNT_MAX][4096];

struct Process *current_process = NULL;

void proc_init(void)
{
	int i;
	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		process_table[i].state = UNUSED;
	current_process = NULL;
}

struct Process *proc_create(void)
{
	struct Process *proc = NULL;
	int i;
	int pid = -1;
	struct SectionTableEntry *vm = NULL;
	char *kernel_stack = NULL;
	char *user_stack = NULL;

	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		if (process_table[i].state == UNUSED) {
			pid = i;
			break;
		}

	if (pid == -1)
		return NULL;

	kernel_stack = kalloc();
	user_stack = kalloc();

	vm = process_vm[pid];
	setup_kernel_vm(vm);
	map_pages(vm, (struct MemoryMapping){
		KERNEL_STACK_BOTTOM,
		V2P(kernel_stack),
		V2P(kernel_stack) + PAGE_SIZE,
		AP_RW_R
	});
	map_pages(vm, (struct MemoryMapping){
		USER_STACK_BOTTOM,
		V2P(user_stack),
		V2P(user_stack) + PAGE_SIZE,
		AP_RW_RW
	});


	proc = &process_table[pid];
	proc->pid = pid;
	proc->state = CREATED;
	proc->vm = vm;
	proc->heap_size = 0;
	proc->kernel_stack = kernel_stack;
	proc->user_stack = user_stack;

	return proc;
}

void proc_free(struct Process *proc)
{
	uint32_t virtual_addr = 0;
	uint32_t physical_addr = 0;

	kfree(proc->kernel_stack);
	kfree(proc->user_stack);
	
	while (virtual_addr < proc->heap_size) {
		physical_addr = resolve_physical_address(proc->vm, virtual_addr);
		kfree((void *) P2V(physical_addr));

		virtual_addr += PAGE_SIZE;
	}
}

void proc_expand_memory(struct Process *proc, int page_count)
{
	int i = 0;

	for (i = 0; i < page_count; i++) {
		char *page = kalloc();
		map_pages(proc->vm, (struct MemoryMapping){
			proc->heap_size,
			V2P(page),
			V2P(page) + PAGE_SIZE,
			AP_RW_RW
		});

		proc->heap_size += PAGE_SIZE;
	}
}

bool proc_load(struct Process *proc, char **proc_image, int page_count)
{
	int prog_header_offset = 0;
	int prog_header_count = 0;
	int i = 0;

	struct ElfHeader *header = (struct ElfHeader *) proc_image[0];
	if (header->type != ELFTYPE_EXECUTABLE)
		return false;

	(void) page_count;

	prog_header_offset = header->phoff;
	prog_header_count = header->phnum;

	for (i = 0; i < prog_header_count; i++) {
		uint32_t j = 0;
		struct ElfProgramHeader *header = (void *) (proc_image[0] + prog_header_offset);

		while (proc->heap_size < header->vaddr + header->memsz)
			proc_expand_memory(proc, 1);

		for (j = 0; j < header->memsz; j++) {
			int vaddr = header->vaddr + j;
			int paddr = resolve_physical_address(proc->vm, vaddr);
			char *ptr = (char *) P2V(paddr);
			int image_off = header->off + j;
			*ptr = proc_image[image_off / PAGE_SIZE][image_off % PAGE_SIZE];
		}

		prog_header_offset += sizeof(struct ElfProgramHeader);
	}

	proc->entry = (entry_function) header->entry;

	return true;
}

void proc_switch(struct Process *proc)
{
	current_process = proc;
	set_translation_table_base((uint32_t) V2P(proc->vm));
	proc->context[CPSR] = 0x10;
	proc->context[RESTART_ADDR] = (int) proc->entry;
	proc->context[SP] = USER_STACK_BOTTOM + PAGE_SIZE;
	switch_to_context(proc->context);
}
