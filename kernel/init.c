#include <console.h>
#include <memory.h>
#include <monitor.h>

/*
 * Kernel starts executing C code here. Before entring this function, some
 * initialization has been done in the assembly code.
 */
void c_entry(void)
{
	memory_init();
	jump_to_high_mem();
	console_init();

	/* start the kernel monitor, which should run forever */
	monitor();
}
