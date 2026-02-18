#include "AF_Input.h"
#include <string.h>

// ====================
// AF_Input_MapKeys
// map keys
// ====================
void AF_Input_MapKeyMappings(AF_Input* _input, const AF_KeyMap* _keymappings){
    
    for(uint32_t i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; i++){
        _input->keys[0][i].code = _keymappings[i].key;
    }
}


// ====================
// AF_Input_GetKey
// Get key by code
// ====================
AF_Key* AF_Input_GetKey(int32_t _code, AF_Input* _input){
    return &_input->keys[0][_code];
}


// ====================
// AF_Input_ZERO
// Input struct Initialise to zero
// ====================
AF_Input AF_Input_ZERO(void){
    AF_Input input;
    memset(&input, 0, sizeof(AF_Input)); // Zero-initialize the struct
    input.firstMouse = AF_TRUE; // Set firstMouse to true
    return input;
}


// ====================
// AF_Input_EncodeKey
// Function to encode the key into a pressed state
// ====================
char AF_Input_EncodeKey(PACKED_CHAR _keyCode, af_bool_t _isPressed) {
    char returnedChar = _keyCode & KEYCODE_MASK; // Ensure only lower 7 bits are used for keycode

    if (_isPressed) {
        returnedChar |= PRESSED_MASK; // Set the 8th bit if the key is pressed
    }

    return returnedChar;
}


// ====================
// AF_Input_GetKeyCode
// Function to decode the key value
// ====================
PACKED_CHAR AF_Input_GetKeyCode(PACKED_CHAR _encodedKey) {
    return _encodedKey & KEYCODE_MASK;  // Return the lower 7 bits as the keycode
}


// ====================
// AF_Input_IsKeyPressed
// Function to check if the key is pressed
// ====================
af_bool_t AF_Input_IsKeyPressed(PACKED_CHAR _encodedKey) {
    return (_encodedKey & PRESSED_MASK) != 0;  // Check if the 8th bit is set
}


// ================
// AF_Input_Keymappings_ConvertToCharArray
// Construct and return the keys as an array of chars
// ================
void AF_Input_Keymappings_ConvertToCharArray(const AF_KeyMap* _keyMappings, const char** _charArray, uint32_t _size) {
    if (!_keyMappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _keyMappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}
