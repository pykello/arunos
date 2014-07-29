#include <memory.h>
#include <proc.h>
#include <kalloc.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <system.h>
#include <types.h>

static struct Process process_table[PROCESS_COUNT_MAX];
__attribute__((__aligned__(SECTION_TABLE_ALIGNMENT)))
struct SectionTableEntry process_vm[PROCESS_COUNT_MAX][4096];

void proc_init(void)
{
	int i;
	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		process_table[i].state = UNUSED;
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

void proc_load(struct Process *proc, void *start_addr, void *end_addr)
{
	char *current_addr = (char *) start_addr;
	uint32_t vpage = 0;
	uint32_t required_memory = 0;

	struct ElfHeader *header = (struct ElfHeader *) start_addr;
	proc->entry = (entry_function) header->entry;

	required_memory = ((uint32_t) end_addr - (uint32_t) start_addr);
	if (required_memory > proc->heap_size) {
		uint32_t required_pages = required_memory / PAGE_SIZE + 1;
		uint32_t current_pages = proc->heap_size / PAGE_SIZE;

		proc_expand_memory(proc, required_pages - current_pages);
	}

	while (current_addr < (char *) end_addr) {
		char *page = NULL;

		page = (char *) P2V(resolve_physical_address(proc->vm, vpage));
		memcpy(page, current_addr, PAGE_SIZE);

		current_addr += PAGE_SIZE;
		vpage += PAGE_SIZE;
	}
}

void proc_switch(struct Process *proc)
{
	set_translation_table_base((uint32_t) V2P(proc->vm));
	proc->entry();
}
