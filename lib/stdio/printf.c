#include "lib/stdio.h"
#include "console.h"


/*
 * printf formats and prints the given data. See vsprintf() for the format
 * flags currently supported.
 */
int
printf(const char *format, ...)
{
	int length = 0;
	char buffer[BUFFER_MAX_LENGTH];
	int buffer_index = 0;
	va_list ap;

	va_start(ap, format);
	length = vsprintf(buffer, format, ap);
	va_end(ap);

	while (buffer[buffer_index] != '\0')
	{
		putch(buffer[buffer_index]);
		buffer_index++;
	} 

	return length;
}
