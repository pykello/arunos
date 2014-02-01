#include <monitor.h>
#include <console.h>
#include <lib/stdio.h>
#include <types.h>

/* mon_help prints the list of available commands in the monitor. */
int mon_help(int argc, char **argv)
{
	uint32_t i = 0;

	(void) argc;
	(void) argv;

	for (i = 0; i < COMMAND_COUNT; i++)
		printf("%s - %s\n", commands[i].name, commands[i].description);

	return 0;
}
