/*
===============================================================================
AF_KEY_H definitions

Definition for the input action struct
===============================================================================
*/
#ifndef AF_KEY_HEADER
#define AF_KEY_HEADER

/*
====================
AF_Key 
Key struct to be used with input system
====================
*/
typedef struct {
	char code;
	unsigned pressed;
    unsigned held;
} AF_Key;

#endif // AF_KEY_H


