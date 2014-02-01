.global entry
entry:
	bl copy_interrupt_table
	bl enable_interrupts
	bl setup_irq_stack_pointer
   
	ldr sp, =kernel_stack_start

	/* jump to c program */
	bl c_entry

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
	nop
	nop
	nop
	nop
	nop
	ldr pc, irq_entry_address
	irq_entry_address: .word irq_entry
interrupt_table_end:

irq_entry: 
	push {r0-r12, lr}
	bl dispatch_interrupts
	pop {r0-r12, lr}

	subs pc, lr, #4
