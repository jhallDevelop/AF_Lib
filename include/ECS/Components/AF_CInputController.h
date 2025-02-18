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


/*
====================
AF_CInputController_ZERO
Empty constructor for the CRigidbody component
====================
*/
static inline AF_CInputController AF_CInputController_ZERO(void){
	// create a version to use
	AF_CInputController inputController = {
		.enabled = FALSE,
		.inputActionCount = 0,
	};
	// init the input actions array
	for(uint32_t i = 0; i < MAX_CINPUTCONTROLLER_ACTIONS; i++){
		inputController.inputActions[i] = AF_InputAction_ZERO();
	}
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
		.enabled = component,
		.inputActionCount = 0
	};

	// init the input actions array
	for(uint32_t i = 0; i < MAX_CINPUTCONTROLLER_ACTIONS; i++){
		inputController.inputActions[i] = AF_InputAction_ZERO();
	}
	return inputController;
}


#ifdef __cplusplus
}
#endif

#endif //AF_CINPUT_CONTROLLER_H 

