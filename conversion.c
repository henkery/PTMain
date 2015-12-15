#include <math.h>
#include <stdint.h>
#include "fmath.h"
#include "mpudefines.h"

void converse_fixed_quat_to_euler(float q[], float v[]){
    int32_t qx = (fmath_float_to_fixed(q[QUATX]));
    int32_t qy = (fmath_float_to_fixed(q[QUATY]));
    int32_t qz = (fmath_float_to_fixed(q[QUATZ]));
    int32_t qw = (fmath_float_to_fixed(q[QUATW]));
    int32_t fpole = 0x00018552;
    int32_t ey = fmath_fixed_asin(fmath_fixed_mul(FIX_TWO , (fmath_fixed_mul(qw, qy) - fmath_fixed_mul(qx, qz))));
    int32_t ex = 0;
    if ((ey < fpole) && (ey > -fpole))
        ex = fmath_fixed_atan2(fmath_fixed_mul(FIX_TWO , ((fmath_fixed_mul(qy , qz) + fmath_fixed_mul(qw , qx)))),
                      FIX_ONE - fmath_fixed_mul(FIX_TWO , (fmath_fixed_mul(qx , qx) + fmath_fixed_mul(qy , qy))));
    
    int32_t ez = fmath_fixed_atan2(fmath_fixed_mul(FIX_TWO , ((fmath_fixed_mul(qx , qy) + fmath_fixed_mul(qw , qz)))),
                      FIX_ONE - fmath_fixed_mul(FIX_TWO , (fmath_fixed_mul(qy , qy) + fmath_fixed_mul(qz , qz))));
                  
    v[VEC3_X] = fmath_fixed_to_float(ex);
    v[VEC3_Y] = fmath_fixed_to_float(ey);
    v[VEC3_Z] = fmath_fixed_to_float(ez);
}

void converse_quaternion_normalize(float q[]){
    float length = sqrt(q[QUATW] * q[QUATW] + q[QUATX] * q[QUATX] +
                        q[QUATY] * q[QUATY] + q[QUATZ] * q[QUATZ]);     
    
    if (length == 0)
        return;
    q[QUATW] /= length;
    q[QUATX] /= length;
    q[QUATY] /= length;
    q[QUATZ] /= length;
}