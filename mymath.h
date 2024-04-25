#ifndef MYMATH_H
#define MYMATH_H

#include "shared.h"

#define MIN(a, b) \
    (((a) < (b)) ? (a) : (b))

#define MAX(a, b) \
    (((a) > (b)) ? (a) : (b))

V2 v2_add(V2 va, V2 vb);
V2 v2_add_scalar(V2 va, float scalar);
V2 v2_sub(V2 va, V2 vb);
V2 v2_sub_scalar(V2 va, float scalar);
V2 v2_mul_scalar(V2 v, float scalar);
V2 v2_div_scalar(V2 v, float scalar);
V2 v2_normalize(V2 v);
float v2_dot(V2 va, V2 vb);
V2 v2_rot_90_cw(V2 p, V2 q);
V2 v2_rot_90_ccw(V2 p, V2 q);
V2 v2_rot_180(V2 p, V2 q);
float v2_dist(V2 va, V2 vb);

#endif
