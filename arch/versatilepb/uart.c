#include <console.h>
#include <vm.h>

/* memory mapping for the serial port */
#define UART0 MMIO_P2V(0x101f1000)

/* serial port register offsets */
#define UART_DATA        0x00 
#define UART_FLAGS       0x06
#define UART_INT_ENABLE  0x0e
#define UART_INT_TARGET  0x0f
#define UART_INT_CLEAR   0x11

/* serial port bitmasks */
#define UART_RECEIVE  0x10
#define UART_TRANSMIT 0x20

void uart_init(void) {
	UART0[UART_INT_ENABLE] = UART_RECEIVE;
}


void uart_transmit(char c) {
	/* wait until transmit buffer is full */
	while (UART0[UART_FLAGS] & UART_TRANSMIT);

	/* write the character */
	UART0[UART_DATA] = c;
}

bool uart_can_receive(void) {
	return UART0[UART_INT_TARGET] & UART_RECEIVE;
}

int uart_receive(void) {
	return UART0[UART_DATA];
}

