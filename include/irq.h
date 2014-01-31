#ifndef IRQ_H
#define IRQ_H

typedef void (*interrupt_handler)(void);

extern void irq_init(void);
extern void register_interrupt_handler(int interrupt_line, interrupt_handler handler);
extern void dispatch_interrupts(void);

#endif
