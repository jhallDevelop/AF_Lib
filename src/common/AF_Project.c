#include "AF_Project.h"
/*
===============================================================================
AF_PROJECT_H


===============================================================================
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AF_Assets.h"
#include "AF_File.h"
#include "AF_Renderer.h"
#include "AF_MeshLoad.h"
#include "ECS/Components/AF_Component.h"




/*
================
AF_Project_SyncEntities
Sync the entities loaded
================
*/
void AF_Project_SyncEntities(AF_AppData* _appData) {

    // TODO: maybe put this back in
    AF_Log_Warning("AF_Project_SyncEntities: disabled destroying mesh buffers before ecs sync. may need to re-implement. Currently cause\n");
    /*
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i){
        af_bool_t hasMesh = AF_Component_GetHas(_appData->ecs.meshes[i].enabled);
        if(hasMesh == AF_FALSE){
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
    for (uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i) {

        af_bool_t hasMesh = AF_Component_GetHas(_appData->ecs.meshes[i].enabled);
        if (hasMesh == AF_FALSE) {
            continue;
        }

        // init the mesh
        af_bool_t meshLoadSuccess = AF_MeshLoad_Load(&_appData->assets, &_appData->ecs.meshes[i], _appData->ecs.meshes[i].meshPath);
        if (meshLoadSuccess == false) {
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
bool AF_Project_Load(AF_AppData* _appData, const char* _filePath) {
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

    // Return true only if the platform string was successfully found and extracted
    return true;
}

