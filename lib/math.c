#include <lib/math.h>

/*
 * unsigned_divmod divides numerator and denmoriator, then returns the quotient
 * as result. If remainder_pointer is not NULL, then the function returns the
 * division remainder in remainder_pointer.
 *
 * Algorithm: http://en.wikipedia.org/wiki/Division_algorithm
 */
uint32_t unsigned_divmod(uint32_t numerator, uint32_t denominator,
			 uint32_t* remainder_pointer)
{
	int i = 0;
	uint32_t quotient = 0;
	uint32_t remainder = 0;

	for (i = 31; i >= 0; i--) {
		uint32_t numerator_bit = ((numerator & (1u << i)) ? 1 : 0);
		
		remainder = (remainder << 1) | numerator_bit;
		if (remainder >= denominator) {
			remainder -= denominator;
			quotient |= (1u << i);
		}
	}

	if (remainder_pointer != NULL)
		(*remainder_pointer) = remainder;

	return quotient;
}
