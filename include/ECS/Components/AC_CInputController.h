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
    AF_InputAction inputActions[MAX_CINPUTCONTROLLER_ACTIONS];
    
} AF_CInputController;


/*
====================
AF_CInputController_ZERO
Empty constructor for the CRigidbody component
====================
*/
static inline AF_CInputController AF_CInputController_ZERO(void){
	AF_CInputController inputController = {
		//.has = false,
		.enabled = FALSE,
		.inputActions = 
	};
	return inputController;
}

/*
====================
AF_CInputController_ADD
Add the AF_CInputController component
enable the component and set some values to default values.
====================
*/
static inline AF_CInputController AF_CInputController_ADD(void){
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);

	AF_CInputController inputController = {
		//.has = true,
		.enabled = component,
		.isKinematic = FALSE,			// isKinematic means to be controlled by script rather than the velocity
		.velocity = {0, 0},		// zero velocity 
		.gravity = 0				// gravity off by default
	};
	return inputController;
}


#ifdef __cplusplus
}
#endif

#endif //AF_CINPUT_CONTROLLER_H 

