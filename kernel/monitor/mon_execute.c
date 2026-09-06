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

/* Execute a root-directory program by filename. */
int mon_execute(int argc, char **argv)
{
	struct Process *proc = NULL;
	bool loaded = false;

	if (argc < 2) {
		kprintf("execute requires at least one argument.\n");
		return -1;
	}

	proc = proc_create();
	loaded = proc_load_program(proc, argv[1]);

	if (loaded)
		schedule();
	else {
		if (proc)
			proc_free(proc);
		kprintf("couldn't load the process.\n");
	}

	return 0;
}
