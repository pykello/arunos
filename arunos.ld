INCLUDE linker.ld

ENTRY(entry)

SECTIONS
{
	. = 0x80000000 + start_address;

	.text : AT(start_address) { *(.text .text.*) }
	PROVIDE(etext = .);

	.rodata : AT(ADDR(.rodata) - 0x80000000) {
		*(.rodata .rodata.*)
	}

	.stab : AT(ADDR(.stab) - 0x80000000) {
		PROVIDE(STAB_BEGIN = .);
		*(.stab);
		PROVIDE(STAB_END = .);
		BYTE(0);
	}
	
	.stabstr : AT(ADDR(.stabstr) - 0x80000000) {
		PROVIDE(STABSTR_BEGIN = .);
		*(.stabstr);
		PROVIDE(STABSTR_END = .);
		BYTE(0);
	}

	.data : AT(ADDR(.data) - 0x80000000) {
		*(.data .data.* .got .got.*)
	}
	PROVIDE(edata = .);

	.bss : AT(ADDR(.bss) - 0x80000000) { *(.bss .bss.* COMMON) }
	. = ALIGN(8);

	. = . + 0x1000; /* 4kB of stack memory */
	kernel_stack_start = .;
	PROVIDE(ekernel_stack_start = .);

	. += 0x1000;
	irq_stack_start = .;

	PROVIDE(kernel_end = .);
}
