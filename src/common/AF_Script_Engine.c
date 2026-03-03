#include "AF_Script_Engine.h"
#include "AF_File.h"



// ===============================================================================
// AF_Script_Load
// Take in a file path and attempt to load the script .o object as a shared object.AF_File_CloseFile
// Return the pointer to the shared object
// ===============================================================================
void* AF_Script_Load(const char* _filePath){
    void* scriptPtr = NULL;
#ifdef _WIN32
    AF_Log_Error("AF_Script_Load: Windows not defined\n");
#else
    // Check file path 
    if(strncmp(_filePath, "", MAX_PROJECTDATA_FILE_PATH) == 0){
        AF_Log_Error("AF_Script_Load: file path is empty\n");
        return NULL;
    }

    // Open a file for binary reading
    FILE* file = AF_File_OpenFile(_filePath, "rb");
    
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
#endif

    return scriptPtr;
}


// ===============================================================================
// AF_Script_Bind_Functions
// Take in a script component, and the loaded script shared object
// Bind the script shared objects start, update, and destroy function points to the component
// ===============================================================================
uint32_t AF_Script_Bind_Functions(AF_CScript* _script, void* _scriptSharedObjPtr){
    if(_script == NULL || _scriptSharedObjPtr == NULL){
        AF_Log_Error("AF_Script_Bind_Functions: passed a null reference\n");
        return AF_FAIL;
    }

#ifdef _WIN32
    AF_Log_Error("AF_Script_Bind_Functions: Windows not defined\n");
    return AF_FAIL;
#else
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
        return AF_FAIL;
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
        return AF_FAIL;
    }

    _script->updateFuncPtr = updateScriptFunctPtr;

    // ==== Late Update Func ==== 
    // Get the script name
    char lateUpdateFuncName[MAX_FUNCTION_NAME];
    snprintf(lateUpdateFuncName, MAX_FUNCTION_NAME, "LateUpdate_%s", _script->scriptName);

    // Search the shared object for the function named
    ScriptFuncPtr lateUpdateScriptFunctPtr = (ScriptFuncPtr) dlsym(_scriptSharedObjPtr, lateUpdateFuncName);
    
    // Check for any error after dlsym
    error = dlerror(); 
    if (error != NULL) {
        AF_Log_Error("AF_Script_Bind_Functions: Failed to load LateUpdate_%s: %s\n", lateUpdateFuncName, error);
        return AF_FAIL;
    }

    _script->lateUpdateFuncPtr = lateUpdateScriptFunctPtr;


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
        return AF_FAIL;
    }

    _script->destroyFuncPtr = destroyScriptFunctPtr;
#endif

    return AF_SUCCESS;
}


// ===============================================================================
// AF_Script_Bind_Functions_To_Components
// Take in all the entities, and and loop through 
// If an entity has a script component with a valid path, then load the script, and then bind
// binding the start, update and destroy function ptrs
// ===============================================================================
void AF_Script_Load_And_Bind_Functions(AF_ECS* _ecs){
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        //AF_Entity* entity = &_ecs->entities[i];
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &_ecs->scripts[scriptID];// entity->scripts[j];
            
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
            // set the correct script path as the build location may have changed.
            //snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "bin/%s/scripts/%s.so", AF_Platform_Mappings[_AppData->projectData.platformData.platformType].name, script->scriptName);
            snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "scripts/%s.so", script->scriptName);
            // attempt to load the script
            script->loadedScriptPtr = AF_Script_Load(script->scriptFullPath);
    
            // attempt the bind the scripts functions to this component
            uint32_t scriptBindSuccess =  AF_Script_Bind_Functions(script, script->loadedScriptPtr);
            if(scriptBindSuccess == AF_FAIL){
                AF_Log_Error("AF_Script_Load_And_Bind_Functions: failed to bind script: Entity: %i, Script: %i\n", i, scriptID);
            }
        }
        
    }
    AF_Log("AF_Script_Load_And_Bind_Functions: Loaded and bound all scripts, Finished \n");
}

