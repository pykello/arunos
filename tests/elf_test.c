#include <lib/stdio.h>
#include <lib/syscall.h>

static volatile unsigned char bss[9000];
static volatile int data = 123;

void _start(void)
{
	unsigned int i;

	for (i = 0; i < sizeof(bss); i++)
		if (bss[i] != 0) {
			printf("BSS FAIL\n");
			exit(1);
		}
	if (data != 123 || exec(-1) != -1 || exec(10) != -1 ||
	    exec(9) != -1 || exec(6) != -1) {
		printf("EXEC FAIL\n");
		exit(1);
	}
	printf("ELF data/BSS and failed exec preserved caller\n");
	exit(0);
}
