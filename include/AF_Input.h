/*
===============================================================================
AF_INPUT_H definitions

Definition for the AF_Input structs
and helper functions
===============================================================================
*/
#ifndef AF_INPUT_H
#define AF_INPUT_H
#include "AF_Lib_Define.h"
#include "AF_Vec2.h"
#include "AF_Log.h"
#include "AF_Key.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define AF_INPUT_KEYS_MAPPED 10
#define AF_INPUT_KEYBOARD_KEYS_COUNT 47

typedef enum AF_KeyboardKeyEnum {
    //AF_KEY_BACKSPACE = 8,
    AF_KEY_TAB = 9,
    AF_KEY_ENTER = 13,
    AF_KEY_SHIFT = 16,
    AF_KEY_CTRL = 17,
    AF_KEY_ALT = 18,
    //AF_KEY_PAUSE = 19,
    //AF_KEY_CAPS_LOCK = 20,
    AF_KEY_ESCAPE = 256,//27,
    AF_KEY_SPACE = 32,
    //AF_KEY_PAGE_UP = 33,
    //AF_KEY_PAGE_DOWN = 34,
    //AF_KEY_END = 35,
    //AF_KEY_HOME = 36,
    AF_KEY_LEFT_ARROW = 37,
    AF_KEY_UP_ARROW = 38,
    AF_KEY_RIGHT_ARROW = 39,
    AF_KEY_DOWN_ARROW = 40,
    //AF_KEY_PRINT_SCREEN = 44,
    //AF_KEY_INSERT = 45,
    //AF_KEY_DELETE = 46,
    
    // Number keys (0-9)
    AF_KEY_0 = 48,
    AF_KEY_1 = 49,
    AF_KEY_2 = 50,
    AF_KEY_3 = 51,
    AF_KEY_4 = 52,
    AF_KEY_5 = 53,
    AF_KEY_6 = 54,
    AF_KEY_7 = 55,
    AF_KEY_8 = 56,
    AF_KEY_9 = 57,
    
    // Alphabet keys (A-Z)
    AF_KEY_A = 65,
    AF_KEY_B = 66,
    AF_KEY_C = 67,
    AF_KEY_D = 68,
    AF_KEY_E = 69,
    AF_KEY_F = 70,
    AF_KEY_G = 71,
    AF_KEY_H = 72,
    AF_KEY_I = 73,
    AF_KEY_J = 74,
    AF_KEY_K = 75,
    AF_KEY_L = 76,
    AF_KEY_M = 77,
    AF_KEY_N = 78,
    AF_KEY_O = 79,
    AF_KEY_P = 80,
    AF_KEY_Q = 81,
    AF_KEY_R = 82,
    AF_KEY_S = 83,
    AF_KEY_T = 84,
    AF_KEY_U = 85,
    AF_KEY_V = 86,
    AF_KEY_W = 87,
    AF_KEY_X = 88,
    AF_KEY_Y = 89,
    AF_KEY_Z = 90,
    
    // Function keys (F1-F12)
    /*
    AF_KEY_F1 = 112,
    AF_KEY_F2 = 113,
    AF_KEY_F3 = 114,
    AF_KEY_F4 = 115,
    AF_KEY_F5 = 116,
    AF_KEY_F6 = 117,
    AF_KEY_F7 = 118,
    AF_KEY_F8 = 119,
    AF_KEY_F9 = 120,
    AF_KEY_F10 = 121,
    AF_KEY_F11 = 122,
    AF_KEY_F12 = 123,
    */
    // Other keys
    /*
    AF_KEY_NUM_LOCK = 144,
    AF_KEY_SCROLL_LOCK = 145,
    AF_KEY_SEMICOLON = 186,
    AF_KEY_EQUAL = 187,
    AF_KEY_COMMA = 188,
    AF_KEY_DASH = 189,
    AF_KEY_PERIOD = 190,
    AF_KEY_SLASH = 191,
    AF_KEY_GRAVE = 192,
    AF_KEY_LEFT_BRACKET = 219,
    AF_KEY_BACKSLASH = 220,
    AF_KEY_RIGHT_BRACKET = 221,
    AF_KEY_APOSTROPHE = 222
    */
} AF_KeyboardKeyEnum;

typedef struct { 
    AF_KeyboardKeyEnum key;
    const char* name;
} AF_KeyMap;


