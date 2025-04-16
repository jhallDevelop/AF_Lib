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
#include <dlfcn.h>



#define MAX_FUNCTION_NAME 1024

#ifdef __cplusplus
extern "C" {    
#endif

/*
===============================================================================
AF_HasScriptEnabled
Check if the entity is enabled
Check if the entity has a script that is enabled
===============================================================================
*/
inline static BOOL AF_HasScriptEnabled(AF_Entity* _entity){
    // Check entity
    AF_Entity* entity = _entity;
    PACKED_CHAR* entityState = &entity->flags;
    BOOL entityIsEnabled = AF_Component_GetEnabled(*entityState);

    if(entityIsEnabled == FALSE){
        return FALSE;
    }

    // Check CScript Component
    AF_CScript* script = entity->scripts;
    BOOL hasComponent = AF_Component_GetHas(script->enabled);
    BOOL componentIsEnabled = AF_Component_GetEnabled(script->enabled);
    if(hasComponent == FALSE || componentIsEnabled == FALSE){
        return FALSE;
    }
    return TRUE;
}

/*
===============================================================================
AF_Script_Load
Take in a file path and attempt to load the script .o object as a shared object.AF_File_CloseFile
Return the pointer to the shared object
===============================================================================
*/
inline static void* AF_Script_Load(const char* _filePath){
    // Open a file for binary reading
    FILE* file = AF_File_OpenFile(_filePath, "rb");
    void *scriptPtr = NULL;
    if (file == NULL) {
        AF_Log_Error("AF_Script_Load: FAILED to open file %s\n", _filePath);
        return scriptPtr;
    } else {
        
        int eret;
        scriptPtr = dlopen(_filePath, RTLD_LOCAL | RTLD_LAZY);
        if (!scriptPtr) {
            AF_Log_Error("Game_App_Awake: Failed to open script 1 shared object: %s\n", dlerror());
            return scriptPtr;
        }
    }   

    return scriptPtr;
}

/*
===============================================================================
AF_Script_Bind_Functions
Take in a script component, and the loaded script shared object
Bind the script shared objects start, update, and destroy function points to the component
===============================================================================
*/
inline static void AF_Script_Bind_Functions(AF_CScript* _script, void* _scriptSharedObjPtr){
    if(_script == NULL || _scriptSharedObjPtr == NULL){
        AF_Log_Error("AF_Script_Bind_Functions: passed a null reference\n");
        return;
    }

    // ==== START Func ==== 
    // Get the script name
    char startFuncName[MAX_FUNCTION_NAME];
    snprintf(startFuncName, MAX_FUNCTION_NAME, "Start_%s", _script->scriptName);

    // Search the shared object for the function named
    ScriptFuncPtr startSCriptFunctPtr = (ScriptFuncPtr) dlsym(_scriptSharedObjPtr, startFuncName);
    
    // Check for any error after dlsym
    char *error = dlerror(); 
    if (error != NULL) {
        AF_Log_Error("AF_Script_Bind_Functions: Failed to load Start_%s: %s\n", startFuncName, error);
        return;
    }

    _script->startFuncPtr = startSCriptFunctPtr;

    // ==== Update Func ==== 
    // Get the script name
    char updateFuncName[MAX_FUNCTION_NAME];
    snprintf(updateFuncName, MAX_FUNCTION_NAME, "Update_%s", _script->scriptName);

    // Search the shared object for the function named
    ScriptFuncPtr updateScriptFunctPtr = (ScriptFuncPtr) dlsym(_scriptSharedObjPtr, updateFuncName);
    
    // Check for any error after dlsym
    error = dlerror(); 
    if (error != NULL) {
        AF_Log_Error("AF_Script_Bind_Functions: Failed to load Start_%s: %s\n", updateFuncName, error);
        return;
    }

    _script->updateFuncPtr = updateScriptFunctPtr;

    // ==== Destroy Func ==== 
    // Get the script name
    char destroyFuncName[MAX_FUNCTION_NAME];
    snprintf(destroyFuncName, MAX_FUNCTION_NAME, "Destroy_%s", _script->scriptName);

    // Search the shared object for the function named
    ScriptFuncPtr destroyScriptFunctPtr = (ScriptFuncPtr) dlsym(_scriptSharedObjPtr, destroyFuncName);
    
    // Check for any error after dlsym
    error = dlerror(); 
    if (error != NULL) {
        AF_Log_Error("AF_Script_Bind_Functions: Failed to load Start_%s: %s\n", destroyFuncName, error);
        return;
    }

    _script->destroyFuncPtr = destroyScriptFunctPtr;
}


/*
===============================================================================
AF_Script_Bind_Functions_To_Components
Take in all the entities, and and loop through 
If an entity has a script component with a valid path, then load the script, and then bind
binding the start, update and destroy function ptrs
===============================================================================
*/
inline static void AF_Script_Load_And_Bind_Functions(AF_ECS* _ecs){
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        AF_Entity* entity = &_ecs->entities[i];
        if(AF_HasScriptEnabled(entity) == FALSE){
            continue;
        }

        AF_CScript* script = entity->scripts;

        // attempt to load the script
        script->loadedScriptPtr = AF_Script_Load(script->scriptFullPath);

        // attempt the bind the scripts functions to this component
        AF_Script_Bind_Functions(script, script->loadedScriptPtr);
    }
    AF_Log("AF_Script_Load_And_Bind_Functions: Loaded and bound all scripts, Finished \n");
}

