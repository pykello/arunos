/*
 * monitor.h
 *
 * Type and function declarations for kernel monitor.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef MONITOR_H
#define MONITOR_H

/* command handler and command types */
typedef int (*command_handler)(int argc, char **argv);

struct Command {
	const char *name;
	const char *description;
	command_handler handler;
};

#define ARG_COUNT_MAX 10

/* monitor entry point */
void monitor(void);

/* monitor command function declarations */
int mon_backtrace(int argc, char **argv);
int mon_help(int argc, char **argv);
int mon_hextee(int argc, char **argv);
int mon_kerninfo(int argc, char **argv);
int mon_status(int argc, char **argv);
int mon_execute(int argc, char **argv);

/* monitor commands declarations */
static struct Command const commands[] = {
	{
		"help", 
		"Displays this list of commands", 
		mon_help
	}, {
		"backtrace",
		"Displays the current stack backtrace",
		mon_backtrace
	}, {
		"hextee",
		"Reads from input and outputs the hexadecimal codes",
		mon_hextee
	}, {
		"kerninfo",
		"Displays information about the kernel", 
		mon_kerninfo
	}, {
		"status",
		"Displays current status of the system", 
		mon_status
	}, {
		"execute",
		"Executes a user program",
		mon_execute
	}
};

#define COMMAND_COUNT 6

#endif
