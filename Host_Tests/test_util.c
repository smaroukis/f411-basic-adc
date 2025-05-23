/*
 * test_util.c
 *
 *  Created on: May 23, 2025
 *      Author: s
 *
 *  Usage:
 *      gcc test_util.c util.c -o test_runner
 *      ./test_runner
 */


// DONT "include" util.c, use gen_util_h.py to create on the fly
#include "../Core/Inc/util.h"
#include <stdio.h>
#include <math.h>

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define RESET   "\033[0m"
#define CHECKMARK "✅"
#define CROSSMARK "❌"

#define ASSERT_EQ(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            printf(GREEN CHECKMARK " PASS: %s == %s (%d)\n" RESET, #actual, #expected, (actual)); \
        } else { \
            printf(RED CROSSMARK " FAIL: %s != %s (%d != %d)\n" RESET, #actual, #expected, (actual), (expected)); \
        } \
    } while(0)


#define ASSERT_FLOAT_EQ(actual, expected, tolerance) \
    do { \
        float _a = (actual); \
        float _e = (expected); \
        if (fabsf(_a - _e) <= (tolerance)) { \
            printf(GREEN CHECKMARK " PASS: %s ≈ %s (%.4f ≈ %.4f)\n" RESET, #actual, #expected, _a, _e); \
        } else { \
            printf(RED CROSSMARK " FAIL: %s != %s (%.4f != %.4f)\n" RESET, #actual, #expected, _a, _e); \
        } \
    } while (0)


// Forward declarations for compiler nice-ness
void test_mapFloat(void);
void test_mapInt(void);


int main(void) {
    test_mapFloat();
    test_mapInt();
    return 0;
}


void test_mapFloat(void) {
    printf("\nRunning test_mapFloat:\n");
    ASSERT_FLOAT_EQ(mapFloat(0.0f, 0.0f, 3.3f, 0.0f, 10.0f), 0.0f, 0.01f);
    ASSERT_FLOAT_EQ(mapFloat(1.65f, 0.0f, 3.3f, 0.0f, 10.0f), 5.0f, 0.01f);
    ASSERT_FLOAT_EQ(mapFloat(3.3f, 0.0f, 3.3f, 0.0f, 10.0f), 10.0f, 0.01f);
    ASSERT_FLOAT_EQ(mapFloat(3.3f, 0.0f, 3.3f, 0.0f, 10.0f), 10.0f, 0.01f);
    ASSERT_FLOAT_EQ(mapFloat(2730.67f, 0.0f, 4096.0f, 0.0f, 10.0f), 6.67f, 0.01f);
    ASSERT_FLOAT_EQ(mapFloat(-1.0f, 0.0f, 3.3f, 0.0f, 10.0f), 0.0f, 0.01f);   // Clamped
    ASSERT_FLOAT_EQ(mapFloat(4.0f, 0.0f, 3.3f, 0.0f, 10.0f), 10.0f, 0.01f);   // Clamped
}

void test_mapInt(void) {
    printf("\nRunning test_mapInt:\n");
    ASSERT_EQ(mapInt(0, 0, 100, 0, 10), 0);
    ASSERT_EQ(mapInt(50, 0, 100, 0, 10), 5);
    ASSERT_EQ(mapInt(100, 0, 100, 0, 10), 10);
    ASSERT_EQ(mapInt(2730, 0, 4096, 0, 10), 6);
    ASSERT_EQ(mapInt(150, 0, 100, 0, 10), 10);  // Clamped
    ASSERT_EQ(mapInt(-1, 0, 100, 0, 10), 0);    // Clamped
}

