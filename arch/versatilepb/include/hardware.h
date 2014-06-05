/*
 * versatilepb/include/hardware.h
 *
 * Definitions for versatilepb board.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef HARDWARE_H
#define HARDWARE_H

/* physical memory */
#define UART0_PHYSICAL 0x101f1000
#define PIC_PHYSICAL 0x10140000
#define GPIO_PHYSICAL 0x10100000

#define UART_IRQ 12

#define TOTAL_MEMORY_SIZE   0x10000000  /* 256MB */

#endif
