#include <lib/stdio.h>

static void console_character(int ch, void *context)
{
	void (**putch)(int) = context;
	(*putch)(ch);
}

int printf_base(void (*putch)(int), const char *format, ...)
{
	int length;
	va_list ap;

	va_start(ap, format);
	length = vprintf_base(console_character, &putch, format, ap);
	va_end(ap);
	return length;
}
