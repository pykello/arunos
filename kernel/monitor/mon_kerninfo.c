#include <monitor.h>
#include <klib.h>
#include <types.h>

/* mon_kerninfo displays information about the kernel. */
int mon_kerninfo(int argc, char **argv)
{
	extern char entry[], etext[], edata[], kernel_end[];
	int footprint_kb = (kernel_end - entry + 1023) / 1024;

	(void) argc;
	(void) argv;

	kprintf("Special kernel symbols:\n");
	kprintf("  entry  %x (phys)\n", entry);
	kprintf("  etext  %x (phys)\n", etext);
	kprintf("  edata  %x (phys)\n", edata);
	kprintf("  end    %x (phys)\n", kernel_end);
	kprintf("Kernel executable memory footprint: %dKB\n",
		footprint_kb);

	return 0;
}
