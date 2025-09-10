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
#include "AF_JSON.h"



/*
================
AF_Project_SyncEntities
Sync the entities loaded
================
*/
void AF_Project_SyncEntities(AF_AppData* _appData) {

    // TODO: maybe put this back in
    //AF_Log_Warning("AF_Project_SyncEntities: disabled destroying mesh buffers before ecs sync. may need to re-implement. Currently cause\n");
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

    // Load Reload meshes
    for (uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i) {

        af_bool_t hasMesh = AF_Component_GetHas(_appData->ecs.meshes[i].enabled);
        if (hasMesh == AF_FALSE) {
            continue;
        }

        // init the mesh
        AF_CMesh* meshComponent = &_appData->ecs.meshes[i];
        af_bool_t meshLoadSuccess = AF_MeshLoad_Init(&_appData->assets, meshComponent, meshComponent->meshPath);
        meshComponent->material.diffuseTexture = AF_Renderer_ReLoadTexture(&_appData->assets, meshComponent->material.diffuseTexture.path);
        //af_bool_t meshLoadSuccess = AF_MeshLoad_Load(&_appData->assets, &_appData->ecs.meshes[i], _appData->ecs.meshes[i].meshPath);
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
af_bool_t AF_Project_Load(AF_AppData* _appData, const char* _appDataPath) {


    if(_appData == NULL || _appDataPath == NULL){
        AF_Log_Error("AF_Project_Load: Invalid _appData or _appDataPath, is NULL!\n");
        return AF_FALSE;
    }
    
    // Load scene stored as default scene in the project data
    FILE* appDataFile = AF_File_OpenFile(_appDataPath, "rb");// switch to binary read mode as cause// "r");
    if (appDataFile == NULL) {
        AF_Log_Error("Editor_Utils_OpenProject: Failed to open project data file %s\n", _appData->projectData.defaultScenePath);
        return AF_FALSE;
    }

    af_bool_t result = AF_JSON_LoadProjectDataJson(&_appData->projectData, appDataFile);
    if(result == AF_FALSE){
        AF_Log_Error("AF_Project_Load: Failed to load project data from %s\n", _appDataPath);
        return AF_FALSE;
    }

    if(appDataFile != NULL){
        AF_File_CloseFile(appDataFile);
        appDataFile = NULL;
    }else{
        AF_Log_Warning("AF_Project_Load: Failed to close project data file %s\n", _appDataPath);
        return AF_FALSE;
    }
    

    // for now, clear ECS data as we will load the default scene
    AF_ECS_Init(&_appData->ecs); // Reset the ECS data

    // Load scene stored as default scene in the project data
    FILE* sceneFile = AF_File_OpenFile(_appData->projectData.defaultScenePath, "rb");// switch to binary read mode as cause// "r");
    if (sceneFile == NULL) {
        AF_Log_Error("Editor_Utils_OpenProject: Failed to open default scene file %s\n", _appData->projectData.defaultScenePath);
        return AF_FALSE;
    }

    // Load the JSON data from the file
    AF_Log("Editor_Utils_OpenProject: Loading default scene from %s\n", _appData->projectData.defaultScenePath);
    // Load the JSON data into the ECS
    af_bool_t sceneResult = AF_JSON_LoadSceneJson(_appData, sceneFile);
    if(sceneResult == AF_FALSE){
       AF_Log_Error("Editor_Utils_OpenProject: Failed to load default scene from %s\n", _appData->projectData.defaultScenePath);
       return AF_FALSE;
    }
    AF_File_CloseFile(sceneFile);

    // Clean up the render objects first as some data is malloc
   AF_Log("AF_Project_Load: Finished Loading\n");
    // Return true only if the platform string was successfully found and extracted
    return AF_TRUE;
}

