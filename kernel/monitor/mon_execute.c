#include <debug.h>
#include <monitor.h>
#include <console.h>
#include <klib.h>
#include <lib/base16.h>
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
	int len = 0;

	if (argc < 2) {
		kprintf("execute requires at least one argument.\n");
		return -1;
	}

	program_index = argv[1][0] - '0';
	process_b16 = user_programs[program_index];

	len = strlen(process_b16);
	if (len > 2 * PAGE_SIZE) {
		kprintf("execution of programs greater than one page"
			"is not supported yet.\n");
		return -1;
	}

	proc = proc_create();
	process_image = kalloc();
	b16decode(process_b16, len, process_image, &process_len);
	proc_load(proc, process_image, process_image + process_len);

	kfree(process_image);
	proc_switch(proc);

	return 0;
}
