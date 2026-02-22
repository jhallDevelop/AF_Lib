#include "AF_Project.h"

// ===============================================================================
// AF_PROJECT_H
// This file contains the implementation of the AF_Project functions, 
// which are responsible for managing project-related operations such as syncing entities, 
// loading and saving project data, and handling project assets. 
// The functions in this file interact with various components of the application, 
// including the ECS (Entity Component System), asset management, 
// and rendering systems to ensure that the project data is correctly 
// loaded and synchronized across different parts of the application.
// ===============================================================================

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AF_Assets.h"
#include "AF_File.h"
#include "AF_Renderer.h"
#include "AF_RendererBuffer.h"
#include "AF_MeshLoad.h"
#include "ECS/Components/AF_Component.h"
#include "AF_JSON.h"
#include "AF_TextureLoader.h"

#include "../../../stb/stb_image.h"


// ================
// AF_Project_SyncEntities
// Sync the entities loaded
// ================
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

    // Update the camera vectors for all cameras
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        AF_CCamera* cam = &_appData->ecs.cameras[i];
        AF_CTransform3D*cameraTransform = &_appData->ecs.transforms[i];
        if(AF_Component_GetHasEnabled(cam->enabled) == AF_TRUE){
            // update the yaw and pitch of the camera from the front vector
            // initialise the yaw and pitch from the camera front vector
            Vec3 front = {0, 0, -1};
            front = AF_Vec4_Quat_RotateVec3(cameraTransform->rot, front);
            // normalise the front vector to ensure it's a unit vector before calculating angles
            front = Vec3_NORMALIZE(front);

            // clamp the pitch 
            AF_FLOAT pitchInput = front.y;
            if(pitchInput > 1.0f){
                pitchInput = 1.0f;
            }
            if(pitchInput < -1.0f){
                pitchInput = -1.0f;
            }

            // convert the pitch and yaw to degrees 
            cam->pitch = AF_Math_Degrees(asinf(pitchInput)); // Calculate pitch from the y component of the front vector

            // safely calulate yaw
            if(fabsf(pitchInput) < 0.9999f){ // avoid gimbal lock singularity
                cam->yaw = AF_Math_Degrees(atan2f(-front.x, -front.z)); 
            }
        }
    }

    // Reset the assets loaded,
    _appData->assets = AF_Assets_ZERO();

    // Load Reload meshes
    for (uint32_t i = 0; i < _appData->ecs.meshSparseSet.count; ++i) {
        // Load Mesh
        AF_CMesh* meshComponent = &_appData->ecs.meshSparseSet.denseComponent[i];
        if(meshComponent == NULL){
             AF_Log_Error("AF_Project_SyncEntities: Failed to get mesh component for entity %u\n", i);
             continue;
        }
        if(AF_Component_GetHas(meshComponent->enabled) == AF_FALSE){
            continue;
        }
        
        af_bool_t meshLoadSuccess = AF_MeshLoad_InitMesh(&_appData->assets, meshComponent, meshComponent->meshPath);
        if(meshLoadSuccess == AF_FALSE){
            AF_Log_Error("AF_Project_SyncEntities: Failed to load mesh for entity %u from path %s\n", i, meshComponent->meshPath);
            continue;
        }
        if (meshComponent->material.diffuseTexture.path[0] != '\0') {
                AF_TextureLoader_ReLoadTexture(&_appData->assets, &meshComponent->material.diffuseTexture);
        }
        AF_Log("AF_Project_SyncEntities: Loaded mesh for entity %u from path %s\n", i, meshComponent->meshPath);
        //af_bool_t meshLoadSuccess = AF_MeshLoad_Load(&_appData->assets, &_appData->ecs.meshes[i], _appData->ecs.meshes[i].meshPath);
        if (meshLoadSuccess == false) {
            AF_Log_Error("AF_Project_Load: Failed to load mesh %s\n", meshComponent->meshPath);
            continue;
        }
        
    }
    
    // Other components to sync:
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i){

        // Terrain textures
        af_bool_t hasTerrain = AF_Component_GetHas(_appData->ecs.terrains[i].enabled);
        if (hasTerrain == AF_TRUE) {
            AF_CTerrain* terrainComponent = &_appData->ecs.terrains[i];
            if (terrainComponent->heightMapPath[0] != '\0') {
                terrainComponent->heightmapTextureID = AF_TextureLoader_LoadTexture(terrainComponent->heightMapPath);
            }
            // Get the mesh component for this entity to init the terrain mesh buffer
            // check 
            uint32_t meshDenseIndex = _appData->ecs.meshSparseSet.sparseEntityIDs[i];
            if(meshDenseIndex == AF_ECS_INVALID_INDEX){
                 AF_Log_Error("AF_Project_SyncEntities: No mesh component found for terrain entity %u\n", i);
                 continue;
            }
            
            AF_CMesh* meshComponent = &_appData->ecs.meshSparseSet.denseComponent[_appData->ecs.meshSparseSet.sparseEntityIDs[i]];
             if(meshComponent == NULL){
                 AF_Log_Error("AF_Project_SyncEntities: Failed to get mesh component for terrain entity %u\n", i);
                 continue;
             }
            AF_RendererBuffer_InitInstancedTerrainMeshBuffer(terrainComponent->gridSize, meshComponent);

            // Generate heightmap data on CPU
            if (terrainComponent->heightmapTextureID != 0) {
                // read the texture data, converting it into a heightmap array
                int width, height, nrComponents;
	
                // Force stbi_load to use 1 channel (last argument '1')
                unsigned char *data = stbi_load(terrainComponent->heightMapPath, &width, &height, &nrComponents, 1);
                terrainComponent->heightMapWidth = (uint32_t)width;
                terrainComponent->heightMapHeight = (uint32_t)height;
                

                // Check if data is loaded
                if (data != NULL) {
                    // Allocate memory for heightmap data
                    size_t dataSize = width * height * sizeof(unsigned char);


                    
                    terrainComponent->heightMapData = (unsigned char*)malloc(dataSize);
                    if (terrainComponent->heightMapData != NULL) {
                        //memcpy(terrainComponent->heightMapData, data, dataSize);
                        // VALIDATION: Find max height once, don't log every pixel
                        unsigned char maxVal = 0;
                        for (int z = 0; z < width * height; z++) {
                            terrainComponent->heightMapData[z] = data[z];
                            //AF_Log("Heightmap Sync: Height at index %d is %u\n", z, data[z]);
                        }

                    } else {
                        AF_Log_Error("AF_Project_Load: Failed to allocate memory for heightmap data for terrain component %u\n", i);
                    }
                    stbi_image_free(data); // Free original image data
                } else {
                    AF_Log_Error("AF_Project_Load: Failed to load heightmap image %s for terrain component %u\n", terrainComponent->heightMapPath, i);
                }
            }
        }   

        // Load Sprite Mesh components
        af_bool_t hasSprite = AF_Component_GetHas(_appData->ecs.sprites[i].enabled);
        if (hasSprite == AF_TRUE) {
            AF_CSprite* spriteComponent = &_appData->ecs.sprites[i];
            
            // Reload the sprite's mesh and texture from their file paths.
            // AF_MeshLoad_FromFile will handle loading the model data and shader.
            AF_RendererBuffer_InitSpriteMeshBuffer(spriteComponent);

            // Guard against empty or dummy shader paths
            if (AF_STRING_IS_EMPTY(spriteComponent->spriteMesh.shader.vertPath) || 
                AF_STRING_IS_EMPTY(spriteComponent->spriteMesh.shader.fragPath) ||
                (strstr(spriteComponent->spriteMesh.shader.vertPath, ".vert") == (spriteComponent->spriteMesh.shader.vertPath + strlen(spriteComponent->spriteMesh.shader.vertPath) - 5) && 
                 (strlen(spriteComponent->spriteMesh.shader.vertPath) < 10))) { // rough check for just ".vert"
                spriteComponent->spriteMesh.shader.shaderID = SHADER_FAILED_TO_LOAD;
            } else {
                spriteComponent->spriteMesh.shader.shaderID = AF_MeshLoad_Shader_LoadFromAssets(&_appData->assets, spriteComponent->spriteMesh.shader.vertPath, spriteComponent->spriteMesh.shader.fragPath);
            }

            //snprintf(spriteComponent->spriteMesh.material.diffuseTexture.path, AF_MAX_PATH_CHAR_SIZE, "assets/textures/%s", spriteComponent->spriteMesh.material.diffuseTexture.path);
            if (spriteComponent->spriteMesh.material.diffuseTexture.path[0] != '\0') {
               AF_TextureLoader_ReLoadTexture(&_appData->assets, &spriteComponent->spriteMesh.material.diffuseTexture);
            }
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
            // Guard against empty or dummy shader paths
            if (AF_STRING_IS_EMPTY(textComponent->mesh.shader.vertPath) || 
                AF_STRING_IS_EMPTY(textComponent->mesh.shader.fragPath) ||
                (strstr(textComponent->mesh.shader.vertPath, ".vert") == (textComponent->mesh.shader.vertPath + strlen(textComponent->mesh.shader.vertPath) - 5) && 
                (strlen(textComponent->mesh.shader.vertPath) < 10))) {
                textComponent->mesh.shader.shaderID = SHADER_FAILED_TO_LOAD;
            } else {
                uint32_t textShaderID = AF_MeshLoad_Shader_LoadFromAssets(&_appData->assets, textComponent->mesh.shader.vertPath, textComponent->mesh.shader.fragPath); 
                textComponent->mesh.shader.shaderID = textShaderID;
            }

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


// ================
// AF_Project_Load
// Take a file path and open the game.proj file if it can be found.
// ================
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

