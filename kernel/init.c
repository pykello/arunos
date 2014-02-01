#include <console.h>
#include <monitor.h>

/*
 * Kernel starts executing C code here. Before entring this function, some
 * initialization has been done in the assembly code.
 */
void c_entry()
{
	console_init();

	/* start the kernel monitor, which should run forever */
	monitor();
}