// ===============================================================================
// AF_Script_UnLoad
// Unload all the loaded script objects
// Return the pointer to the shared object
// ===============================================================================
void AF_Script_UnLoad(void* _scriptSharedObjPtr){
    
    if(_scriptSharedObjPtr == NULL){
        //AF_Log_Error("AF_Script_UnLoad: Failed to unload scriptSharedObjPtr due to passing null reference\n");
        return;
    }

    int eret;
    // Close the shared objects
#ifdef _WIN32
    AF_Log_Error("AF_Script_UnLoad: Windows not defined\n");
#else
    eret = dlclose(_scriptSharedObjPtr);
    if (eret != 0) {
        AF_Log_Error("AF_Script_UnLoad: Failed to close shared object 1: %s\n", dlerror());
    }
#endif
}


// ===============================================================================
// AF_Script_UnloadScripts
// Take in all the entities, and and loop through 
// If an entity has a script component with a valid path, then unload the script, 
// ===============================================================================
void AF_Script_UnloadScripts(AF_ECS* _ecs){
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        AF_Entity* entity = &_ecs->entities[i];
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &_ecs->scripts[scriptID];// entity->scripts[j];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
    
            // attempt to unload the script
            AF_Script_UnLoad(script->loadedScriptPtr);
            
            // set the script ptrs to null
            script->startFuncPtr = NULL;
            script->updateFuncPtr = NULL;
            script->lateUpdateFuncPtr = NULL;
            script->destroyFuncPtr = NULL;
        }
    }
    AF_Log("AF_Script_UnloadScripts: Unload Finished \n");
}


// ===============================================================================
// AF_GetScriptFuncPtr
// Take in a script func name, and loaded shared object ptr, and attempt to find the function in the .o object as a shared object.
// Return the pointer to the shared object
// ===============================================================================
ScriptFuncPtr AF_GetScriptFuncPtr(void* _sharedObjectPtr, const char* _funcName){
    // Load functions from the shared objects
    ScriptFuncPtr scriptFunctPtr1 = NULL;
#ifdef _WIN32
    AF_Log_Error("AF_GetScriptFuncPtr: Windows not defined\n");
#else
    scriptFunctPtr1 = (ScriptFuncPtr) dlsym(_sharedObjectPtr, _funcName);
    char *error = dlerror(); // Check for any error after dlsym
    if (error != NULL) {
        AF_Log_Error("Game_App_Awake: Failed to load %s: %s\n", _funcName, error);
    }
#endif
    return scriptFunctPtr1;
}


// ===============================================================================
// AF_Script_Call_Start
// Loop through all script components that have valid script func pointers and call start
// ===============================================================================
void AF_Script_Call_Start(AF_AppData* _appData){
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        
        
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];// entity->scripts[j];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
            
            if(script->startFuncPtr == NULL){
                AF_Log_Error("AF_CallScriptStart: script startFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->startFuncPtr;
            // Call it
            scriptFunctPtr(i, _appData);
        }
    }
}

// ===============================================================================
// AF_Script_Call_Update
// Loop through all script components that have valid script func pointers and call Update
// ===============================================================================
void AF_Script_Call_Update(AF_AppData* _appData){
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        // Run all the scripts
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
        
            if(script->updateFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptUpdate: script updateFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->updateFuncPtr;
            // Call it passing the entity ID and reference to the game data
            scriptFunctPtr(i, _appData);
        }
    }
}

// ===============================================================================
// AF_Script_Call_LateUpdate
// Loop through all script components that have valid script func pointers and call Update
// ===============================================================================
void AF_Script_Call_LateUpdate(AF_AppData* _appData){
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        // Run all the scripts
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
        
            if(script->lateUpdateFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptUpdate: script updateFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->lateUpdateFuncPtr;
            // Call it passing the entity ID and reference to the game data
            scriptFunctPtr(i, _appData);
        }
    }
}


// ===============================================================================
// AF_Script_Call_Destroy
// Loop through all script components that have valid script func pointers and call Destroy
// ===============================================================================
void AF_Script_Call_Destroy(AF_AppData* _appData){
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
    
            if(script->destroyFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptDestroy: script destroyFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->destroyFuncPtr;
            // Call it
            scriptFunctPtr(j, _appData);
        }
    }
}

