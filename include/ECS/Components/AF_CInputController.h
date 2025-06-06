/*
===============================================================================
AF_CINPUT_CONTROLLER_H definitions

Definition for the input controller component struct
and helper functions
===============================================================================
*/
#ifndef AF_CINPUT_CONTROLLER_H
#define AF_CINPUT_CONTROLLER_H
#include "AF_Lib_Define.h"
#include "AF_InputAction.h"
#include "AF_Component.h"
#ifdef __cplusplus
extern "C" {    
#endif

#define MAX_CINPUTCONTROLLER_ACTIONS 8



/*
====================
AF_CInputController Struct used for physics
====================
*/
typedef struct AF_CInputController{
    PACKED_CHAR enabled;
	uint8_t inputActionCount;
    AF_InputAction inputActions[MAX_CINPUTCONTROLLER_ACTIONS];
    
} AF_CInputController;

AF_CInputController AF_CInputController_ZERO(void);
AF_CInputController AF_CInputController_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CINPUT_CONTROLLER_H 

