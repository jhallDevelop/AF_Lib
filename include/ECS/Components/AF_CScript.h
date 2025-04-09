/*
===============================================================================
AF_CSCRIPT_H

Script component
Designed to suport custom scripts being loaded onto an object.
===============================================================================
*/
#ifndef AF_CSCRIPT_H
#define AF_CSCRIPT_H
#include "AF_Lib_Define.h"
#include "AF_Component.h"

#define MAX_CSCRIPT_PATH 1024

// Define the correct function pointer type
typedef void (*ScriptFuncPtr)(void*);

#ifdef __cplusplus
extern "C" {
#endif


/*
====================
AF_CScript
Script struct
====================
*/
typedef struct AF_CScript {
    // TODO pack this
	PACKED_CHAR enabled;
	char scriptName[MAX_CSCRIPT_PATH];
	char scriptFullPath[MAX_CSCRIPT_PATH];
	ScriptFuncPtr startFuncPtr;
	ScriptFuncPtr updateFuncPtr;
	ScriptFuncPtr destroyFuncPtr;
	void* loadedScriptPtr;
} AF_CScript;

/*
====================
AF_CCamera_ZERO
Initialisation constructor function
====================
*/
inline static AF_CScript AF_CScript_ZERO(void){
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
inline static AF_CScript AF_CScript_ADD(void){
		
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



#ifdef __cplusplus
}
#endif

#endif //AF_CSCRIPT_H

