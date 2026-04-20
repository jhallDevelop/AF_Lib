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
#include "AF_Script_Engine.h"
#include "AF_Physics.h"

// TODO: why is this needed
#include "../../../stb/stb_image.h"

#define GAME_PROJECTS_DIR "game_projects"





// Forward declarations
void AF_Project_SetProjectRoot(AF_AppData* _appData);

// ================
// Editor_SetProjectRoot
// Sets the project root to the editor's project folder, which is the parent of the editor executable.
// ================
void AF_Project_SetProjectRoot(AF_AppData* _appData){
    // Set the correct project root.
    // We are in the editor, so project root should be a path of the folder for the editor, unlike a game which would set project root at "./"
    // Project root will be "cwd/game_projects/game_name"
    char absoluteProjectRoot[AF_MAX_PATH_CHAR_SIZE] = {0};
    // Current working director
    char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
    if (AF_Project_GetCWD(cwd, sizeof(cwd)) == NULL) {
        cwd[0] = '\0';
    }
    snprintf(absoluteProjectRoot, sizeof(absoluteProjectRoot), "%s/%s/%s", cwd, GAME_PROJECTS_DIR, _appData->projectData.name);
    AF_File_NormalisePath(absoluteProjectRoot);
    AF_Log("AF_Project_SetProjectRoot: absolute project root %s\n", absoluteProjectRoot);
    
    // set the project root in app data, which is used by other systems to resolve paths.
    snprintf(_appData->projectData.projectRoot, sizeof(_appData->projectData.projectRoot), "%s", absoluteProjectRoot);
}


