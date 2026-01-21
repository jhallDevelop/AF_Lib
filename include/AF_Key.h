/*
===============================================================================
AF_KEY_H definitions

Definition for the input action struct
===============================================================================
*/
#ifndef AF_KEY_HEADER
#define AF_KEY_HEADER
#include <stdint.h>

/*
====================
AF_Key 
Key struct to be used with input system
====================
*/
typedef struct {
	int32_t code;
	uint32_t pressed;
    uint32_t held;
} AF_Key;

#endif // AF_KEY_H


