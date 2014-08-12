#include <console.h>
#include <lib/stdio.h>
#include <lib/syscall.h>
#include <types.h>

/*
 * gets_base reads a line from the input and puts it as a null terminated
 * string in the given buffer. It returns the pointer buffer as the return value.
 */
char *
gets_base(int (*getch)(void), void (*putch)(int), char *buffer)
{
	int buffer_index = 0;
	int next_char = 0;
	while (next_char != '\r' && next_char != '\n') {
		next_char = getch();
		if (next_char == 0)
			continue;

		if (next_char == '\r' || next_char == '\n')
			break;

		if (next_char == KEY_BACKSPACE) {
			if (buffer_index != 0) {
				putch(KEY_ARROW_LEFT);
				putch(' ');
				putch(KEY_ARROW_LEFT);
				buffer_index--;
			}
		}
		else if (next_char >= 32 && next_char < 128) {
			putch(next_char);

			buffer[buffer_index] = next_char;
			buffer_index++;
		}
	}

	putch('\n');
	buffer[buffer_index] = 0;

	return buffer;
}
