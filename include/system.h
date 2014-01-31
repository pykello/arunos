#ifndef SYSTEM_H
#define SYSTEM_H


typedef enum CPUModeType 
{
	CPU_MODE_USER       = 0x10,
	CPU_MODE_FIQ        = 0x11,
	CPU_MODE_IRQ        = 0x12,
	CPU_MODE_SUPERVISOR = 0x13,
	CPU_MODE_ABORT      = 0x17,
	CPU_MODE_UNDEFINED  = 0x1b,
	CPU_MODE_SYSTEM     = 0x1f

} CPUModeType;


extern void enable_interrupts(void);
extern char *get_stack_pointer(CPUModeType cpu_mode);
extern int read_cpsr(void);


#endif
