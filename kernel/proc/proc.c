#include <vm.h>
#include <proc.h>
#include <kalloc.h>
#include <klib.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <system.h>
#include <types.h>

struct Process process_table[PROCESS_COUNT_MAX];
__attribute__((__aligned__(SECTION_TABLE_ALIGNMENT)))
struct SectionTableEntry process_vm[PROCESS_COUNT_MAX][4096];

struct Process *current_process = NULL;

/* proc_init initializes the process sub-system. */
void proc_init(void)
{
	int i;
	for (i = 0; i < PROCESS_COUNT_MAX; i++)
		process_table[i].state = UNUSED;
	current_process = NULL;
}

/* proc_creates allocates a new process and returns it. */
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
			pid = i + 1;
			break;
		}

	if (pid == -1)
		return NULL;

	kernel_stack = kalloc();
	user_stack = kalloc();

	vm = process_vm[pid - 1];
	memset(vm, 0, SECTION_TABLE_SIZE);

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


	proc = &process_table[pid - 1];
	proc->pid = pid;
	proc->state = CREATED;
	proc->vm = vm;
	proc->heap_size = 0;
	proc->kernel_stack = kernel_stack;
	proc->user_stack = user_stack;
	proc->wait_pid = -1;

	return proc;
}

/* proc_free frees all resources allocated by proc. */
void proc_free(struct Process *proc)
{
	kfree(proc->kernel_stack);
	kfree(proc->user_stack);
	proc_shrink_memory(proc, proc->heap_size / PAGE_SIZE);
	free_vm_page_tables(proc->vm);
	proc->state = UNUSED;
}

/* proc_exapnad_memory expands the heap size of the given process. */
void proc_expand_memory(struct Process *proc, int page_count)
{
	int i = 0;

	for (i = 0; i < page_count; i++) {
		char *page = kalloc();
		memset(page, 0, PAGE_SIZE);

		map_pages(proc->vm, (struct MemoryMapping){
			proc->heap_size,
			V2P(page),
			V2P(page) + PAGE_SIZE,
			AP_RW_RW
		});

		proc->heap_size += PAGE_SIZE;
	}
}

/* proc_shrink_memory shrinks the heap size of the given process. */
void proc_shrink_memory(struct Process *proc, int page_count)
{
	int i = 0;

	for (i = 0; i < page_count; i++) {
		uint32_t virtual_addr = proc->heap_size - PAGE_SIZE;
		uint32_t physical_addr = resolve_physical_address(proc->vm,
								  virtual_addr);
		uint32_t kernel_addr = P2V(physical_addr);

		kfree((void *) kernel_addr);
		unmap_page(proc->vm, virtual_addr);

		proc->heap_size -= PAGE_SIZE;
		if (proc->heap_size == 0) {
			break;
		}
	}
}

/* proc_load loads the given ELF process image into the given process. */
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

		/* make enough room for this section */
		while (proc->heap_size < header->vaddr + header->memsz)
			proc_expand_memory(proc, 1);

		/* copy the section */
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
	proc->state = READY;

	memset(proc->context, 0, sizeof(proc->context));
	proc->context[CPSR] = 0x10;
	proc->context[RESTART_ADDR] = (int) proc->entry;
	proc->context[SP] = USER_STACK_BOTTOM + PAGE_SIZE;

	return true;
}

/* proc_start starts running the given process. */
void proc_start(struct Process *proc)
{
	current_process = proc;

	set_translation_table_base((uint32_t) V2P(proc->vm));

	/* clear TLB */
	__asm__ volatile("mov R4, #0");
	__asm__ volatile("MCR p15, 0, R4, c8, c7, 0");

	switch_to_context(proc->context);
}

int *get_current_context(void)
{
	return current_process->context;
}
