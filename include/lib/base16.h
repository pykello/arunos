/*
 * base16.h
 *
 * Type and function declarations for base16 encoding and decoding.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef BASE16_H
#define BASE16_H

void b16encode(const char *input, int input_len, char *output, int *output_len);
void b16decode(const char *input, int input_len, char *output, int *output_len);

#endif
