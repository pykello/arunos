#include <lib/base16.h>

void b16encode(const char *input, int input_len, char *output, int *output_len)
{
	int i;
	for (i = 0; i < input_len; i++) {
		output[2 * i] = (input[i] & 0xF) + 'A';
		output[2 * i + 1] = ((input[i] >> 4) & 0xF) + 'A';
	}

	*output_len = 2 * input_len;
}

void b16decode(const char *input, int input_len, char *output, int *output_len)
{
	int i;
	for (i = 0; i < input_len / 2; i++) {
		int low = input[2 * i] - 'A';
		int high = input[2 * i + 1] - 'A';
		output[i] = low | (high << 4);
	}

	*output_len = input_len / 2;
}
