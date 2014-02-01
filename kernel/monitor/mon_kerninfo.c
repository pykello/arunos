#include <monitor.h>
#include <lib/stdio.h>
#include <types.h>

/* mon_kerninfo */
int
mon_kerninfo(int argc, char **argv)
{
	extern char entry[], etext[], edata[], end[];
	int footprint_kb = (end - entry + 1023) / 1024;

	(void) argc;
	(void) argv;

	printf("Special kernel symbols:\n");
	printf("  entry  %x (phys)\n", entry);
	printf("  etext  %x (phys)\n", etext);
	printf("  edata  %x (phys)\n", edata);
	printf("  end    %x (phys)\n", end);
	printf("Kernel executable memory footprint: %dKB\n",
	       footprint_kb);

	return 0;
}
