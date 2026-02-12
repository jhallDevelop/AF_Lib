/*
===============================================================================
AF_MATH_DEFINE_H
AUTHOR: jhalldevelop
define extra types e.g. af_bool_t which doesn't exist in c

===============================================================================
*/
#ifndef AF_MATH_DEFINE_H  
#define AF_MATH_DEFINE_H
#include <stdint.h>

#define PI 3.141592653589793
#define AF_PI 3.14159265358979323846  // Define the value of Pi
#define AF_PI_DIV_180  0.01745329251994329576923690768489 // PI / 180
#define AF_180_DIV_PI 57.295779513082320876798154814105 // 180 / PI
#define AF_PI_MUL_2 6.28318530717958647692  // 2 * PI
#define AF_NULL ((void*)0)
// Switch to fixed point math if set at compile time
#if USE_FIXED
    #define AF_FLOAT int8_t //uint16_t
    #define AF_EPSILON 1 << 10
#else
    #define AF_FLOAT float
    #define AF_EPSILON 1e-6
    #define AF_FLOAT_MAX 3.402823466e+38F
#endif	



#endif
