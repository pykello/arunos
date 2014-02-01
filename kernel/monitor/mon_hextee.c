#include <monitor.h>
#include <console.h>
#include <lib/stdio.h>
#include <types.h>

/* mon_hextee */
int mon_hextee(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	printf("press 'q' to exit.\n");

	while (true) {
		int c = getch();
		if (c == 'q') {
			printf("\n");
			break;
		}

		if (c != 0)
			printf("%x ", c);
	}

	return 0;
}
