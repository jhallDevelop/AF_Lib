/*
===============================================================================
AF_PROJECT_H


===============================================================================
*/
#ifndef AF_PROJECT_H
#define AF_PROJECT_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AF_AppData.h"
#include "AF_Assets.h"
#include "AF_File.h"
#include "AF_Renderer.h"
#include "AF_MeshLoad.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}


/*
================
AF_Project_SyncEntities
Sync the entities loaded
================
*/
inline static void AF_Project_SyncEntities(AF_AppData* _appData){
    
    // TODO: maybe put this back in
    AF_Log_Warning("AF_Project_SyncEntities: disabled destroying mesh buffers before ecs sync. may need to re-implement. Currently cause\n"); 
    /*
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i){
        BOOL hasMesh = AF_Component_GetHas(_appData->ecs.meshes[i].enabled);
        if(hasMesh == FALSE){
            continue;
        }
        AF_Renderer_DestroyMeshBuffers(&_appData->ecs.meshes[i]);
    }*/
    
    // Load ecs data from file
    // resync the pointers so we don't get null reference
	AF_ECS_ReSyncComponents(&_appData->ecs);

    // Reset the assets loaded,
    _appData->assets = AF_Assets_ZERO();

    // Reset the frame buffer data as it will be re-loaded
    _appData->rendererData.screenFrameBufferData.fbo = 0;
    _appData->rendererData.depthFrameBufferData.fbo = 0;
    _appData->rendererData.depthDebugFrameBufferData.fbo = 0;
    
    // Load Reload meshes
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i){
        
        BOOL hasMesh = AF_Component_GetHas(_appData->ecs.meshes[i].enabled);
        if(hasMesh == FALSE){
            continue;
        }
        
        // init the mesh
        BOOL meshLoadSuccess = AF_MeshLoad_Load(&_appData->assets, &_appData->ecs.meshes[i], _appData->ecs.meshes[i].meshPath);
        if(meshLoadSuccess == false){
            AF_Log_Error("AF_Project_Load: Failed to load mesh %s\n", _appData->ecs.meshes[i].meshPath);
            continue;
        }
    }
}

/*
================
AF_Project_Load
Take a file path and open the game.proj file if it can be found.
================
*/
inline static bool AF_Project_Load(AF_AppData* _appData, const char* _filePath){
    // Open a file for binary reading
    FILE* file = AF_File_OpenFile(_filePath, "rb");
    if (file == NULL) {
        AF_Log_Error("AF_Project_Load: FAILED to open file %s\n", _filePath);
        return false;
    } 

    
    fread(_appData, sizeof(AF_AppData), 1, file); // Read the struct
	if (ferror(file)) {
        printf("AF_Project_Load: Error while reading appData file %s \n", _filePath);
		return false;
    }

    // Clean up the render objects first as some data is malloc
    
    
    
    /*
    // buffer reading the line
    char line[256];
    bool found = false;

    while(fgets(line, sizeof(line), file)){
        // split the string at "PLATFORM = "
        char* token = strstr(line, PLATFORM_STRING);
        if(token){
                // move pointer past "PLATFORM ="
                token += strlen(PLATFORM_STRING);
                // skip any leading whitespace
                while(*token == ' ' || *token =='\t') 
                {
                    token++;
                }

                // extract the word (stop at space or newline
                char word[MAX_PLATFORM_LEN + 1];
                int i = 0;
                // Copy while valid character, not whitespace/newline, and buffer has space
                while (token[i] && token[i] != ' ' && token[i] != '\n' && token[i] != '\t' && i < MAX_PLATFORM_LEN) {
                    word[i] = token[i];
                    i++;
                }
                word[i] = '\0'; // null terminator
                
                // check if we extracted something or just whitespace
                if(i > 0){
                    // if word found is a int value, its likely one of our platform build types
                    // if the value is larger than 16, its likely there is an error in the data being read.
                    if(isdigit(*word) == false){
                        return false;
                    }
                    
                    // convert from ascii char to int
                    int platformDigit = *word - '0';
                    // range check if the result is a mess
                    if(platformDigit < 0 || platformDigit > 16){
                        return false;
                    }
                    AF_Log("AF_Project_Load: save platform digit %i\n", platformDigit);
                    // Do platform loading stuff
                    AF_ProjectData* AF_ProjectData = &_appData->projectData;

                    //AF_ProjectData->name = 
                    // save the platform type as a enum value
                    //platformData->platformType = (AF_Platform_e)platformDigit;

                    found = true;
                    break; // Exit the loop once found (if only first occurrence is needed)
                }else {
                    // Found "PLATFORM = " but it was followed only by whitespace or nothing on the line
                    AF_Log_Warning("AF_Project_Load: Found '%s' but no value following it on the line in %s\n", PLATFORM_STRING, _filePath);
                    // Continue searching subsequent lines? Or treat as error? Depends on requirements.
                    // If finding the line but no value is an error, set found = false and break, or return false here.
               }

                
            }
        }

    // Close the file - *AFTER* the loop
    fclose(file);

    // Log error only if the loop finished without finding the string
    if (!found) {
        AF_Log_Error("AF_Project_Load: Could not find valid '%s' line in file %s\n", PLATFORM_STRING, _filePath);
    }
    */
    // Return true only if the platform string was successfully found and extracted
    return true;
}






#endif


#endif // AF_PROJECT_H
