#include <debug.h>
#include <lib/string.h>

static void init_debug_info(struct DebugInfo *info);
static void update_debug_info(struct DebugInfo *info,
			      struct SymbolTableEntry *stab_entry);

/*
 * get_debug_info returns the debug information for the case when the instruction
 * at the given program counter is being executed.
 */
struct DebugInfo get_debug_info(int pc)
{
	struct DebugInfo info;
	struct SymbolTableEntry *stab_entry = NULL;

	init_debug_info(&info);

	stab_entry = (struct SymbolTableEntry *) STAB_BEGIN;
	while (stab_entry != STAB_END)
	{
		update_debug_info(&info, stab_entry);
		if (info.source_line_address >= pc)
			break;

		stab_entry++;
	}

	return info;
}

static void init_debug_info(struct DebugInfo *info)
{
	info->file[0] = '\0';
	info->function[0] = '\0';
	info->function_address = 0;
	info->arg_count = 0;
	info->source_line_number = 0;
	info->source_line_address = 0;
}

static void update_debug_info(struct DebugInfo *info,
			      struct SymbolTableEntry *stab_entry)
{
	const char *symbol_str = STABSTR_BEGIN + stab_entry->n_strx;
	char symbol_name[NAME_MAX_LENGTH];

	strlcpy(symbol_name, symbol_str, NAME_MAX_LENGTH);
	strtok(symbol_name, ":");
	
	switch (stab_entry->n_type)
	{
	case SYMBOL_SOURCE_FILE:
		strcpy(info->file, symbol_name);
		break;
	case SYMBOL_FUNCTION:
		strcpy(info->function, symbol_name);
		info->function_address = stab_entry->n_value;
		info->arg_count = 0;
		break;
	case SYMBOL_PARAMETER:
		strcpy(info->arg_names[info->arg_count], symbol_name);
		info->arg_positions[info->arg_count] = stab_entry->n_value;
		info->arg_count++;
		break;
	case SYMBOL_SOURCE_LINE:
		info->source_line_address = info->function_address +
					    stab_entry->n_value;
		info->source_line_number = stab_entry->n_desc - 1;
		break;
	}
}