static const AF_KeyMap AF_Input_KeyMappings[] = {
    {AF_KEY_0, "0"},
    {AF_KEY_1, "1"},
    {AF_KEY_2, "2"},
    {AF_KEY_3, "3"},
    {AF_KEY_4, "4"},
    {AF_KEY_5, "5"},
    {AF_KEY_6, "6"},
    {AF_KEY_7, "7"},
    {AF_KEY_8, "8"},
    {AF_KEY_9, "9"},

    {AF_KEY_A, "A"},
    {AF_KEY_B, "B"},
    {AF_KEY_C, "C"},
    {AF_KEY_D, "D"},
    {AF_KEY_E, "E"},
    {AF_KEY_F, "F"},
    {AF_KEY_G, "G"},
    {AF_KEY_H, "H"},
    {AF_KEY_I, "I"},
    {AF_KEY_J, "J"},
    {AF_KEY_K, "K"},
    {AF_KEY_L, "L"},
    {AF_KEY_M, "M"},
    {AF_KEY_N, "N"},
    {AF_KEY_O, "O"},
    {AF_KEY_P, "P"},
    {AF_KEY_Q, "Q"},
    {AF_KEY_R, "R"},
    {AF_KEY_S, "S"},
    {AF_KEY_T, "T"},
    {AF_KEY_U, "U"},
    {AF_KEY_V, "V"},
    {AF_KEY_W, "W"},
    {AF_KEY_X, "X"},
    {AF_KEY_Y, "Y"},
    {AF_KEY_Z, "Z"},
    {AF_KEY_TAB, "Tab"},
    {AF_KEY_ENTER, "Enter"},
    {AF_KEY_SHIFT, "Shift"},
    {AF_KEY_CTRL, "Ctrl"},
    {AF_KEY_ALT, "Alt"},
    {AF_KEY_ESCAPE, "Escape"},
    {AF_KEY_SPACE, "Space"},
    {AF_KEY_LEFT_ARROW, "Left Arrow"},
    {AF_KEY_UP_ARROW, "Up Arrow"},
    {AF_KEY_RIGHT_ARROW, "Right Arrow"},
    {AF_KEY_DOWN_ARROW, "Down Arrow"}
};


// Define the key mappings
extern const AF_KeyMap AF_Input_KeyMappings[];


#define PRESSED_MASK 0x80  // Pressed bit mask (8th bit)
#define KEYCODE_MASK 0x7F  // Keycode bit mask (lower 7 bits)
#define CONTROLLER_COUNT 4




/*
====================
AF_Input
Input struct to store the registered keys
====================
*/
typedef struct {
    // input buffer que
    // TODO: make array for size CONTROLLER_COUNT
    AF_Key keys[CONTROLLER_COUNT][AF_INPUT_KEYBOARD_KEYS_COUNT];

    Vec2 controlSticks[CONTROLLER_COUNT];

    // Mouse
    af_bool_t mouse1Down;
    af_bool_t mouse2Down;
    af_bool_t firstMouse;
    float lastMouseX;
    float lastMouseY;
    float mouseX;
    float mouseY;

} AF_Input;

/*
====================
AF_Input_MapKeys
map keys
====================
*/
static inline void AF_Input_MapKeyMappings(AF_Input* _input, const AF_KeyMap* _keymappings){
    
    for(uint32_t i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; i++){
        _input->keys[0][i].code = _keymappings[i].key;
    }
}

/*
====================
AF_Input_GetKey
Get key by code
====================
*/
static inline AF_Key* AF_Input_GetKey(char _code, AF_Input* _input){
    AF_Key* returnKey = NULL;
    for(uint32_t i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; i++){
        if(_input->keys[0][i].code == _code){
            returnKey = &_input->keys[0][i];
        }
    }
    return returnKey;
}

/*
====================
AF_Input_ZERO
Input struct Initialise to zero
====================
*/
static inline AF_Input AF_Input_ZERO(void){
    AF_Input input;
    
    // Initialise the keys
    for(int i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; ++i){
        AF_Key key = {0, 0, 0};
        input.keys[0][i] = key; // Player 1
        input.keys[1][i] = key; // Player 2
        input.keys[2][i] = key; // Player 3
        input.keys[3][i] = key; // Player 4
        
       // map the keymappings to input keys
    }
    
    AF_Input_MapKeyMappings(&input, AF_Input_KeyMappings);
    
    for(int i = 0; i < CONTROLLER_COUNT; ++i){
        Vec2 controlStick = {0, 0};
        input.controlSticks[i] = controlStick;
    }
    input.firstMouse = AF_TRUE;
    input.mouse1Down = AF_FALSE;
    input.mouse2Down = AF_FALSE;
    input.mouseX = 0;
    input.mouseY = 0;
    return input;
}




/*
====================
AF_Input_Input
Init definition
====================
*/
void AF_Input_Init(void);

/*
====================
AF_Input_Update
Update definition
====================
*/
void AF_Input_Update(AF_Input* _input);


/*
====================
AF_Input_Shutdown
Shutdown definition
====================
*/
void AF_Input_Shutdown(void);


/*
====================
AF_Input_EncodeKey
Function to encode the key into a pressed state
====================
*/
static inline char AF_Input_EncodeKey(PACKED_CHAR _keyCode, af_bool_t _isPressed) {
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
static inline af_bool_t AF_Input_IsKeyPressed(PACKED_CHAR _encodedKey) {
    return (_encodedKey & PRESSED_MASK) != 0;  // Check if the 8th bit is set
}


/*
================
AF_Input_Keymappings_ConvertToCharArray
// Construct and return the keys as an array of chars
================
*/
inline static void AF_Input_Keymappings_ConvertToCharArray(const AF_KeyMap* _keyMappings, const char** _charArray, uint32_t _size) {
    if (!_keyMappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _keyMappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}


#ifdef __cplusplus
}
#endif
#endif // AF_INPUT_H