// ================
// AF_File_SyncEntities
// Sync the entities loaded
// ================
void AF_Project_SyncEntities(AF_AppData* _appData) {
    // Sync the entities loaded    // Load ecs data from file
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

        // Mesh Path
        // Normalise the mesh path to ensure it's correct relative to the project root
        char absoluteMeshPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        snprintf(absoluteMeshPath, sizeof(absoluteMeshPath), "%s/%s", _appData->projectData.projectRoot, meshComponent->meshPath);
        AF_File_NormalisePath(absoluteMeshPath);
        snprintf(meshComponent->meshPath, sizeof(meshComponent->meshPath), "%s", absoluteMeshPath);

        // Shader Paths
        char absoluteVertexShaderPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        char absoluteFragmentShaderPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        snprintf(absoluteVertexShaderPath, sizeof(absoluteVertexShaderPath), "%s/%s", _appData->projectData.projectRoot, meshComponent->shader.vertPath);
        snprintf(absoluteFragmentShaderPath, sizeof(absoluteFragmentShaderPath), "%s/%s", _appData->projectData.projectRoot, meshComponent->shader.fragPath);
        AF_File_NormalisePath(absoluteVertexShaderPath);
        AF_File_NormalisePath(absoluteFragmentShaderPath);
        snprintf(meshComponent->shader.vertPath, sizeof(meshComponent->shader.vertPath), "%s", absoluteVertexShaderPath);
        snprintf(meshComponent->shader.fragPath, sizeof(meshComponent->shader.fragPath), "%s", absoluteFragmentShaderPath);

        // Texture Paths
        char absoluteDiffuseTexturePath[AF_MAX_PATH_CHAR_SIZE] = {0};
        snprintf(absoluteDiffuseTexturePath, sizeof(absoluteDiffuseTexturePath), "%s/%s", _appData->projectData.projectRoot, meshComponent->material.diffuseTexture.path);
        AF_File_NormalisePath(absoluteDiffuseTexturePath);
        snprintf(meshComponent->material.diffuseTexture.path, sizeof(meshComponent->material.diffuseTexture.path), "%s", absoluteDiffuseTexturePath);




        if(AF_Component_GetHas(meshComponent->enabled) == AF_FALSE){
            continue;
        }

        af_bool_t meshLoadSuccess = AF_MeshLoad_InitMesh(&_appData->assets, meshComponent, meshComponent->meshPath);
        if(meshLoadSuccess == AF_FALSE){
            AF_Log_Error("AF_Project_SyncEntities: Failed to load mesh for entity %u from path %s\n", i, meshComponent->meshPath);
            continue;
        }

        // Load diffuse texture if there is one
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
            //AF_Project_NormaliseScenePath(_appData, terrainComponent->heightMapPath, sizeof(terrainComponent->heightMapPath));
            char absoluteHeightMapPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            snprintf(absoluteHeightMapPath, sizeof(absoluteHeightMapPath), "%s/%s", _appData->projectData.projectRoot, terrainComponent->heightMapPath);
            AF_File_NormalisePath(absoluteHeightMapPath);
                snprintf(terrainComponent->heightMapPath, sizeof(terrainComponent->heightMapPath), "%s", absoluteHeightMapPath);

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

            
            // Shader Paths
            char absoluteVertexShaderPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            char absoluteFragmentShaderPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            snprintf(absoluteVertexShaderPath, sizeof(absoluteVertexShaderPath), "%s/%s", _appData->projectData.projectRoot, spriteComponent->spriteMesh.shader.vertPath);
            snprintf(absoluteFragmentShaderPath, sizeof(absoluteFragmentShaderPath), "%s/%s", _appData->projectData.projectRoot, spriteComponent->spriteMesh.shader.fragPath);
            AF_File_NormalisePath(absoluteVertexShaderPath);
            AF_File_NormalisePath(absoluteFragmentShaderPath);
            snprintf(spriteComponent->spriteMesh.shader.vertPath, sizeof(spriteComponent->spriteMesh.shader.vertPath), "%s", absoluteVertexShaderPath);
            snprintf(spriteComponent->spriteMesh.shader.fragPath, sizeof(spriteComponent->spriteMesh.shader.fragPath), "%s", absoluteFragmentShaderPath);

            
            // Reload the sprite's mesh and texture from their file paths.
            // AF_MeshLoad_FromFile will handle loading the model data and shader.
            AF_RendererBuffer_InitSpriteMeshBuffer(spriteComponent);

            // Guard against empty or dummy shader paths
            if (AF_String_IsEmpty(spriteComponent->spriteMesh.shader.vertPath) || 
                AF_String_IsEmpty(spriteComponent->spriteMesh.shader.fragPath) ||
                (strstr(spriteComponent->spriteMesh.shader.vertPath, ".vert") == (spriteComponent->spriteMesh.shader.vertPath + strlen(spriteComponent->spriteMesh.shader.vertPath) - 5) && 
                 (strlen(spriteComponent->spriteMesh.shader.vertPath) < 10))) { // rough check for just ".vert"
                spriteComponent->spriteMesh.shader.shaderID = SHADER_FAILED_TO_LOAD;
            } else {
                spriteComponent->spriteMesh.shader.shaderID = AF_MeshLoad_Shader_LoadFromAssets(&_appData->assets, spriteComponent->spriteMesh.shader.vertPath, spriteComponent->spriteMesh.shader.fragPath);
            }

            // sprite texture paths
            char absoluteDiffuseTexturePath[AF_MAX_PATH_CHAR_SIZE] = {0};
            snprintf(absoluteDiffuseTexturePath, sizeof(absoluteDiffuseTexturePath), "%s/%s", _appData->projectData.projectRoot, spriteComponent->spriteMesh.material.diffuseTexture.path);
            AF_File_NormalisePath(absoluteDiffuseTexturePath);
            snprintf(spriteComponent->spriteMesh.material.diffuseTexture.path, sizeof(spriteComponent->spriteMesh.material.diffuseTexture.path), "%s", absoluteDiffuseTexturePath);

            //snprintf(spriteComponent->spriteMesh.material.diffuseTexture.path, AF_MAX_PATH_CHAR_SIZE, "assets/textures/%s", spriteComponent->spriteMesh.material.diffuseTexture.path);
            if (spriteComponent->spriteMesh.material.diffuseTexture.path[0] != '\0') {
               AF_TextureLoader_ReLoadTexture(&_appData->assets, &spriteComponent->spriteMesh.material.diffuseTexture);
            }
        }

        // Load Font/Mesh for text components
        af_bool_t hasText = AF_Component_GetHas(_appData->ecs.texts[i].enabled);
        if (hasText == AF_TRUE) {
            AF_CText* textComponent = &_appData->ecs.texts[i];
            
            // Shader
            char absoluteVertexShaderPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            char absoluteFragmentShaderPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            snprintf(absoluteVertexShaderPath, sizeof(absoluteVertexShaderPath), "%s/%s", _appData->projectData.projectRoot, textComponent->mesh.shader.vertPath);
            snprintf(absoluteFragmentShaderPath, sizeof(absoluteFragmentShaderPath), "%s/%s", _appData->projectData.projectRoot, textComponent->mesh.shader.fragPath);
            AF_File_NormalisePath(absoluteVertexShaderPath);
            AF_File_NormalisePath(absoluteFragmentShaderPath);
            snprintf(textComponent->mesh.shader.vertPath, sizeof(textComponent->mesh.shader.vertPath), "%s", absoluteVertexShaderPath);
            snprintf(textComponent->mesh.shader.fragPath, sizeof(textComponent->mesh.shader.fragPath), "%s", absoluteFragmentShaderPath);
            
            
            uint32_t fontSize = 1;
            if(textComponent->font.fontSize > 0) {
                fontSize = textComponent->font.fontSize;
            } 

            // Resolve text font path from scene data. Legacy scenes often store relative
            // paths (e.g. "assets/font/...") that should be relative to projectRoot.
            char resolvedFontPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            const char* preferredFontPath = textComponent->font.fontPath;
            if (AF_String_IsEmpty(preferredFontPath) && !AF_String_IsEmpty(textComponent->fontPath)) {
                preferredFontPath = textComponent->fontPath;
            }

            if (!AF_String_IsEmpty(preferredFontPath)) {
                FILE* fontFile = AF_File_Open(preferredFontPath, "rb");
                if (fontFile != NULL) {
                    AF_File_Close(fontFile);
                    snprintf(resolvedFontPath, AF_MAX_PATH_CHAR_SIZE, "%s", preferredFontPath);
                } else if (!AF_String_IsEmpty(_appData->projectData.projectRoot)) {
                    snprintf(
                        resolvedFontPath,
                        AF_MAX_PATH_CHAR_SIZE,
                        "%s/%s",
                        _appData->projectData.projectRoot,
                        preferredFontPath
                    );
                    fontFile = AF_File_Open(resolvedFontPath, "rb");
                    if (fontFile != NULL) {
                        AF_File_Close(fontFile);
                    } else {
                        resolvedFontPath[0] = '\0';
                    }
                }
            }

            // Last-resort fallback so text components still render if project font moved.
            if (AF_String_IsEmpty(resolvedFontPath)) {
                const char* fallbackFontPath = "assets/font/Montserrat/static/Montserrat-Medium.ttf";
                FILE* fallbackFile = AF_File_Open(fallbackFontPath, "rb");
                if (fallbackFile != NULL) {
                    AF_File_Close(fallbackFile);
                    snprintf(resolvedFontPath, AF_MAX_PATH_CHAR_SIZE, "%s", fallbackFontPath);
                    AF_Log_Warning("AF_Project_Load: Falling back to default font for text component %u\n", i);
                }
            }

            if (!AF_String_IsEmpty(resolvedFontPath)) {
                snprintf(textComponent->font.fontPath, AF_MAX_PATH_CHAR_SIZE, "%s", resolvedFontPath);
                snprintf(textComponent->fontPath, AF_MAX_PATH_CHAR_SIZE, "%s", resolvedFontPath);
            }

            // Load the shader for the text mesh
            // Guard against empty or dummy shader paths
            if (AF_String_IsEmpty(textComponent->mesh.shader.vertPath) || 
                AF_String_IsEmpty(textComponent->mesh.shader.fragPath) ||
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
                AF_Log_Warning("AF_Project_Load: Font backend unavailable or font load failed (%s). Text component will be skipped.\n", textComponent->font.fontPath);
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

    // Current working director
    char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
    if (AF_Project_GetCWD(cwd, sizeof(cwd)) == NULL) {
        cwd[0] = '\0';
    }

    // Create the appData Absolute path
    char appDataPathAbsolute[AF_MAX_PATH_CHAR_SIZE] = {0};
    snprintf(appDataPathAbsolute, sizeof(appDataPathAbsolute), "%s/%s", cwd, _appDataPath);
    AF_File_NormalisePath(appDataPathAbsolute);
    
    
    AF_Log("AF_Project_Load: Attempting to load project data from %s\n", appDataPathAbsolute);
    // Load scene stored as default scene in the project data
    FILE* appDataFile = AF_File_Open(appDataPathAbsolute, "rb");// switch to binary read mode as cause// "r");
    if (appDataFile == NULL) {
        AF_Log_Error("AF_Project_Load: Failed to open project data file %s\n", appDataPathAbsolute);
        return AF_FALSE;
    }

    af_bool_t result = AF_JSON_LoadProjectDataJson(&_appData->projectData, appDataFile);
    if(result == AF_FALSE){
        AF_Log_Error("AF_Project_Load: Failed to load project data from %s\n", appDataPathAbsolute);
        return AF_FALSE;
    }

    if(appDataFile != NULL){
        AF_File_Close(appDataFile);
        appDataFile = NULL;
    }else{
        AF_Log_Warning("AF_Project_Load: Failed to close project data file %s\n", appDataPathAbsolute);
        return AF_FALSE;
    }

    // set the project root.
    AF_Project_SetProjectRoot(_appData);

    return AF_TRUE;
}

// TODO: refactor this, should it be exposed as a public function? maybe not, but it is used in multiple places in the editor codebase.

af_bool_t AF_Project_LoadScene(AF_AppData *_appData, const char *_sceneJSONFilePath)
{
    af_bool_t returnValue = AF_FALSE;
    AF_Event_ResetRegistry(&_appData->eventRegistry);

    // Unload everything first
    AF_Script_Call_Destroy(_appData);
    AF_Script_UnloadScripts(&_appData->ecs);

    // Tear down old Bullet world first. Re-init happens after scene JSON is loaded.
    AF_Physics_Shutdown(&_appData->physicsEngineHandle);

    // Clear the ECS to remove all existing entities and components before loading the new scene.
    AF_ECS_Init(&_appData->ecs);


    // Load the ECS from the file
    FILE* sceneFile = AF_File_Open(_sceneJSONFilePath, "rb");
    if (sceneFile == NULL) {
        AF_Log_Error("AF_Project_LoadScene: Failed to open scene file %s\n", _sceneJSONFilePath);
        return AF_FALSE;
    }

    af_bool_t sceneLoaded = AF_JSON_LoadSceneJson(_appData, sceneFile);
    AF_File_Close(sceneFile);

    AF_Log("AF_LoadScene: Finished loading scene from %s\n", _sceneJSONFilePath);

    AF_Project_SyncEntities(_appData);

    // Build Bullet bodies from the freshly loaded/synced ECS scene.
    AF_Physics_Init(&_appData->ecs, &_appData->physicsEngineHandle);
    AF_Renderer_InitCollisionGeomtery(&_appData->ecs);

    AF_Script_Load_And_Bind_Functions(_appData);
    AF_Script_Call_Start(_appData);

    return AF_TRUE;
}

// ================
// AF_Project_RequestSceneChange
// Request a scene change by setting the pendingScenePath and hasPendingSceneChange flag in the project data.
// The main loop should check for hasPendingSceneChange and call AF_Project_LoadScene with the pendingScenePath when true.
// ================
void AF_Project_RequestSceneChange(AF_AppData* _appData, const char* _sceneFilePath) {
    if (_appData == NULL || _sceneFilePath == NULL) {
        return;
    }
    snprintf(_appData->projectData.pendingScenePath, AF_MAX_PATH_CHAR_SIZE, "%s", _sceneFilePath);
    _appData->projectData.hasPendingSceneChange = AF_TRUE;
    AF_Log("AF_Project_RequestSceneChange: Scene change to '%s' queued\n", _sceneFilePath);
}
