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


int main(void) {
    ASSERT_EQ(mapInt(0, 0, 100, 0, 10), 0);
    ASSERT_EQ(mapInt(50, 0, 100, 0, 10), 5);
    ASSERT_EQ(mapInt(100, 0, 100, 0, 10), 10);
    ASSERT_EQ(mapInt(150, 0, 100, 0, 10), 10);  // Clamped
    ASSERT_EQ(mapInt(-1, 0, 100, 0, 10), 0);    // Clamped
    return 0;
}
