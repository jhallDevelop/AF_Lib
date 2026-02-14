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

#define AF_PI_d 3.14159265358979323846  // Define the value of Pi as double
#define AF_PI_f 3.1415927f  // Define the value of Pi as float
#define AF_PI_DIV_2_d 1.57079632679489661923 // PI / 2 as double
#define AF_PI_DIV_2_f 1.5707963f // PI / 2 as float
#define AF_PI_DIV_180_d  0.01745329251994329576923690768489 // PI / 180 as double
#define AF_PI_DIV_180_f  0.0174533f // PI / 180 as float
#define AF_180_DIV_PI_d 57.295779513082320876798154814105 // 180 / PI as double
#define AF_180_DIV_PI_f 57.2958f // 180 / PI as a float 
#define AF_PI_MUL_2_d 6.28318530717958647692  // 2 * PI as double
#define AF_PI_MUL_2_f 6.2831853f // 2 * PI as a float
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
