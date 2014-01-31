#ifndef MONITOR_H
#define MONITOR_H


/* command handler and command types */
typedef void (*command_handler)(int argc, char **argv);

typedef struct Command
{
	const char *name;
	const char *description;
	command_handler handler;
} Command;


/* defines */
#define ARG_COUNT_MAX 10


/* monitor entry point */
extern void monitor(void);

/* monitor command function declarations */
extern int mon_help(int argc, char **argv);
extern int mon_hextee(int argc, char **argv);
extern int mon_kerninfo(int argc, char **argv);

/* monitor commands declarations */
static Command const commands[] = {
	{
		"help", 
		"Displays this list of commands", 
		(command_handler) mon_help
	},
	{
		"hextee",
		"Reads from input and outputs the hexadecimal codes",
		(command_handler) mon_hextee
	},
	{
		"kerninfo",
		"Displays information about the kernel", 
		(command_handler) mon_kerninfo
	},
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))


#endif