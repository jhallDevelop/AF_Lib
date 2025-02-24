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
#include <stdarg.h>  // Include for variadic functions

#include "AF_Lib_Define.h"
#include "ECS/Entities/AF_ECS.h"

#ifdef __cplusplus
extern "C" {
#endif

static const Vec3 ZERO_Vec3 = {0.0f, 0.0f, 0.0f};


// ==============================================================
// Action functions
// ==============================================================
/*
================
AF_SActions_None()
Do none action
================
*/
inline static void AF_SActions_None() {
   AF_Log("AF_SActions_None: Action \n");
}

/*
================
AF_SActions_Add_Velocity(uint32_t _argsSize, AF_Entity* _entity, Vec3 _velocity)
Add velocity to the rigidbody
================
*/
//Vec3 _velocity, AF_C3DRigidbody* _rigidbody
inline static void AF_SActions_Add_Velocity(uint32_t _argsSize, ...) {
    va_list args;
    va_start(args, _argsSize);
    AF_Entity* _entity = va_arg(args, AF_Entity*);
    Vec3* _velocity = va_arg(args, Vec3*);
    va_end(args);
    if(_entity == NULL){
        AF_Log_Warning("AF_SActions_Add_Velocity: _entity ptr is null \n");
        return;
    }
    AF_C3DRigidbody* _rigidbody = _entity->rigidbody;
    if(_rigidbody == NULL){
        AF_Log_Warning("AF_SActions_Add_Velocity: _rigidbody ptr is null \n");
        return;
    }
    //AF_Log("AF_SActions_Add_Velocity: Action x: (%f y: %f z: %f) \n", _velocity->x, _velocity->y, _velocity->z);
    _rigidbody->velocity = Vec3_ADD(_rigidbody->velocity, *_velocity);
    //_entity->transform->pos = Vec3_ADD(_entity->transform->pos, *_velocity);
}


/*
================
AF_SActions_Shoot()
Do none action
================
*/
inline static void AF_SActions_Shoot(AF_Key* _key) {
   AF_Log("AF_SActions_Shoot: Action %i \n", _key->code);
}

/*
================
AF_SActions_Custom()
Do none action
================
*/
inline static void AF_SActions_Custom(AF_Key* _key) {
   AF_Log("AF_SActions_Custom: Action %i \n", _key->code);
}

/*
====================
AF_SActions_Update
Process key presses and call the associated action function pointer

====================
*/
// TODO: this function is big an ugly. split it up
inline static void AF_SActions_Update(AF_Input* _input, AF_Entity* _entity){
    if(_input == NULL){
        AF_Log_Warning("AF_SAction_Update: _input ptr is null \n");
        return;
    }
    AF_CInputController* _inputController = _entity->inputController;
    // Doublecheck the input controller has
    BOOL hasInputController = AF_Component_GetHas(_inputController->enabled);
    if(hasInputController == FALSE){
        return;
    }

    // TODO: don't do this, let velocity dissapate over time
	_entity->rigidbody->velocity = Vec3_ZERO();
	//rigidbody->anglularVelocity = Vec3_ZERO;
	//rigidbody->force = Vec3_ZERO;
	//rigidbody->torque = Vec3_ZERO;
	//rigidbody->inertiaTensor = Vec3_ZERO;
	// Clear Velocities
    // search to see if the input action is from our pre-scribed list, or is it custom
    // for each key pressed in the input controller
    for (uint32_t i = 0; i < MAX_CINPUTCONTROLLER_ACTIONS; i++){
        AF_InputAction* inputAction = &_inputController->inputActions[i];

        //AF_Log("AF_SActions: Action func ptr: %p\n", inputAction->actionFuncPtr);
        // Call the action function pointer passing the input controller and rigidbody
        // Double check the rigidbody has

        switch (inputAction->actionType)
        {
        case AF_ActionType::ACTION_TYPE_NONE:
            /* code */
            //AF_Log_Warning("AF_SActions_Update: Action type is NONE, not implemented \n");
            break;
        case AF_ActionType::ACTION_TYPE_ADD_VELOCITY:
           
            
            inputAction->key = *AF_Input_GetKey(AF_Input_KeyMappings[inputAction->currentKeyIndex].key, _input);
            // Update the action function pointer
            inputAction->actionFuncPtr = AF_SActions_Add_Velocity;

            //if (inputAction->keyPtr->pressed == FALSE){
            if (inputAction->key.pressed == FALSE){
                inputAction->actionFuncPtr(1, _entity, &ZERO_Vec3);
                //_entity->rigidbody->velocity = {0.0f, 0.0f, 0.0f};
            } else{
                // send velocity from input aciton buffer
                inputAction->actionFuncPtr(1, _entity, &inputAction->vec3Buffer);
            }
            
            break;
        case AF_ActionType::ACTION_TYPE_SHOOT:
            /* code */
            AF_Log_Warning("AF_SActions_Update: Action type is Shoot, not implemented \n");
            break;

        case AF_ActionType::ACTION_TYPE_CUSTOM:
            /* code */
            AF_Log_Warning("AF_SActions_Update: Action type is Custom, not implemented \n");
            break;
        
        default:
            break;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif // AF_SACTION_H
