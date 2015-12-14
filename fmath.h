// fmath.h

#ifndef _FIXED_MATH_h
#define _FIXED_MATH_h


#define FIX16		(65536)

#define S8			(8)
#define S16			(16)		
#define CHAR_BIT	(0x08)
#define FIX_ONE		(0x00010000)
#define FIX_TWO		(0x00020000)

int32_t fmath_float_to_fixed(float x);
float	fmath_fixed_to_float(int32_t x);
int32_t fmath_fixed_add(int32_t x1, int32_t x2);
int32_t fmath_fixed_mul(int32_t x1, int32_t x2);
int32_t fmath_fixed_mul_fast(int32_t x1, int32_t x2);
int32_t fmath_fixed_div(int32_t x1, int32_t x2);
int32_t fmath_fixed_atan2(int32_t inY, int32_t inX);
int32_t fmath_fixed_sqrt(int32_t inValue);
int32_t fmath_fixed_asin(int32_t inValue);
int32_t fmath_fixed_atan(int32_t inValue);

#endif

