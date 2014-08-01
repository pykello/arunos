#include <monitor.h>
#include <console.h>
#include <klib.h>
#include <types.h>

/* mon_hextee */
int mon_hextee(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	kprintf("press 'q' to exit.\n");

	while (true) {
		int c = kgetch();
		if (c == 'q') {
			kprintf("\n");
			break;
		}

		if (c != 0)
			kprintf("%x ", c);
	}

	return 0;
}