/*
===============================================================================
AF_Script_UnLoad
Unload all the loaded script objects
Return the pointer to the shared object
===============================================================================
*/
inline static void AF_Script_UnLoad(void* _scriptSharedObjPtr){
    
    if(_scriptSharedObjPtr == NULL){
        AF_Log_Error("AF_Script_UnLoad: Failed to unload scriptSharedObjPtr due to passing null reference\n");
        return;
    }

    int eret;
    // Close the shared objects
    eret = dlclose(_scriptSharedObjPtr);
    if (eret != 0) {
        AF_Log_Error("AF_Script_UnLoad: Failed to close shared object 1: %s\n", dlerror());
    }
}

/*
===============================================================================
AF_Script_UnloadScripts
Take in all the entities, and and loop through 
If an entity has a script component with a valid path, then unload the script, 
===============================================================================
*/
inline static void AF_Script_UnloadScripts(AF_ECS* _ecs){
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        AF_Entity* entity = &_ecs->entities[i];
        if(AF_HasScriptEnabled(entity) == FALSE){
            continue;
        }

        AF_CScript* script = entity->scripts;

        // attempt to unload the script
        AF_Script_UnLoad(script->loadedScriptPtr);
        
        // set the script ptrs to null
        script->startFuncPtr = NULL;
        script->updateFuncPtr = NULL;
        script->destroyFuncPtr = NULL;
    }
    AF_Log("AF_Script_UnloadScripts: Unload Finished \n");
}



/*
===============================================================================
AF_GetScriptFuncPtr
Take in a script func name, and loaded shared object ptr, and attempt to find the function in the .o object as a shared object.
Return the pointer to the shared object
===============================================================================
*/
inline static ScriptFuncPtr AF_GetScriptFuncPtr(void* _sharedObjectPtr, const char* _funcName){
    // Load functions from the shared objects
    ScriptFuncPtr scriptFunctPtr1 = (ScriptFuncPtr) dlsym(_sharedObjectPtr, _funcName);
    char *error = dlerror(); // Check for any error after dlsym
    if (error != NULL) {
        AF_Log_Error("Game_App_Awake: Failed to load %s: %s\n", _funcName, error);
    }
    return scriptFunctPtr1;
}


/*
===============================================================================
AF_Script_Call_Start
Loop through all script components that have valid script func pointers and call start
===============================================================================
*/
inline static void AF_Script_Call_Start(AF_AppData* _appData){
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        
        AF_Entity* entity = &_appData->ecs.entities[i];
        if(AF_HasScriptEnabled(entity) == FALSE){
            continue;
        }
        
        AF_CScript* script = entity->scripts;
        if(script->startFuncPtr == NULL){
            AF_Log_Error("AF_CallScriptStart: script startFuncPtr is null. Forgot to set it\n");
            continue;
        }

        // Call the function
        // Cast to special func ptr
        ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->startFuncPtr;
        // Call it
        scriptFunctPtr(entity, _appData);
    }
}

/*
===============================================================================
AF_Script_Call_Update
Loop through all script components that have valid script func pointers and call Update
===============================================================================
*/
inline static void AF_Script_Call_Update(AF_AppData* _appData){
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        AF_Entity* entity = &_appData->ecs.entities[i];
        if(AF_HasScriptEnabled(entity) == FALSE){
            continue;
        }
        
        AF_CScript* script = entity->scripts;
        if(script->updateFuncPtr == NULL){
            AF_Log_Error("AF_CallScriptUpdate: script updateFuncPtr is null. Forgot to set it\n");
            continue;
        }

        // Call the function
        // Cast to special func ptr
        ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->updateFuncPtr;
        // Call it
        scriptFunctPtr(entity, _appData);
    }
}

/*
===============================================================================
AF_Script_Call_Destroy
Loop through all script components that have valid script func pointers and call Destroy
===============================================================================
*/
inline static void AF_Script_Call_Destroy(AF_AppData* _appData){
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        AF_Entity* entity = &_appData->ecs.entities[i];
        if(AF_HasScriptEnabled(entity) == FALSE){
            continue;
        }
        
        AF_CScript* script = entity->scripts;
        if(script->destroyFuncPtr == NULL){
            AF_Log_Error("AF_CallScriptDestroy: script destroyFuncPtr is null. Forgot to set it\n");
            continue;
        }

        // Call the function
        // Cast to special func ptr
        ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->destroyFuncPtr;
        // Call it
        scriptFunctPtr(entity, _appData);
    }
}


#ifdef __cplusplus
}
#endif

#endif