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
#include "AF_Lib_API.h"
#include "AF_Math/AF_Vec2.h"
#include "AF_Log.h"
#include "AF_Key.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define AF_INPUT_KEYS_MAPPED 10
#define AF_INPUT_KEYBOARD_KEYS_COUNT 350

typedef enum AF_KeyboardKeyEnum {
    // System / Validation
    AF_KEY_NUL           = 0,
    AF_KEY_SOH           = 1,
    AF_KEY_STX           = 2,
    AF_KEY_ETX           = 3,
    AF_KEY_EOT           = 4,
    AF_KEY_ENQ           = 5,
    AF_KEY_ACK           = 6,
    AF_KEY_BEL           = 7,
    AF_KEY_BS            = 8, 
    AF_KEY_TAB           = 9,
    AF_KEY_LF            = 10,
    AF_KEY_VT            = 11,
    AF_KEY_FF            = 12,
    AF_KEY_CR            = 13,
    AF_KEY_SO            = 14,
    AF_KEY_SI            = 15,
    AF_KEY_DLE           = 16,
    AF_KEY_DC1           = 17,
    AF_KEY_DC2           = 18,
    AF_KEY_DC3           = 19,
    AF_KEY_DC4           = 20,
    AF_KEY_NAK           = 21,
    AF_KEY_SYN           = 22,
    AF_KEY_ETB           = 23,
    AF_KEY_CAN           = 24,
    AF_KEY_EM            = 25,
    AF_KEY_SUB           = 26,
    AF_KEY_ESC           = 27,
    AF_KEY_FS            = 28,
    AF_KEY_GS            = 29,
    AF_KEY_RS            = 30,
    AF_KEY_US            = 31,
    AF_KEY_SPACE         = 32,
    AF_KEY_EXCLAMATION   = 33,
    AF_KEY_DOUBLE_QUOTE  = 34,
    AF_KEY_HASH          = 35,
    AF_KEY_DOLLAR        = 36,
    AF_KEY_PERCENT       = 37,
    AF_KEY_AMPERSAND     = 38,
    AF_KEY_SINGLE_QUOTE  = 39,
    AF_KEY_LEFT_PAREN    = 40,
    AF_KEY_RIGHT_PAREN   = 41,
    AF_KEY_ASTERISK      = 42,
    AF_KEY_PLUS          = 43,
    AF_KEY_COMMA         = 44,
    AF_KEY_MINUS         = 45,
    AF_KEY_PERIOD        = 46,
    AF_KEY_SLASH         = 47,

    // Numbers (0-9)
    AF_KEY_0             = 48,
    AF_KEY_1             = 49,
    AF_KEY_2             = 50,
    AF_KEY_3             = 51,
    AF_KEY_4             = 52,
    AF_KEY_5             = 53,
    AF_KEY_6             = 54,
    AF_KEY_7             = 55,
    AF_KEY_8             = 56,
    AF_KEY_9             = 57,

    // Punctuation (Mid-range)
    AF_KEY_COLON         = 58,
    AF_KEY_SEMICOLON     = 59,
    AF_KEY_LESS_THAN     = 60,
    AF_KEY_EQUALS        = 61,
    AF_KEY_GREATER_THAN  = 62,
    AF_KEY_QUESTION      = 63,
    AF_KEY_AT            = 64,

    // Alphabet (A-Z)
    AF_KEY_A             = 65,
    AF_KEY_B             = 66,
    AF_KEY_C             = 67,
    AF_KEY_D             = 68,
    AF_KEY_E             = 69,
    AF_KEY_F             = 70,
    AF_KEY_G             = 71,
    AF_KEY_H             = 72,
    AF_KEY_I             = 73,
    AF_KEY_J             = 74,
    AF_KEY_K             = 75,
    AF_KEY_L             = 76,
    AF_KEY_M             = 77,
    AF_KEY_N             = 78,
    AF_KEY_O             = 79,
    AF_KEY_P             = 80,
    AF_KEY_Q             = 81,
    AF_KEY_R             = 82,
    AF_KEY_S             = 83,
    AF_KEY_T             = 84,
    AF_KEY_U             = 85,
    AF_KEY_V             = 86,
    AF_KEY_W             = 87,
    AF_KEY_X             = 88,
    AF_KEY_Y             = 89,
    AF_KEY_Z             = 90,

    // Brackets and Symbols
    AF_KEY_LEFT_BRACKET  = 91,
    AF_KEY_BACKSLASH     = 92,
    AF_KEY_RIGHT_BRACKET = 93,
    AF_KEY_CARET         = 94,
    AF_KEY_UNDERSCORE    = 95,
    AF_KEY_BACKTICK      = 96,
    // Lowercase (a-z)
    AF_KEY_a             = 97,
    AF_KEY_b             = 98,
    AF_KEY_c             = 99,
    AF_KEY_d             = 100,
    AF_KEY_e             = 101,
    AF_KEY_f             = 102,
    AF_KEY_g             = 103,
    AF_KEY_h             = 104,
    AF_KEY_i             = 105,
    AF_KEY_j             = 106,
    AF_KEY_k             = 107,
    AF_KEY_l             = 108,
    AF_KEY_m             = 109,
    AF_KEY_n             = 110,
    AF_KEY_o             = 111,
    AF_KEY_p             = 112,
    AF_KEY_q             = 113,
    AF_KEY_r             = 114,
    AF_KEY_s             = 115,
    AF_KEY_t             = 116,
    AF_KEY_u             = 117,
    AF_KEY_v             = 118,
    AF_KEY_w             = 119,
    AF_KEY_x             = 120,
    AF_KEY_y             = 121,
    AF_KEY_z             = 122,

    // Braces and Misc
    AF_KEY_LEFT_BRACE    = 123,
    AF_KEY_VERTICAL_BAR  = 124,
    AF_KEY_RIGHT_BRACE   = 125,
    AF_KEY_TILDE         = 126,
    AF_KEY_DELTA       = 127,

    // Function and Navigation Keys (GLFW Style Offsets)
    AF_KEY_ESCAPE        = 256,
    AF_KEY_ENTER         = 257,
    AF_KEY_TAB2          = 258,    // not sure why GLFW has this, but it is different from the standard tab key
    AF_KEY_BACKSPACE     = 259,
    AF_KEY_INSERT        = 260,
    AF_KEY_DELETE        = 261,
    AF_KEY_RIGHT_ARROW   = 262,
    AF_KEY_LEFT_ARROW    = 263,
    AF_KEY_DOWN_ARROW    = 264,
    AF_KEY_UP_ARROW      = 265,
    AF_KEY_PAGE_UP       = 266,
    AF_KEY_PAGE_DOWN     = 267,
    AF_KEY_HOME          = 268,
    AF_KEY_END           = 269,
    AF_KEY_CAPS_LOCK     = 280,
    AF_KEY_SCROLL_LOCK   = 281,
    AF_KEY_NUM_LOCK      = 282,
    AF_KEY_PRINT_SCREEN  = 283,
    AF_KEY_PAUSE         = 284,
    // Function Keys
    AF_KEY_F1            = 290,
    AF_KEY_F2            = 291,
    AF_KEY_F3            = 292,
    AF_KEY_F4            = 293,
    AF_KEY_F5            = 294,
    AF_KEY_F6            = 295,
    AF_KEY_F7            = 296,
    AF_KEY_F8            = 297,
    AF_KEY_F9            = 298,
    AF_KEY_F10           = 299,
    AF_KEY_F11           = 300,
    AF_KEY_F12           = 301,
    AF_KEY_F13           = 302,
    AF_KEY_F14           = 303,
    AF_KEY_F15           = 304,
    AF_KEY_F16           = 305,
    AF_KEY_F17           = 306,
    AF_KEY_F18           = 307,
    AF_KEY_F19           = 308,
    AF_KEY_F20           = 309,
    AF_KEY_F21           = 310,
    AF_KEY_F22           = 311,
    AF_KEY_F23           = 312,
    AF_KEY_F24           = 313,
    AF_KEY_F25           = 314,
    // Unassigned keys 
    AF_KEY_UNKNOWN1      = 316,
    AF_KEY_UNKNOWN2      = 317,
    AF_KEY_UNKNOWN3      = 318,
    AF_KEY_UNKNOWN4      = 319,
    // Keypad keys
    AF_KEY_KP_0          = 320,
    AF_KEY_KP_1          = 321,
    AF_KEY_KP_2          = 322,
    AF_KEY_KP_3          = 323,
    AF_KEY_KP_4          = 324,
    AF_KEY_KP_5          = 325,
    AF_KEY_KP_6          = 326,
    AF_KEY_KP_7          = 327,
    AF_KEY_KP_8          = 328,
    AF_KEY_KP_9          = 329,
    AF_KEY_KP_DECIMAL    = 330,
    AF_KEY_KP_DIVIDE     = 331,
    AF_KEY_KP_MULTIPLY   = 332,
    AF_KEY_KP_SUBTRACT   = 333,
    AF_KEY_KP_ADD        = 334,
    AF_KEY_KP_ENTER      = 335,
    AF_KEY_KP_EQUAL      = 336, 
    AF_KEY_LEFT_SHIFT    = 340,
    AF_KEY_LEFT_CONTROL  = 341,
    AF_KEY_LEFT_ALT      = 342,
    AF_KEY_LEFT_SUPER    = 343,
    AF_KEY_RIGHT_SHIFT   = 344,
    AF_KEY_RIGHT_CONTROL = 345,
    AF_KEY_RIGHT_ALT     = 346,
    AF_KEY_RIGHT_SUPER   = 347,
    // GLFW specific keys
    AF_KEY_MENU          = 348,
    AF_KEY_LAST          = 349
    
    
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
    {AF_KEY_LEFT_SHIFT, "L Shift"},
    {AF_KEY_LEFT_CONTROL, "L Ctrl"},
    {AF_KEY_LEFT_ALT, "L Alt"},
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
#define AF_INPUT_MOUSE_BUTTON_COUNT 8



/*
====================
AF_Input
Input struct to store the registered keys
====================
*/
typedef struct {
    AF_Key keys[CONTROLLER_COUNT][AF_INPUT_KEYBOARD_KEYS_COUNT];
    Vec2 controlSticks[CONTROLLER_COUNT];
    // Mouse
    float lastMouseX;
    float lastMouseY;
    float mouseX;
    float mouseY;

    af_bool_t mouseDownArray[AF_INPUT_MOUSE_BUTTON_COUNT]; // 0: left button, 1: right button
    af_bool_t firstMouse;

} AF_Input;

// ====================
// AF_Input_MapKeys
// map keys
// ====================
void AF_Input_MapKeyMappings(AF_Input* _input, const AF_KeyMap* _keymappings);


// ====================
// AF_Input_GetKey
// Get key by code
// ====================
AF_Key* AF_Input_GetKey(int32_t _code, AF_Input* _input);


// ====================
// AF_Input_ZERO
// Input struct Initialise to zero
// ====================
AF_Input AF_Input_ZERO(void);

// ====================
// AF_Input_Input
// Init definition
// ====================
void AF_Input_Init(void);


// ====================
// AF_Input_Update
// Update definition
// ====================

AF_LIB_API void AF_Input_Update(AF_Input* _input);


// ====================
// AF_Input_Shutdown
// Shutdown definition
// ====================
void AF_Input_Shutdown(void);


// ====================
// AF_Input_EncodeKey
// Function to encode the key into a pressed state
// ====================
char AF_Input_EncodeKey(PACKED_CHAR _keyCode, af_bool_t _isPressed);


// ====================
// AF_Input_GetKeyCode
// Function to decode the key value
// ====================
PACKED_CHAR AF_Input_GetKeyCode(PACKED_CHAR _encodedKey);


// ====================
// AF_Input_IsKeyPressed
// Function to check if the key is pressed
// ====================
af_bool_t AF_Input_IsKeyPressed(PACKED_CHAR _encodedKey);


// ================
// AF_Input_Keymappings_ConvertToCharArray
// Construct and return the keys as an array of chars
// ================
void AF_Input_Keymappings_ConvertToCharArray(const AF_KeyMap* _keyMappings, const char** _charArray, uint32_t _size);


#ifdef __cplusplus
}
#endif
#endif // AF_INPUT_H
