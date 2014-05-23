#include <memory.h>

.global entry
entry:
	ldr sp, =kernel_stack_start

	bl enable_paging
	bl copy_interrupt_table
	bl use_high_interrupts
	bl enable_interrupts
	bl setup_irq_stack_pointer

	bl jump_to_high_mem
	bl c_entry

enable_paging:
	mov r2, lr

	mov r0, #1
	bl set_domain_access_control

	ldr r0, =startup_sectiontable
	sub r0, #KERNEL_BASE
	bl set_translation_table_base

	bl read_control_register
	orr r0, #1
	bl set_control_register

	mov pc, r2

copy_interrupt_table:
	mov r0, #0x0
	ldr r1, =interrupt_table_start
	ldr r3, =interrupt_table_end
	
	copy_loop_start:
		ldr r2, [r1, #0x0]
		str r2, [r0, #0x0]
		add r0, r0, #0x4
		add r1, r1, #0x4
		cmp r1, r3
		bne copy_loop_start

	mov pc, lr

setup_irq_stack_pointer:
	mrs r0, cpsr
	bic r1, r0, #0x1F
	orr r1, r1, #0x12  
	msr cpsr, r1  
	ldr sp, =irq_stack_start
	msr cpsr, r0

	mov pc, lr

interrupt_table_start:
	nop
	subs pc, lr, #4
	subs pc, lr, #4
	subs pc, lr, #4
	subs pc, lr, #4
	subs pc, lr, #4
	ldr pc, irq_entry_address
	irq_entry_address: .word irq_entry
interrupt_table_end:

irq_entry: 
	push {r0-r12, lr}
	bl dispatch_interrupts
	pop {r0-r12, lr}

	subs pc, lr, #4
