/*
===============================================================================
AF_SMOVEMENT_H definitions

Definition for the movement system
and helper functions
===============================================================================
*/
#ifndef AF_SMOVEMENT_H
#define AF_SMOVEMENT_H
#include "AF_Lib_Define.h"
#include "ECS/Entities/AF_ECS.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_SMOVEMENT_
Process key presses and call the associated function pointer

====================
*/
inline static void AF_SMovement_Move(AF_Input* _input, AF_CInputController* _inputController, AF_C3DRigidbody* _rigidbody){
    if(_input == NULL){
        AF_Log_Warning("AF_SMovement_Move: _input ptr is null \n");
        return;
    }
    // Doublecheck the input controller has
    BOOL hasInputController = AF_Component_GetHas(_inputController->enabled);
    if(hasInputController == FALSE){
        return;
    }
    

    // copy over the key state if there is a match
    // TODO fix this
    /*
    if(_input->keys[0][0].pressed == TRUE){
        AF_Log("AF_SMovement_Move\n");
        // Call the action function pointer passing the input controller and rigidbody
        if(_inputController->inputActionCount > 0){
            _inputController->inputActions[0].actionFuncPtr(_inputController, _rigidbody);
        }
    }
    */
    
    // for each key pressed in the input controller
    for (uint32_t i = 0; i < MAX_CINPUTCONTROLLER_ACTIONS; i++){
        if (_inputController->inputActions[i].keyPtr == NULL){
            continue;
        }
        if (_inputController->inputActions[i].keyPtr->pressed == FALSE){
            continue;
        } 
        if(_inputController->inputActions[i].actionFuncPtr == NULL) {
            continue;
        }
        AF_Log("AF_SMovement_Move\n");
        // Call the action function pointer passing the input controller and rigidbody
        // Double check the rigidbody has
        BOOL hasRigidBody = AF_Component_GetHas(_rigidbody->enabled);
        if(hasRigidBody == FALSE){
            continue;
        }
        _inputController->inputActions[i].actionFuncPtr(_inputController, _rigidbody);
    }
    
    // call the function pointer

}

#ifdef __cplusplus
}
#endif

#endif // AF_SMOVEMENT_H
