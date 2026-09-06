#include <lib/stdio.h>
#include <lib/syscall.h>

static volatile unsigned char bss[9000];
static volatile int data = 123;
static char long_name[13] = "12345678.ELFX";

void _start(void)
{
	unsigned int i;

	for (i = 0; i < sizeof(bss); i++)
		if (bss[i] != 0) {
			printf("BSS FAIL\n");
			exit(1);
		}
	if (data != 123 || exec((char *)0) != -1 ||
	    exec((char *)0x80000000) != -1 ||
	    exec((char *)0x10000000) != -1 ||
	    exec(long_name) != -1 || exec("missing") != -1 ||
	    exec("bad.elf") != -1 || syscall1(SYSCALL_EXEC, 1) != -1) {
		printf("EXEC FAIL\n");
		exit(1);
	}
	printf("ELF data/BSS and failed exec preserved caller\n");
	exit(0);
}
