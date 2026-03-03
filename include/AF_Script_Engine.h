/*
===============================================================================
AF_SCRIPT_ENGINE_H

Definitions for helper functions to load and use scripts 
===============================================================================
*/
#ifndef AF_SCRIPT_ENGINE_H
#define AF_SCRIPT_ENGINE_H
#include "AF_File.h"
#include "AF_Log.h"
#include "AF_AppData.h"
#ifdef _WIN32

#else
#include <dlfcn.h>
#endif
#include "ECS/Components/AF_Component.h"
#include "AF_Pre_Processor.h"

#define MAX_FUNCTION_NAME 1024

#ifdef __cplusplus
extern "C" {    
#endif



void* AF_Script_Load(const char* _filePath);
uint32_t AF_Script_Bind_Functions(AF_CScript* _script, void* _scriptSharedObjPtr);
void AF_Script_Load_And_Bind_Functions(AF_ECS* _ecs);
void AF_Script_UnLoad(void* _scriptSharedObjPtr);
void AF_Script_UnloadScripts(AF_ECS* _ecs);
ScriptFuncPtr AF_GetScriptFuncPtr(void* _sharedObjectPtr, const char* _funcName);
void AF_Script_Call_Start(AF_AppData* _appData);
void AF_Script_Call_Update(AF_AppData* _appData);
void AF_Script_Call_LateUpdate(AF_AppData* _appData);
void AF_Script_Call_Destroy(AF_AppData* _appData);
void AF_Script_SerialiseEditorVars(const char* _scriptPath, AF_CScript* _scriptComponent);
AF_EDITOR_VAR_TYPE_e AF_Script_MapStringToEditorVarType(const char* _typeString);
                



#ifdef __cplusplus
}
#endif

#endif
