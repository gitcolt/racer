#include "../mymath.c"

#include "minunit.h"

#undef MINUNIT_EPSILON
#define MINUNIT_EPSILON 1E-4

#define ASSERT_VEC_EQ(vec_a, vec_b) \
    mu_assert_double_eq((vec_a.x), (vec_b.x)); \
    mu_assert_double_eq((vec_a.y), (vec_b.y))

MU_TEST(test_v2_add) {
    V2 vec_a = { 10, 20 };
    V2 original_vec_a = vec_a;
    V2 vec_b = { 30, 40 };
    V2 original_vec_b = vec_b;

    V2 res = v2_add(vec_a, vec_b);

    V2 expected = { 40, 60 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_vec_a, vec_a);
    ASSERT_VEC_EQ(original_vec_b, vec_b);
}

MU_TEST(test_v2_add_scalar) {
    V2 vec = { 5, 10 };
    V2 original_vec = vec;
    float scale = 3;

    V2 res = v2_add_scalar(vec, scale);

    V2 expected = { 8, 13 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operand was not modified
    ASSERT_VEC_EQ(original_vec, vec);
}

MU_TEST(test_v2_sub) {
    V2 vec_a = { 15, 12 };
    V2 original_vec_a = vec_a;
    V2 vec_b = { 5, 2 };
    V2 original_vec_b = vec_b;

    V2 res = v2_sub(vec_a, vec_b);

    V2 expected = { 10, 10 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_vec_a, vec_a);
    ASSERT_VEC_EQ(original_vec_b, vec_b);
}

MU_TEST(test_v2_sub_scalar) {
    V2 vec = { 7, 4 };
    V2 original_vec = vec;
    float scale = 2;

    V2 res = v2_sub_scalar(vec, scale);

    V2 expected = { 5, 2 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operand was not modified
    ASSERT_VEC_EQ(original_vec, vec);
}

MU_TEST(test_v2_normalize) {
    V2 vec = { 55, 123 };
    V2 original_vec = vec;

    V2 res = v2_normalize(vec);

    V2 expected = { 0.408203, 0.912891 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operand was not modified
    ASSERT_VEC_EQ(original_vec, vec);
}

MU_TEST(test_v2_mul_scalar) {
    V2 vec = { 3, 4 };
    V2 original_vec = { 3, 4 };
    float scale = 5;

    V2 res = v2_mul_scalar(vec, scale);

    V2 expected = { 15, 20 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operand was not modified
    ASSERT_VEC_EQ(original_vec, vec);
}

MU_TEST(test_v2_div_scalar) {
    V2 vec = { 3, 4 };
    V2 original_vec = { 3, 4 };
    float scale = 5;

    V2 res = v2_div_scalar(vec, scale);

    V2 expected = { 1.6, 0.8 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operand was not modified
    ASSERT_VEC_EQ(original_vec, vec);
}

MU_TEST(test_v2_dot) {
    V2 vec_a = { 1, 2 };
    V2 original_vec_a = vec_a;
    V2 vec_b = { 3, 4 };
    V2 original_vec_b = vec_b;

    float res = v2_dot(vec_a, vec_b);

    float expected = 11;
    mu_assert_double_eq(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_vec_a, vec_a);
    ASSERT_VEC_EQ(original_vec_b, vec_b);
}

MU_TEST(test_v2_rot_90_ccw) {
    V2 p = { 3, 4 };
    V2 original_p = p;
    V2 q = { 5, 6 };
    V2 original_q = q;

    V2 res = v2_rot_90_ccw(p, q);

    V2 expected = { 3, 8 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_p, p);
    ASSERT_VEC_EQ(original_q, q);
}

MU_TEST(test_v2_rot_90_cw) {
    V2 p = { 3, 4 };
    V2 original_p = p;
    V2 q = { 5, 6 };
    V2 original_q = q;

    V2 res = v2_rot_90_cw(p, q);

    V2 expected = { 7, 4 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_p, p);
    ASSERT_VEC_EQ(original_q, q);
}

MU_TEST(test_v2_rot_180) {
    V2 p = { 6, 4 };
    V2 original_p = p;
    V2 q = { 2, 3 };
    V2 original_q = q;

    V2 res = v2_rot_180(p, q);

    V2 expected = { -2, 2 };
    ASSERT_VEC_EQ(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_p, p);
    ASSERT_VEC_EQ(original_q, q);
}

MU_TEST(test_v2_dist) {
    V2 a = { -5.2, 3.8 };
    V2 original_a = a;
    V2 b = { 8.7, -4.1 };
    V2 original_b = b;

    float res = v2_dist(a, b);

    float expected = 15.9881;
    mu_assert_double_eq(expected, res);
    // Check that the operands were not modified
    ASSERT_VEC_EQ(original_a, a);
    ASSERT_VEC_EQ(original_b, b);
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_v2_add);
    MU_RUN_TEST(test_v2_normalize);
    MU_RUN_TEST(test_v2_mul_scalar);
    MU_RUN_TEST(test_v2_div_scalar);
    MU_RUN_TEST(test_v2_dot);
    MU_RUN_TEST(test_v2_add);
    MU_RUN_TEST(test_v2_add_scalar);
    MU_RUN_TEST(test_v2_sub);
    MU_RUN_TEST(test_v2_sub_scalar);
    MU_RUN_TEST(test_v2_rot_90_cw);
    MU_RUN_TEST(test_v2_rot_90_ccw);
    MU_RUN_TEST(test_v2_rot_180);
    MU_RUN_TEST(test_v2_dist);
}

int main() {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
}
