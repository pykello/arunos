#include <lib/stdio.h>
#include <lib/math.h>

struct output {
	void (*emit)(int, void *);
	void *context;
	int length;
};

static void character(struct output *out, int ch)
{
	out->emit(ch, out->context);
	out->length++;
}

static void number(struct output *out, uint32_t value, uint32_t base)
{
	uint32_t digit;
	uint32_t rest = unsigned_divmod(value, base, &digit);
	if (rest) number(out, rest, base);
	character(out, "0123456789abcdef"[digit]);
}

/* Share the formatter between streaming console and caller-owned buffers. */
int vprintf_base(void (*emit)(int, void *), void *context,
		 const char *format, va_list ap)
{
	struct output out = { emit, context, 0 };
	while (*format) {
		if (*format != '%') {
			character(&out, *format++);
			continue;
		}
		if (!*++format) break;
		switch (*format++) {
		case 's': {
			const char *str = va_arg(ap, const char *);
			while (*str) character(&out, *str++);
			break;
		}
		case 'c': character(&out, va_arg(ap, int)); break;
		case '%': character(&out, '%'); break;
		case 'd': {
			int value = va_arg(ap, int);
			if (value < 0) character(&out, '-');
			number(&out, value < 0 ? 0u - (uint32_t)value :
			       (uint32_t)value, 10);
			break;
		}
		case 'u': number(&out, va_arg(ap, uint32_t), 10); break;
		case 'x':
			character(&out, '0'); character(&out, 'x');
			number(&out, va_arg(ap, uint32_t), 16);
			break;
		}
	}
	return out.length;
}

static void buffer_character(int ch, void *context)
{
	char **cursor = context;
	*(*cursor)++ = ch;
}

/* As with sprintf, the caller must provide enough space for the result. */
int vsprintf(char *target, const char *format, va_list ap)
{
	int length = vprintf_base(buffer_character, &target, format, ap);
	*target = '\0';
	return length;
}