// ===============================================================================
// AF_Script_SerialiseEditorVars
// serialise script editor vars
// ===============================================================================
void AF_Script_SerialiseEditorVars(const char *_scriptPath, AF_CScript *_scriptComponent)
{       
    (void) _scriptComponent;
    uint32_t scriptSize = AF_File_GetFileSize(_scriptPath);
    if(scriptSize == 0){
        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to get script file size for path: %s\n", _scriptPath);
        return;
    }
    // fileopen the script.
    uint32_t scriptBufferSize = scriptSize*sizeof(char) + 1;
    char* scriptBuffer = malloc(scriptBufferSize);
    if(scriptBuffer == NULL){
        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to allocate memory for script buffer\n");
        return;
    }
    af_bool_t readSuccess = AF_File_ReadFile(scriptBuffer, scriptBufferSize, _scriptPath, "r");

    // search the script line by line for EDITOR_VAR tags.
    if(readSuccess == AF_FALSE){
        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to read script file at path: %s\n", _scriptPath);
        free(scriptBuffer);
        scriptBuffer = NULL;
        return;
    }

    // If found Tag, line under EDITOR_VAR is the variable. 
    // space delimiter
    char delimiter[] = " \t\r\n";

    // declare empty string to store token
    char* token = NULL;

    // Get the first token
    token = strtok(scriptBuffer, delimiter);

    af_bool_t foundEditorVar = AF_FALSE;
    // continue upto the last token
    while(token != NULL){
            
        //AF_Log("AF_Script_SerialiseEditorVars: Token: %s\n", token);
        // pass null to get next token
        token = strtok(NULL, delimiter);
        //AF_Log("AF_Script_SerialiseEditorVars: Token: %s\n", token);
        if (token != NULL && strcmp(token, "AF_EDITOR_VAR") == 0) {
            // Get the NEXT token which should be the type (int, float, etc)
            token = strtok(NULL, delimiter); 
            if (token) {
                
                AF_EDITOR_VAR_TYPE_e varType = AF_Script_MapStringToEditorVarType(token);
                
                if(varType == AF_EDITOR_VAR_TYPE_TOTAL_TYPES){
                    AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to map var type string to enum for var: %s\n", token);
                    continue;   
                }

                if(token == NULL){
                    AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to get var type token\n");
                    continue;
                }

                // set the variable type
                _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].type = varType;

                AF_Log("Type: %s ", token);
                token = strtok(NULL, delimiter); 
                

                // progress the token to get the var name
                AF_Log("Name: %s ", token);
                // set the name
                snprintf(_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].name, AF_MAX_PATH_CHAR_SIZE, "%s", token);

                token = strtok(NULL, delimiter); 
                
                
                // store the data
                size_t currentOffset = _scriptComponent->scriptEditorVarCount * 16; // max size of editor var is 16 bytes (vec3)
                _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].offset = currentOffset;
                
                _scriptComponent->scriptEditorVarCount++;
                AF_Log("Value: %s\n", token);
                
            }

        }
    }

    
        // First token is the type, second token is the name, third is assignment, forth is the value.
        // Determine what type the first token is and match with enum types.
        // determine if forth value token is a string or number
        // determine if number token 
        // convert number token relevent number
    // assign new variable into component var data slot.
    //AF_Log("%s\n", scriptBuffer);
    free(scriptBuffer);
    scriptBuffer = NULL;
}


AF_EDITOR_VAR_TYPE_e AF_Script_MapStringToEditorVarType(const char* _typeString){
    if(strcmp(_typeString, "uint32_t") == 0 || strcmp(_typeString, "int") == 0){
        return AF_EDITOR_VAR_TYPE_INT;
    } else if(strcmp(_typeString, "float") == 0 || strcmp(_typeString, "AF_FLOAT") == 0){
        return AF_EDITOR_VAR_TYPE_FLOAT;
    }
    else if(strcmp(_typeString, "af_bool_t") == 0 || strcmp(_typeString, "bool") == 0){
        return AF_EDITOR_VAR_TYPE_BOOL;
    }
    else if(strcmp(_typeString, "char*") == 0 || strcmp(_typeString, "char") == 0){
        return AF_EDITOR_VAR_TYPE_STRING;
    }else if(strcmp(_typeString, "Vec3") == 0){
        return AF_EDITOR_VAR_TYPE_VEC3;
    } 
    else {
        AF_Log_Error("AF_Script_MapStringToEditorVarType: Var Type %s not recognised\n", _typeString);
        return AF_EDITOR_VAR_TYPE_TOTAL_TYPES; // Return an invalid type to indicate error
    }
}
