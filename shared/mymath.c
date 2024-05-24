#include "mymath.h"

#include <math.h>
#include <stdio.h>

inline V2 v2_mul_scalar(V2 v, float scalar) {
    return (V2){
        .x = scalar * v.x,
        .y = scalar * v.y,
    };
}

inline V2 v2_div_scalar(V2 v, float scalar) {
    return (V2){
        .x = v.x / scalar,
        .y = v.y / scalar,
    };
}

inline V2 v2_add(V2 va, V2 vb) {
    return (V2){
        .x = va.x + vb.x,
        .y = va.y + vb.y,
    };
}

inline V2 v2_add_scalar(V2 va, float scale) {
    return (V2){
        .x = va.x + scale,
        .y = va.y + scale,
    };
}

inline V2 v2_sub(V2 va, V2 vb) {
    return (V2){
        .x = va.x - vb.x,
        .y = va.y - vb.y,
    };
}

inline V2 v2_sub_scalar(V2 va, float scale) {
    return (V2){
        .x = va.x - scale,
        .y = va.y - scale,
    };
}

inline V2 v2_normalize(V2 v) {
    float len = sqrtf((v.x*v.x) + (v.y*v.y));
    return (V2){
        .x = len == 0 ? 0 : v.x / len,
        .y = len == 0 ? 0 : v.y / len,
    };
}

inline V2 v2_rot_90_cw(V2 p, V2 q) {
    return (V2){
        .x = -(p.y - q.y) + q.x,
        .y = (p.x - q.x) + q.y,
    };
}

inline V2 v2_rot_90_ccw(V2 p, V2 q) {
    return (V2){
        .x = (p.y - q.y) + q.x,
        .y = -(p.x - q.x) + q.y,
    };
}

inline V2 v2_rot_180(V2 p, V2 q) {
    return (V2){
        .x = -(p.x - q.x) + q.x,
        .y = -(p.y - q.y) + q.y,
    };
}

inline float v2_dot(V2 va, V2 vb) {
    return (va.x * vb.x) + (va.y * vb.y);
}

inline float v2_dist(V2 va, V2 vb) {
    return sqrtf(pow(vb.x - va.x, 2) + pow(vb.y - va.y, 2));
}
