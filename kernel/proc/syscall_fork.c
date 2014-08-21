#include <ksyscall.h>
#include <proc.h>
#include <lib/string.h>

int syscall_fork(void)
{
	struct Process *child = NULL;
	struct Process *parent = current_process;
	uint32_t i = 0;

	child = proc_create();
	proc_expand_memory(child, parent->heap_size / PAGE_SIZE);

	/* copy parent's memory to child's memory */
	for (i = 0; i < parent->heap_size; i++) {
		int child_paddr = resolve_physical_address(child->vm, i);
		char *child_ptr = (char *) P2V(child_paddr);
		char *parent_ptr = (char *) i;

		*child_ptr = *parent_ptr;
	}

	/* copy parent's stack to child's stack */
	memcpy(child->kernel_stack, parent->kernel_stack, PAGE_SIZE);
	memcpy(child->user_stack, parent->user_stack, PAGE_SIZE);

	/* copy parent's context to child's context */
	memcpy(child->context, parent->context, sizeof(child->context));

	/* set return value of fork in child to 0 */
	child->context[R0] = 0;

	/* child is ready to run */
	child->state = READY;

	/* return pid of child to the parent. */
	return child->pid;
}
