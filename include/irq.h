/*
 * irq.h
 *
 * Type and function declarations for interrupt handling.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef IRQ_H
#define IRQ_H

typedef void (*interrupt_handler)(void);

void irq_init(void);
void register_interrupt_handler(int interrupt_line, interrupt_handler handler);
void dispatch_interrupts(void);

#endif
