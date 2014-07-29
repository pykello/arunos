#define KERNEL_BASE 0x80000000
#define UART0_BASE (KERNEL_BASE + 0x60000000)
#define UART0 ((volatile unsigned int*) UART0_BASE)
#define UART_DATA        0x00 
#define UART_FLAGS       0x06
#define UART_TRANSMIT 0x20

int _start()
{
	int i = 0;
	char c[] = "hello world";
	while (c[i])
		UART0[UART_DATA] = c[i], i++;

	UART0[UART_DATA] = '\n';

	while (1);

	return 0;
}
