#include "ECS/Components/AF_CInputController.h"
#include "ECS/Components/AF_Component.h"


/*
====================
AF_CInputController_ZERO
Empty constructor for the CRigidbody component
====================
*/
AF_CInputController AF_CInputController_ZERO(void){
	// create a version to use
	AF_CInputController inputController = {
		.enabled = AF_FALSE,
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
AF_CInputController AF_CInputController_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);

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
