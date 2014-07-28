#include <debug.h>
#include <monitor.h>
#include <console.h>
#include <lib/stdio.h>
#include <proc.h>
#include <system.h>
#include <types.h>

#define UART0 ((volatile unsigned int*) UART0_BASE)
#define UART_DATA        0x00 
#define UART_FLAGS       0x06
#define UART_TRANSMIT 0x20

/* mon_execute. */
int mon_execute(int argc, char **argv)
{
	char *function_name = NULL;
	int function_begin = -1;
	int function_end = -1;
	struct Process *proc = NULL;

	if (argc < 2) {
		printf("execute requires at least one argument.\n");
		return -1;
	}

	function_name = argv[1];

	get_function_bounds(function_name, &function_begin, &function_end);

	proc = proc_create();
	proc_load(proc, (void *) function_begin, (void *) function_end);
	proc_switch(proc);

	return 0;
}

void user_hello()
{
	UART0[UART_DATA] = 'h';
	UART0[UART_DATA] = 'e';
	UART0[UART_DATA] = 'l';
	UART0[UART_DATA] = 'l';
	UART0[UART_DATA] = 'o';
	UART0[UART_DATA] = '\n';
	while (1);
}
