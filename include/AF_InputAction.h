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

/*
====================
AF_InputAction
AF_InputAction struct
====================
*/
typedef struct AF_InputAction {
    BOOL enabled;
    AF_Key key;
    void* actionFuncPtr;
} AF_InputAction;


/*
====================
AF_InputAction_ZERO
Init for an AF_InputAction struct
====================
*/
inline static AF_InputAction AF_InputAction_ZERO(){
    AF_InputAction returnIA = {
        .enabled = FALSE,
        .key = {0, 0, 0},
        .actionFuncPtr = NULL
    };
}


#ifdef __cplusplus
}
#endif

#endif //AF_INPUT_ACTION_H 
