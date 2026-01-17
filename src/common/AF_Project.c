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
    // update thje model matrix for all transforms
    for (uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i) {
        af_bool_t hasTransform = AF_Component_GetHas(_appData->ecs.transforms[i].enabled);
        if (hasTransform == AF_FALSE) {
            continue;
        }
        AF_CTransform3D* transform = &_appData->ecs.transforms[i];
        transform->modelMat = Mat4_ToModelMat4(transform->pos, transform->rot, transform->scale);
    }       

    // Reset the assets loaded,
    _appData->assets = AF_Assets_ZERO();

    // Load Reload meshes
    for (uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i) {
        // Load Mesh
        af_bool_t hasMesh = AF_Component_GetHas(_appData->ecs.meshes[i].enabled);
        if (hasMesh == AF_TRUE) {
            
            // init the mesh
            AF_CMesh* meshComponent = &_appData->ecs.meshes[i];
            af_bool_t meshLoadSuccess = AF_MeshLoad_InitMesh(&_appData->assets, meshComponent, meshComponent->meshPath);
            AF_Renderer_ReLoadTexture(&_appData->assets, &meshComponent->material.diffuseTexture);
            //af_bool_t meshLoadSuccess = AF_MeshLoad_Load(&_appData->assets, &_appData->ecs.meshes[i], _appData->ecs.meshes[i].meshPath);
            if (meshLoadSuccess == false) {
                AF_Log_Error("AF_Project_Load: Failed to load mesh %s\n", _appData->ecs.meshes[i].meshPath);
                continue;
            }
        }

        // Terrain textures
        af_bool_t hasTerrain = AF_Component_GetHas(_appData->ecs.terrains[i].enabled);
        if (hasTerrain == AF_TRUE) {
            AF_CTerrain* terrainComponent = &_appData->ecs.terrains[i];
            terrainComponent->heightmapTextureID = AF_Renderer_LoadTexture(terrainComponent->heightMapPath);
            AF_CMesh* meshComponent = &_appData->ecs.meshes[i];
            AF_Renderer_InitInstancedTerrainMeshBuffer(terrainComponent->gridSize, meshComponent);
        }   

        // Load Sprite Mesh components
        af_bool_t hasSprite = AF_Component_GetHas(_appData->ecs.sprites[i].enabled);
        if (hasSprite == AF_TRUE) {
            AF_CSprite* spriteComponent = &_appData->ecs.sprites[i];
            
            // Reload the sprite's mesh and texture from their file paths.
            // AF_MeshLoad_FromFile will handle loading the model data and shader.
            AF_Renderer_InitSpriteMeshBuffer(spriteComponent);
            spriteComponent->spriteMesh.shader.shaderID = AF_MeshLoad_Shader_LoadFromAssets(&_appData->assets, spriteComponent->spriteMesh.shader.vertPath, spriteComponent->spriteMesh.shader.fragPath);
            //snprintf(spriteComponent->spriteMesh.material.diffuseTexture.path, AF_MAX_PATH_CHAR_SIZE, "assets/textures/%s", spriteComponent->spriteMesh.material.diffuseTexture.path);
            AF_Renderer_ReLoadTexture(&_appData->assets, &spriteComponent->spriteMesh.material.diffuseTexture);
            
        }

        // Load Font/Mesh for text components
        af_bool_t hasText = AF_Component_GetHas(_appData->ecs.texts[i].enabled);
        if (hasText == AF_TRUE) {
            AF_CText* textComponent = &_appData->ecs.texts[i];
            uint32_t fontSize = 1;
            if(textComponent->font.fontSize > 0) {
                fontSize = textComponent->font.fontSize;
            } 

            // Load the shader for the text mesh
            uint32_t textShaderID = AF_MeshLoad_Shader_LoadFromAssets(&_appData->assets, textComponent->mesh.shader.vertPath, textComponent->mesh.shader.fragPath); 
            textComponent->mesh.shader.shaderID = textShaderID;

            // Load the font
            af_bool_t fontLoadSuccess = AF_LoadFont(&textComponent->font);
            if(fontLoadSuccess == AF_FALSE) {
                AF_Log_Error("AF_Project_Load: Failed to load font %s for text component\n", textComponent->font.fontPath);
                continue;
            }
            // Init the font mesh
            af_bool_t fontMeshLoadSuccess = AF_MeshLoad_InitTextMesh(&_appData->assets, textComponent, textComponent->fontPath, fontSize);
            if (fontMeshLoadSuccess == AF_FALSE) {
                AF_Log_Error("AF_Project_Load: Failed to load font %s for text component\n", textComponent->fontPath);
                continue;
            }
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

