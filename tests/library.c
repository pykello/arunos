#include <console.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)

static int input[512], input_pos;
static int read_char(void) { return input[input_pos++]; }
static void discard(int ch) { (void)ch; }

static int test_input(void)
{
	struct { char text[128]; char guard[4]; } data;
	char small[4];
	int i;

	memset(&data, '?', sizeof(data));
	for (i = 0; i < 300; i++) input[i] = 'x';
	input[300] = '\n'; input[301] = 'o'; input[302] = 'k';
	input[303] = '\n'; input_pos = 0;
	CHECK(gets_base(read_char, discard, data.text, sizeof(data.text)));
	CHECK(strlen(data.text) == 127 && data.guard[0] == '?');
	CHECK(input_pos == 301);
	CHECK(gets_base(read_char, discard, small, sizeof(small)));
	CHECK(strcmp(small, "ok") == 0);
	CHECK(!gets_base(read_char, discard, NULL, 0));
	CHECK(!gets_base(read_char, discard, small, 0));
	CHECK(input_pos == 304);
	input_pos = 0; input[0] = 'a'; input[1] = '\n';
	CHECK(gets_base(read_char, discard, small, 1));
	CHECK(!small[0] && input_pos == 2);
	input_pos = 0; input[0] = 'a'; input[1] = KEY_BACKSPACE;
	input[2] = 'b'; input[3] = '\n';
	CHECK(gets_base(read_char, discard, small, sizeof(small)));
	CHECK(strcmp(small, "b") == 0);
	return 0;
}

int main(void)
{
	return test_input();
}
