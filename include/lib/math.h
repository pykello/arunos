/*
 * lib/math.h
 *
 * Type and function declarations for mathematical computations.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef MATH_H
#define MATH_H

#include <types.h>

uint32_t unsigned_divmod(uint32_t numerator, uint32_t denominator,
			 uint32_t* remainder_pointer);

#endif
