#include <lib/stdio.h>
#include <lib/math.h>
#include <types.h>

/* digits */
#define DIGITS "0123456789abcdef"

/* forward declarations for local functions */
static int print_string(char *target, const char *str);
static int print_int(char *target, int number);
static int print_uint_in_base(char *target, uint32_t number, uint32_t base);

/*
 * vsprintf formats the given data and returns the result in the given target
 * character pointer. Following format flags are currently supported:
 * 
 *   - %s: strings,
 *   - %c: characters,
 *   - %d: signed integers,
 *   - %u: unsigned integers,
 *   - %x: hexadecimal representation of integers.
 */
int vsprintf(char *target, const char *format, va_list ap)
{
	int format_index = 0;
	int target_index = 0;

	while (format[format_index] != 0) {
		char format_flag = 0;

		while (format[format_index] != '%' &&
		       format[format_index] != '\0')
		{
			target[target_index] = format[format_index];
			target_index++;
			format_index++;
		}

		if (format[format_index] == 0 || format[format_index + 1] == 0)
			break;

		format_flag = format[format_index + 1];
		switch (format_flag) {
		/* string */
		case 's':
		{
			const char *string_arg = va_arg(ap, char *);
			target_index += print_string(target + target_index,
						     string_arg);

			break;
		}
		/* char */
		case 'c':
		{
			target[target_index] = (char) va_arg(ap, int);
			target_index++;

			break;
		}
		/* int */
		case 'd':
		{
			int int_arg = va_arg(ap, int);
			target_index += print_int(target + target_index,
						  int_arg);

			break;
		}
		/* unsigned int */
		case 'u':
		{
			uint32_t uint_arg = va_arg(ap, uint32_t);
			target_index += print_uint_in_base(target + target_index,
							   uint_arg, 10);

			break;
		}
		/* hexadecimal */
		case 'x':
		{
			int uint_arg = va_arg(ap, uint32_t);
			target_index += print_string(target + target_index, "0x");
			target_index += print_uint_in_base(target + target_index,
							   uint_arg, 16);

			break;
		}
		}

		/* skip % and format_flag */
		format_index += 2;
	}

	target[target_index] = 0;

	return target_index;
}

static int print_string(char *target, const char *str)
{
	int target_index = 0;
	int str_index = 0;
	while (str[str_index] != 0) {
		target[target_index] = str[str_index];
		target_index++;
		str_index++;
	}

	return str_index;
}

static int print_int(char *target, int number)
{
	int length = 0;
	if (number < 0) {
		target[0] = '-';
		length = 1;
		length += print_uint_in_base(target + 1, -number, 10);
	} else {
		length = print_uint_in_base(target, number, 10);
	}

	return length;
}

static int print_uint_in_base(char *target, uint32_t number, uint32_t base)
{
	int length = 0;
	uint32_t last_digit = 0;
	uint32_t rest = 0;

	rest = unsigned_divmod(number, base, &last_digit);
	if (rest != 0)
		length = print_uint_in_base(target, rest, base);

	target[length] = DIGITS[last_digit];
	length++;

	return length;
}
