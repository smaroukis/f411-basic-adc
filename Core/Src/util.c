/*
 * util.c (utilities)
 *
 *  Created on: May 23, 2025
 *      Author: s
 */

#ifndef TEST_HOST
// as needed for testing only e.g.
// #include "stm32f4xx_hal.h"
// use flag with make e.g. CFLAGS = -DTEST_HOST
#endif



/* Utility Functions */
// Exposed Global for testing see root/Host_Tests
int mapInt(int x, int inMin, int inMax, int outMin, int outMax) {
	if (x < inMin) x = inMin;
	if (x > inMax) x = inMax;
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
