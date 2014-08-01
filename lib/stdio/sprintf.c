#include <lib/stdio.h>

/*
 * sprintf formats the given data and outputs the result into the given character
 * pointer. See vsprintf for the format flags currently supported.
 */
int
sprintf(char *target, const char *format, ...)
{
	int length = 0;
	va_list ap;

	va_start(ap, format);
	length = vsprintf(target, format, ap);
	va_end(ap);

	return length;
}
