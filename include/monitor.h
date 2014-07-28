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
typedef void (*command_handler)(int argc, char **argv);

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
		(command_handler) mon_help
	}, {
		"backtrace",
		"Displays the current stack backtrace",
		(command_handler) mon_backtrace
	}, {
		"hextee",
		"Reads from input and outputs the hexadecimal codes",
		(command_handler) mon_hextee
	}, {
		"kerninfo",
		"Displays information about the kernel", 
		(command_handler) mon_kerninfo
	}, {
		"status",
		"Displays current status of the system", 
		(command_handler) mon_status
	}, {
		"execute",
		"Executes a user program",
		(command_handler) mon_execute
	}
};

#define COMMAND_COUNT 6

#endif
