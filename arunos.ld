ENTRY(entry)

SECTIONS
{
	. = 0x10000;

	.text : { *(.text) }
	PROVIDE(etext = .);

	.stab : {
		PROVIDE(STAB_BEGIN = .);
		*(.stab);
		PROVIDE(STAB_END = .);
		BYTE(0);
	}
	
	.stabstr : {
		PROVIDE(STABSTR_BEGIN = .);
		*(.stabstr);
		PROVIDE(STABSTR_END = .);
		BYTE(0);
	}

	.data : { *(.data) }
	PROVIDE(edata = .);

	.bss : { *(.bss COMMON) }
	. = ALIGN(8);

	. = . + 0x1000; /* 4kB of stack memory */
	kernel_stack_start = .;
	PROVIDE(ekernel_stack_start = .);

	. += 0x1000;
	irq_stack_start = .;

	PROVIDE(end = .);
}
