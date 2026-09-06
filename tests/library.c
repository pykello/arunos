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

static char printed[1024];
static int printed_size;
static void capture(int ch) { printed[printed_size++] = ch; }

static int test_format(void)
{
	char long_string[601], formatted[80];
	memset(long_string, 'x', 600); long_string[600] = 0;
	CHECK(printf_base(capture, "%s", long_string) == 600);
	CHECK(printed_size == 600);
	for (int i = 0; i < 600; i++) CHECK(printed[i] == 'x');
	printed_size = 0;
	CHECK(sprintf(formatted, "%d %u %x %c %%", (-2147483647 - 1),
	              4294967295u, 0xabu, 'z') == 31);
	CHECK(strcmp(formatted, "-2147483648 4294967295 0xab z %") == 0);
	CHECK(printf_base(capture, "a%cb", 0) == 3);
	CHECK(printed_size == 3 && !printed[1] && printed[2] == 'b');
	return 0;
}

static int test_tokens(void)
{
	char text[] = "  help  next, last  ";
	char empty[] = "", delimiters[] = " , ", single[] = "one";
	CHECK(!strtok(NULL, " ,"));
	CHECK(strcmp(strtok(text, " ,"), "help") == 0);
	CHECK(strcmp(strtok(NULL, " ,"), "next") == 0);
	CHECK(strcmp(strtok(NULL, " ,"), "last") == 0);
	CHECK(!strtok(NULL, " ,") && !strtok(NULL, " ,"));
	CHECK(!strtok(empty, " ,") && !strtok(NULL, " ,"));
	CHECK(!strtok(delimiters, " ,") && !strtok(NULL, " ,"));
	CHECK(strcmp(strtok(single, " ,"), "one") == 0);
	CHECK(!strtok(NULL, " ,") && !strtok(NULL, " ,"));
	return 0;
}

static int test_copy(void)
{
	char target[4] = "xxx";
	CHECK(strlcpy(target, "hello", 0) == 5 && target[0] == 'x');
	CHECK(strlcpy(NULL, "hello", 0) == 5);
	CHECK(strlcpy(target, "hello", 1) == 5 && !target[0]);
	CHECK(strlcpy(target, "hello", 4) == 5);
	CHECK(strcmp(target, "hel") == 0);
	CHECK(strlcpy(target, "abc", 4) == 3);
	CHECK(strcmp(target, "abc") == 0);
	CHECK(strlcpy(target, "", 4) == 0 && !target[0]);
	return 0;
}

int main(void)
{
	int result = test_input();
	if (!result) result = test_format();
	if (!result) result = test_tokens();
	return result ? result : test_copy();
}
