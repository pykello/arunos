#include <irq.h>

/* memory mapping for the interrupt controller */
#define PIC ((volatile unsigned int*) 0x10140000)

/* interrupt controller register offsets */
#define PIC_STATUS     0x0
#define PIC_INT_ENABLE 0x4

/* number of interrupt lines */
#define IRQ_COUNT 32

/* interrupt handler vector */
interrupt_handler interrupt_handler_vector[IRQ_COUNT] = {0};

/*
 * register_interrupt_handler enables the given interrupt line, and assigns the
 * given handler function to that line.
 */
void register_interrupt_handler(int interrupt_line, interrupt_handler handler)
{
	PIC[PIC_INT_ENABLE] |= (1 << interrupt_line);
	interrupt_handler_vector[interrupt_line] = handler;
}

/*
 * dispatch_interrupts is called when an interrupt occurs. This function finds
 * the line in which interrupt happened and then triggers the correct handler
 * for that interrupt. In case of no handler for that interrupt, this function
 * does nothing.
 */
void dispatch_interrupts(void)
{
	interrupt_handler handler = 0;

	unsigned int irq_status = PIC[PIC_STATUS];
	int irq_number = 0;
	while (irq_status >>= 1)
		irq_number++;

	handler = interrupt_handler_vector[irq_number];
	if (handler != 0)
		handler();
}
