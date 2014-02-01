#include <monitor.h>
#include <console.h>
#include <lib/stdio.h>
#include <system.h>
#include <types.h>

/* mon_backtrace */
int mon_backtrace(int argc, char **argv)
{
	int *fp = NULL;
	int *sp = NULL;

	(void) argc;
	(void) argv;

	printf("Stack backtrace:\n");
	
	fp = read_fp();
	sp = read_sp();

	while (fp != NULL) {
		int lr = fp[-1];
		int arg1 = sp[3];
		int arg2 = sp[2];
		int arg3 = sp[1];
		int arg4 = sp[0];

		printf("  fp: %x, lr: %x, args: %x %x %x %x \n",
		       fp, lr, arg1, arg2, arg3, arg4);

		fp = (int *) fp[-3];
		sp = (int *) fp[-2];
	}

	return 0;
}
