/*
===============================================================================
AF_INPUT_ACTION_H definitions

Definition for the input action struct
===============================================================================
*/
#ifndef AF_INPUT_ACTION_H
#define AF_INPUT_ACTION_H
#include "AF_Lib_Define.h"
#include "AF_Input.h"

#ifdef __cplusplus
extern "C" {    
#endif

#define AF_INPUTACTION_ACTIONS_COUNT 10

typedef enum AF_ActionType {
    ACTION_TYPE_NONE = 0,
    ACTION_TYPE_MOVE_LEFT = 1,
    ACTION_TYPE_MOVE_RIGHT = 2,
    ACTION_TYPE_MOVE_FORWARD = 3,
    ACTION_TYPE_MOVE_BACK = 4,
    ACTION_TYPE_MOVE_UP = 5,
    ACTION_TYPE_MOVE_DOWN = 6,
    ACTION_TYPE_JUMP = 7,
    ACTION_TYPE_SHOOT = 8,
    ACTION_TYPE_CUSTOM = 9
} AF_ActionType;

typedef struct { 
    AF_ActionType key;
    const char* name;
} AF_InputAction_Map;


static const AF_InputAction_Map AF_InputAction_KeyMappings[] = {
    {ACTION_TYPE_NONE, "NONE"},
    {ACTION_TYPE_MOVE_LEFT, "Move Left"},
    {ACTION_TYPE_MOVE_RIGHT, "Move Right"},
    {ACTION_TYPE_MOVE_FORWARD, "Move Forward"},
    {ACTION_TYPE_MOVE_BACK, "Move Back"},
    {ACTION_TYPE_MOVE_UP, "Move Up"},
    {ACTION_TYPE_MOVE_DOWN, "Move Down"},
    {ACTION_TYPE_JUMP, "Jump"},
    {ACTION_TYPE_SHOOT, "Shoot"},
    {ACTION_TYPE_CUSTOM, "Custom"}
};
/*
====================
AF_InputAction
AF_InputAction struct
====================
*/
typedef struct AF_InputAction {
    BOOL enabled;
    AF_Key* keyPtr;
    AF_ActionType actionType;
    void (*actionFuncPtr)(AF_Key*); // Correct forward reference
} AF_InputAction;


/*
====================
AF_InputAction_ZERO
Init for an AF_InputAction struct
====================
*/
inline static AF_InputAction AF_InputAction_ZERO(void){
    AF_InputAction returnIA = {
        .enabled = FALSE,
        .keyPtr = NULL,//{0, 0, 0},
        .actionType = ACTION_TYPE_NONE,
        .actionFuncPtr = NULL
    };
    return returnIA;
}

/*
================
AF_Input_Keymappings_ConvertToCharArray
// Construct and return the keys as an array of chars
================
*/
inline static void AF_InputAction_ActionMap_ConvertToCharArray(const AF_InputAction_Map* _actionMappings, const char** _charArray, uint32_t _size) {
    if (!_actionMappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _actionMappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}

/*
================
AF_InputAction_None()
Do none action
================
*/
inline static void AF_InputAction_None(AF_Key* _key) {
   AF_Log("AF_InputAction_None: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Left()
Do none action
================
*/
inline static void AF_InputAction_Move_Left(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Left: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Right()
Do none action
================
*/
inline static void AF_InputAction_Move_Right(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Right: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Forward()
Do none action
================
*/
inline static void AF_InputAction_Move_Forward(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Forward: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Back()
Do none action
================
*/
inline static void AF_InputAction_Move_Back(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Back: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Up()
Do none action
================
*/
inline static void AF_InputAction_Move_Up(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Up: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Down()
Do none action
================
*/
inline static void AF_InputAction_Move_Down(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Down: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Jump()
Do none action
================
*/
inline static void AF_InputAction_Move_Jump(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Jump: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Shoot()
Do none action
================
*/
inline static void AF_InputAction_Move_Shoot(AF_Key* _key) {
   AF_Log("AF_InputAction_Shoot_Forward: Action %i \n", _key->code);
}

/*
================
AF_InputAction_Move_Custom()
Do none action
================
*/
inline static void AF_InputAction_Move_Custom(AF_Key* _key) {
   AF_Log("AF_InputAction_Move_Custom: Action %i \n", _key->code);
}


#ifdef __cplusplus
}
#endif

#endif //AF_INPUT_ACTION_H 
