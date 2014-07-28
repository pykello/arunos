/*
 * debug.h
 *
 * Type and function declarations for debug information.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef DEBUG_H
#define DEBUG_H

#define NAME_MAX_LENGTH 128
#define ARG_MAX_COUNT 4

struct DebugInfo {
	char file[NAME_MAX_LENGTH];
	char function[NAME_MAX_LENGTH];
	int function_address;
	int arg_count;
	char arg_names[ARG_MAX_COUNT][NAME_MAX_LENGTH];
	int arg_positions[ARG_MAX_COUNT];
	int source_line_number;
	int source_line_address;
};

/*
 * Format of entries in ".stab" section.
 * Source: https://sourceware.org/gdb/onlinedocs/stabs.html#Symbol-Table-Format
 */
struct SymbolTableEntry {
	unsigned long n_strx;         /* index into string table of name */
	unsigned char n_type;         /* type of symbol */
	unsigned char n_other;        /* misc info (usually empty) */
	unsigned short n_desc;        /* description field */
	unsigned int n_value;         /* value of symbol */
};

/* constants for SymbolTableEntry->n_type */
#define SYMBOL_FUNCTION       0x24
#define SYMBOL_SOURCE_LINE    0x44
#define SYMBOL_SOURCE_FILE    0x64
#define SYMBOL_PARAMETER      0xa0

/*
 * Pointers to debug info in ".stab" and ".stabstr" sections. To see where these
 * pointers are set, see the linker script with .ld extension and also the -gstab
 * compiler flag.
 */
extern const struct SymbolTableEntry STAB_BEGIN[];
extern const struct SymbolTableEntry STAB_END[];
extern const char STABSTR_BEGIN[];
extern const char STABSTR_END[];

struct DebugInfo get_debug_info(int pc);
void get_function_bounds(const char *name, int *begin, int *end);

#endif
