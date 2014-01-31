#include "monitor.h"

#include "console.h"
#include "lib/stdio.h"
#include "types.h"


/* mon_help prints the list of available commands in the monitor. */
int
mon_help(int argc, char **argv)
{
	uint32_t command_index = 0;

	(void) argc;
	(void) argv;

	for (command_index = 0; command_index < COMMAND_COUNT; command_index++)
	{
		Command command = commands[command_index];
		printf("%s - %s\n", command.name, command.description);
	}

	return 0;
}
