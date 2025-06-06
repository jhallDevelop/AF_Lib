#include "ECS/Components/AF_CScript.h"
#include <stddef.h> // For NULL
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CCamera_ZERO
Initialisation constructor function
====================
*/
AF_CScript AF_CScript_ZERO(void){
    PACKED_CHAR component = FALSE;
	component = AF_Component_SetHas(component, FALSE);
	component = AF_Component_SetEnabled(component, FALSE);
	AF_CScript returnComponent = {
		.enabled = component,
		.scriptName = "\0",
		.scriptFullPath = "\0",
		.startFuncPtr = NULL,
		.updateFuncPtr = NULL,
		.destroyFuncPtr = NULL,
		.loadedScriptPtr = NULL
    };

    return returnComponent;
}

/*
====================
AF_CCamera_ADD
Add the camera component
Initialise with enable and has set to true
====================
*/
AF_CScript AF_CScript_ADD(void){
		
	PACKED_CHAR component = FALSE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);
	AF_CScript returnComponent = {
		.enabled = component,
		.scriptName = "\0",
		.scriptFullPath = "\0",
		.startFuncPtr = NULL,
		.updateFuncPtr = NULL,
		.destroyFuncPtr = NULL,
		.loadedScriptPtr = NULL
    };

    return returnComponent;
}
