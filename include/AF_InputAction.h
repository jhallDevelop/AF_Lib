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

// Example function pointer type: accepts a controller and a rigidbody
typedef void (*ActionFuncPtr)(void* _controller, void* _rigidbody);
/*
====================
AF_InputAction
AF_InputAction struct
====================
*/
typedef struct AF_InputAction {
    BOOL enabled;
    AF_Key* keyPtr;
    ActionFuncPtr actionFuncPtr;
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
        .actionFuncPtr = NULL
    };
    return returnIA;
}


#ifdef __cplusplus
}
#endif

#endif //AF_INPUT_ACTION_H 
