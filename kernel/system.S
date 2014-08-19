#include <vm.h>

.global enable_interrupts
enable_interrupts:
	mrs r1, cpsr
	bic r1, r1, #0x80
	msr cpsr_c, r1

	mov pc, lr

.global use_high_interrupts
use_high_interrupts:
	push {lr}
	bl read_control_register
	orr r0, #0x2000
	bl set_control_register
	pop {lr}

	mov pc, lr

/* get_stack_pointer(cpu_mode) */
.global get_stack_pointer
get_stack_pointer:
	mov r3, r0
	mrs r1, cpsr
	bic r2, r1, #0x1F
	orr r2, r2, r3
	msr cpsr, r2
	mov r0, sp
	msr cpsr, r1

	mov pc, lr

.global read_cpsr
read_cpsr:
	mrs r0, cpsr
	mov pc, lr

.global read_fp
read_fp:
	mov r0, fp
	mov pc, lr

.global read_cpu_id
read_cpu_id:
	mrc p15, 0, r0, cr0, cr0, 0
	mov pc, lr

.global read_cache_type
read_cache_type:
	mrc p15, 0, r0, cr0, cr0, 1
	mov pc, lr

.global read_tcm_type
read_tcm_type:
	mrc p15, 0, r0, cr0, cr0, 2
	mov pc, lr

.global read_tlb_type
read_tlb_type:
	mrc p15, 0, r0, cr0, cr0, 3
	mov pc, lr

.global read_control_register
read_control_register:
	mrc p15, 0, r0, cr1, cr0, 0
	mov pc, lr

.global set_control_register
set_control_register:
	mcr p15, 0, r0, cr1, cr0, 0
	mov pc, lr

.global set_domain_access_control
set_domain_access_control:
	mcr p15, 0, r0, cr3, cr0, 0
	mov pc, lr

.global set_translation_table_base
set_translation_table_base:
	mcr p15, 0, r0, cr2, cr0, 0
	mov pc, lr

.global jump_to_high_mem
jump_to_high_mem:
	add lr, #KERNEL_BASE
	mov pc, lr

.global switch_to_context
switch_to_context:
	LDMIA R0!, {R12, R14}
	MSR SPSR_fsxc, R12
	LDMIA R0, {R0-R14}^
	NOP
	MOVS PC, R14
