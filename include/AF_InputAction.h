/*
===============================================================================
AF_INPUT_ACTION_H definitions

Definition for the input action struct
===============================================================================
*/
#ifndef AF_INPUT_ACTION_H
#define AF_INPUT_ACTION_H
#include <stdarg.h>
#include "AF_Lib_Define.h"
#include "AF_Key.h"
//#include "AF_Input.h"
#include "AF_Math/AF_Vec2.h"
#include "AF_Math/AF_Vec3.h"
#include <stddef.h> // For NULL
#include "AF_Log.h"

#ifdef __cplusplus
extern "C" {    
#endif

#define AF_INPUTACTION_ACTIONS_COUNT 4

typedef enum AF_ActionType {
    ACTION_TYPE_NONE = 0,
    ACTION_TYPE_ADD_VELOCITY = 1,
    ACTION_TYPE_SHOOT = 2,
    ACTION_TYPE_CUSTOM = 3
} AF_ActionType;

typedef struct { 
    AF_ActionType key;
    const char* name;
} AF_InputAction_Map;


static const AF_InputAction_Map AF_InputAction_KeyMappings[] = {
    {ACTION_TYPE_NONE, "NONE"},
    {ACTION_TYPE_ADD_VELOCITY, "Add Velocity"},
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
    af_bool_t enabled;
    AF_Key key;
    AF_ActionType actionType;
    af_bool_t boolBuffer;
    uint32_t uintBuffer;
    float floatBuffer;
    Vec2 vec2Buffer;
    Vec3 vec3Buffer;
    void (*actionFuncPtr)(uint32_t,...); // Variadic function pointer
    int32_t currentKeyIndex;
	int32_t currentActionIndex;
} AF_InputAction;

/*
====================
AF_InputAction_DefaultFunction
Default function for an AF_InputAction struct
====================
*/
inline static void AF_InputAction_DefaultFunction(uint32_t _numArgs, ...) {
   va_list args;
   va_start(args, _numArgs);
   
   AF_Log_Warning("AF_InputAction_DefaultFunction: using default varadic function. assign custom function pointer \n");

   va_end(args);
}


/*
====================
AF_InputAction_ZERO
Init for an AF_InputAction struct
====================
*/
inline static AF_InputAction AF_InputAction_ZERO(void) {
    AF_InputAction returnIA;
    returnIA.enabled = AF_FALSE;
    returnIA.key.code = 0;
    returnIA.key.held = 0;
    returnIA.key.pressed = 0;
    returnIA.actionType = ACTION_TYPE_NONE;
    returnIA.boolBuffer = AF_FALSE;
    returnIA.uintBuffer = 0;
    returnIA.floatBuffer = 0.0f;
    returnIA.vec2Buffer.x = 0.0f;
    returnIA.vec2Buffer.y = 0.0f;
    returnIA.vec3Buffer.x = 0.0f;
	returnIA.vec3Buffer.y = 0.0f;
	returnIA.vec3Buffer.z = 0.0f;
    returnIA.actionFuncPtr = AF_InputAction_DefaultFunction;
    returnIA.currentKeyIndex = 0;
    returnIA.currentActionIndex = 0;
    
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




#ifdef __cplusplus
}
#endif

#endif //AF_INPUT_ACTION_H 
