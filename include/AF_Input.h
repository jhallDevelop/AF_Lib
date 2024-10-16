/*
===============================================================================
AF_INPUT_H definitions

Definition for the AF_Key and AF_Input structs
and helper functions
===============================================================================
*/
#ifndef AF_INPUT_H
#define AF_INPUT_H
#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {
#endif
#define AF_INPUT_KEYS_MAPPED 5


#define PRESSED_MASK 0x80  // Pressed bit mask (8th bit)
#define KEYCODE_MASK 0x7F  // Keycode bit mask (lower 7 bits)

/*
====================
AF_Key 
Key struct to be used with input system
====================
*/
typedef struct {
	char code;
	//int pressed;
} AF_Key;

/*
====================
AF_Input_EncodeKey
Function to encode the key into a pressed state
====================
*/
static inline char AF_Input_EncodeKey(PACKED_CHAR _keyCode, BOOL _isPressed) {
    char returnedChar = _keyCode & KEYCODE_MASK; // Ensure only lower 7 bits are used for keycode

    if (_isPressed) {
        returnedChar |= PRESSED_MASK; // Set the 8th bit if the key is pressed
    }

    return returnedChar;
}



/*
====================
AF_Input_GetKeyCode
Function to decode the key value
====================
*/
static inline PACKED_CHAR AF_Input_GetKeyCode(PACKED_CHAR _encodedKey) {
    return _encodedKey & KEYCODE_MASK;  // Return the lower 7 bits as the keycode
}

/*
====================
AF_Input_IsKeyPressed
Function to check if the key is pressed
====================
*/
static inline BOOL AF_Input_IsKeyPressed(PACKED_CHAR _encodedKey) {
    return (_encodedKey & PRESSED_MASK) != 0;  // Check if the 8th bit is set
}

/*
====================
AF_Input
Input struct to store the registered keys
====================
*/
typedef struct {
    // input buffer que
    AF_Key keys[AF_INPUT_KEYS_MAPPED];

    // Mouse
    //int mouseDown;
    //float mouseX;
    //float mouseY;

} AF_Input;

#ifdef __cplusplus
}
#endif

#endif  // AF_INPUT_H
