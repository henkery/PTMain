#include <stdint.h>
#include "fmath.h"

static const int32_t PI_DIV_4 = 0x0000C90F;             /*!< Fix16 value of PI/4 */
static const int32_t THREE_PI_DIV_4 = 0x00025B2F;       /*!< Fix16 value of 3PI/4 */

int32_t fmath_float_to_fixed(float x){
	int32_t rslt = (int32_t) ((x) * FIX16);
	return rslt;
}

float fmath_fixed_to_float(int32_t x){
	float rslt = (((float) (x)) / FIX16);
	return rslt;
}

int32_t fmath_fixed_add(int32_t x1, int32_t x2){
	return (x1 + x2);
}

int32_t fmath_fixed_mul(int32_t x1, int32_t x2){
	//x1 = x1 >> S8;
	//x2 = x2 >> S8;
	int32_t rslt = (((int64_t) x1 * x2) / FIX16);
	return rslt;
	//return x1 * x2;
}

int32_t fmath_fixed_mul_fast(int32_t x1, int32_t x2){
	long x1i, x1f, x2i, x2f;
	
	x1i = x1 >> S16;
	x1f = x1 & 0xFFFF;
	x2i = x2 >> S16;
	x2f = x2 & 0xFFFF;
	
	return (((x1f * x2f) >> S16) + (x1i * x2) + (x1f * x2i));
}

int32_t fmath_fixed_div(int32_t x1, int32_t x2){
	int32_t rslt = (((int64_t) x1 * FIX16) / x2);
	return rslt;
}

int32_t fmath_fixed_atan2(int32_t inY , int32_t inX) {
	int32_t abs_inY, mask, angle, r, r_3;


	/* Absolute inY */
	mask = (inY >> (sizeof(int32_t)*CHAR_BIT-1));
	abs_inY = (inY + mask) ^ mask;

	if (inX >= 0)
	{
		r = fmath_fixed_div( (inX - abs_inY), (inX + abs_inY));
		r_3 = fmath_fixed_mul(fmath_fixed_mul(r, r),r);
		angle = fmath_fixed_mul(0x00003240 , r_3) - fmath_fixed_mul(0x0000FB50,r) + PI_DIV_4;
	} else {
		r = fmath_fixed_div( (inX + abs_inY), (abs_inY - inX));
		r_3 = fmath_fixed_mul(fmath_fixed_mul(r, r),r);
		angle = fmath_fixed_mul(0x00003240 , r_3) - fmath_fixed_mul(0x0000FB50,r) + THREE_PI_DIV_4;
	}
	if (inY < 0)
	{
		angle = -angle;
	}

	return angle;
}

int32_t fmath_fixed_sqrt(int32_t inValue) {
	uint8_t neg = (inValue < 0);
	uint32_t num = (neg ? -inValue : inValue);
	uint32_t result = 0;
	uint32_t bit;
	uint8_t n;
	
	// Many numbers will be less than 15, so
	// this gives a good balance between time spent
	// in if vs. time spent in the while loop
	// when searching for the starting value.
	if (num & 0xFFF00000)
	bit = (uint32_t)1 << 30;
	else
	bit = (uint32_t)1 << 18;
	
	while (bit > num) bit >>= 2;
	
	// The main part is executed twice, in order to avoid
	// using 64 bit values in computations.
	for (n = 0; n < 2; n++)
	{
		// First we get the top 24 bits of the answer.
		while (bit)
		{
			if (num >= result + bit)
			{
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else
			{
				result = (result >> 1);
			}
			bit >>= 2;
		}
		
		if (n == 0)
		{
			// Then process it again to get the lowest 8 bits.
			if (num > 65535)
			{
				// The remainder 'num' is too large to be shifted left
				// by 16, so we have to add 1 to result manually and
				// adjust 'num' accordingly.
				// num = a - (result + 0.5)^2
				//   = num + result^2 - (result + 0.5)^2
				//   = num - result - 0.5
				num -= result;
				num = (num << 16) - 0x8000;
				result = (result << 16) + 0x8000;
			}
			else
			{
				num <<= 16;
				result <<= 16;
			}
			
			bit = 1 << 14;
		}
	}
	
	return (neg ? -result : result);
}

int32_t fmath_fixed_asin(int32_t inValue) {
	if((inValue > FIX_ONE) || (inValue < -FIX_ONE))
		return 0;
	int32_t tempOut;
	tempOut = (FIX_ONE - fmath_fixed_mul(inValue, inValue));
	tempOut = fmath_fixed_div(inValue, fmath_fixed_sqrt(tempOut));
	tempOut = fmath_fixed_atan(tempOut);
	return tempOut;
}

int32_t fmath_fixed_atan(int32_t inValue) {
	return fmath_fixed_atan2(inValue, FIX_ONE);
}