.global enable_interrupts
enable_interrupts:
	mrs r1, cpsr
	bic r1, r1, #0x80
	msr cpsr_c, r1

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
