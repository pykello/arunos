#include <lib/stdio.h>
#include <lib/syscall.h>

#define KERNEL_BASE 0x80000000
#define UART0_BASE (KERNEL_BASE + 0x60000000)
#define UART0 ((volatile unsigned int*) UART0_BASE)
#define UART_DATA        0x00 
#define UART_FLAGS       0x06
#define UART_TRANSMIT 0x20

void _start()
{
	int i = 0;
	char c[128];
	sprintf(c, "%s %s %d", "hello", "world", 1);

	while (c[i])
		UART0[UART_DATA] = c[i], i++;

	UART0[UART_DATA] = '\n';

	syscall1(SYSCALL_EXIT, 0);
}
