/*
 * irq.h
 *
 * Type and function declarations for interrupt handling.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef IRQ_H
#define IRQ_H

#include <types.h>

/* number of interrupt lines */
#define IRQ_COUNT 32

typedef void (*interrupt_handler)(void);

void irq_init(void);
void register_interrupt_handler(int interrupt_line, interrupt_handler handler);
void dispatch_interrupts(void);

/* architecture specific functions */
void enable_irq_line(int line);
void get_pending_irqs(bool *result);

#endif
