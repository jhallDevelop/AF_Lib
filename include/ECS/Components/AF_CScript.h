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
typedef void (*ScriptFuncPtr)(uint32_t, void*);

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
	// TODO: move strings out to a separate struct or use a string library
	char scriptName[MAX_CSCRIPT_PATH];
	char scriptFullPath[MAX_CSCRIPT_PATH];
	ScriptFuncPtr startFuncPtr;
	ScriptFuncPtr updateFuncPtr;
	ScriptFuncPtr destroyFuncPtr;
	void* loadedScriptPtr;
} AF_CScript;

AF_CScript AF_CScript_ZERO(void);
AF_CScript AF_CScript_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CSCRIPT_H

