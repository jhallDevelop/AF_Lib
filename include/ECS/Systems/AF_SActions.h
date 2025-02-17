/*
===============================================================================
AF_SACTIONS_H definitions

Definition for the action system
Works with the AF_CInputcontroller to determin the inputs used, and calls the associated Action function pointers
and helper functions
===============================================================================
*/
#ifndef AF_SACTIONS_H
#define AF_SACTIONS_H
#include "AF_Lib_Define.h"
#include "ECS/Entities/AF_ECS.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_SActions_Update
Process key presses and call the associated action function pointer

====================
*/
inline static void AF_SActions_Update(AF_Input* _input, AF_CInputController* _inputController){
    if(_input == NULL){
        AF_Log_Warning("AF_SAction_Update: _input ptr is null \n");
        return;
    }
    
    // Doublecheck the input controller has
    BOOL hasInputController = AF_Component_GetHas(_inputController->enabled);
    if(hasInputController == FALSE){
        return;
    }

    // search to see if the input action is from our pre-scribed list, or is it custom
    // for each key pressed in the input controller
    for (uint32_t i = 0; i < MAX_CINPUTCONTROLLER_ACTIONS; i++){
        AF_InputAction* inputAction = &_inputController->inputActions[i];
        if (inputAction->keyPtr == NULL){
            continue;
        }
        
        if (inputAction->keyPtr->pressed == FALSE){
            continue;
        } 
        
        if(inputAction->actionFuncPtr == NULL) {
            continue;
        }

        //AF_Log("AF_SActions: Action func ptr: %p\n", inputAction->actionFuncPtr);
        // Call the action function pointer passing the input controller and rigidbody
        // Double check the rigidbody has
        _inputController->inputActions[i].actionFuncPtr(_inputController->inputActions[0].keyPtr);
    }
    
    // call the function pointer

}

#ifdef __cplusplus
}
#endif

#endif // AF_SACTION_H
