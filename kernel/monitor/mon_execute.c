#include <debug.h>
#include <monitor.h>
#include <console.h>
#include <lib/base16.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <proc.h>
#include <kalloc.h>
#include <system.h>
#include <types.h>

extern const char *user_programs[];

/* mon_execute. */
int mon_execute(int argc, char **argv)
{
	struct Process *proc = NULL;
	char *process_image = NULL;
	const char *process_b16 = NULL;
	int process_len = 0;
	int program_index = 0;
	int index = 0;
	int len = 0;

	if (argc < 2) {
		printf("execute requires at least one argument.\n");
		return -1;
	}

	program_index = argv[1][0] - '0';
	process_b16 = user_programs[program_index];
	process_image = kalloc();

	proc = proc_create();
	len = strlen(process_b16);
	while (index < len) {
		int part_len = 2048;
		if (len - index < part_len) part_len = len - index;

		b16decode(process_b16 + index, part_len,
			  process_image, &process_len);
		proc_load(proc, process_image, process_image + process_len);

		index += part_len;
	}

	kfree(process_image);
	proc_switch(proc);

	return 0;
}
