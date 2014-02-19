#include <monitor.h>
#include <console.h>
#include <debug.h>
#include <lib/stdio.h>
#include <system.h>
#include <types.h>

static void print_arguments(struct DebugInfo *info, int *fp);

/* mon_backtrace prints the current stack backtrace. */
int mon_backtrace(int argc, char **argv)
{
	int *fp = NULL;
	int pc = 0;

	(void) argc;
	(void) argv;
	
	fp = read_fp();
	pc = fp[0];

	while (fp != NULL) {
		int lr = fp[-1];
		struct DebugInfo info = get_debug_info(pc);

		printf("  fp: %x, lr: %x\n", fp, lr);
		printf("    %s:%s:%d\n", info.file, info.function,
			info.source_line_number);

		if (info.arg_count != 0)
			print_arguments(&info, fp);

		pc = lr;
		fp = (int *) fp[-3];
	}

	return 0;
}

static void print_arguments(struct DebugInfo *info, int *fp)
{
	int arg_index = 0;

	printf("    arguments: ");
	for (arg_index = 0; arg_index < info->arg_count; arg_index++) {
		char *arg_name = info->arg_names[arg_index];
		int arg_position = info->arg_positions[arg_index];
		int arg_value = fp[arg_position / 4];

		printf("%s=%d ", arg_name, arg_value);
	}

	printf("\n");
}
