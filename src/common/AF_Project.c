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
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define AF_PROJECT_GETCWD _getcwd
#define AF_PROJECT_ACCESS _access
#else
#include <unistd.h>
#define AF_PROJECT_GETCWD getcwd
#define AF_PROJECT_ACCESS access
#endif
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

#include "../../../stb/stb_image.h"

static af_bool_t AF_Project_IsAbsolutePath(const char* _path) {
    if (_path == NULL || _path[0] == '\0') {
        return AF_FALSE;
    }

    if (_path[0] == '/' || _path[0] == '\\') {
        return AF_TRUE;
    }

#ifdef _WIN32
    if (strlen(_path) > 1 && _path[1] == ':') {
        return AF_TRUE;
    }
#endif

    return AF_FALSE;
}

static af_bool_t AF_Project_PathExists(const char* _path) {
    if (_path == NULL || _path[0] == '\0') {
        return AF_FALSE;
    }
    return (AF_PROJECT_ACCESS(_path, 0) == 0) ? AF_TRUE : AF_FALSE;
}

static void AF_Project_MakeAbsolutePath(char* _path, uint32_t _pathSize, const char* _baseDir) {
    if (_path == NULL || _path[0] == '\0' || _baseDir == NULL || _baseDir[0] == '\0') {
        return;
    }
    if (AF_Project_IsAbsolutePath(_path) == AF_TRUE) {
        return;
    }

    char originalPath[MAX_PROJECTDATA_FILE_PATH];
    snprintf(originalPath, sizeof(originalPath), "%s", _path);
    snprintf(_path, _pathSize, "%s/%s", _baseDir, originalPath);
}

static void AF_Project_GetDirectoryPath(const char* _filePath, char* _directoryPath, uint32_t _directoryPathSize) {
    if (_directoryPath == NULL || _directoryPathSize == 0) {
        return;
    }

    _directoryPath[0] = '\0';
    if (_filePath == NULL || _filePath[0] == '\0') {
        return;
    }

    snprintf(_directoryPath, _directoryPathSize, "%s", _filePath);
    char* lastSlash = strrchr(_directoryPath, '/');
    char* lastBackslash = strrchr(_directoryPath, '\\');
    char* separator = lastSlash;
    if (lastBackslash != NULL && (separator == NULL || lastBackslash > separator)) {
        separator = lastBackslash;
    }

    if (separator != NULL) {
        *separator = '\0';
    } else {
        _directoryPath[0] = '\0';
    }
}

static void AF_Project_ResolveShaderPathPlatform(AF_AppData* _appData, char* _shaderPath, uint32_t _shaderPathSize);

static void AF_Project_GetFileNameOnly(const char* _path, char* _outFileName, uint32_t _outFileNameSize) {
    if (_outFileName == NULL || _outFileNameSize == 0) {
        return;
    }
    _outFileName[0] = '\0';

    if (_path == NULL || _path[0] == '\0') {
        return;
    }

    const char* lastSlash = strrchr(_path, '/');
    const char* lastBackslash = strrchr(_path, '\\');
    const char* fileName = _path;

    if (lastSlash != NULL && lastBackslash != NULL) {
        fileName = (lastSlash > lastBackslash) ? (lastSlash + 1) : (lastBackslash + 1);
    } else if (lastSlash != NULL) {
        fileName = lastSlash + 1;
    } else if (lastBackslash != NULL) {
        fileName = lastBackslash + 1;
    }

    snprintf(_outFileName, _outFileNameSize, "%s", fileName);
}

static void AF_Project_GetFileStemNoExt(const char* _fileName, char* _outStem, uint32_t _outStemSize) {
    if (_outStem == NULL || _outStemSize == 0) {
        return;
    }
    _outStem[0] = '\0';

    if (_fileName == NULL || _fileName[0] == '\0') {
        return;
    }

    snprintf(_outStem, _outStemSize, "%s", _fileName);
    char* dot = strrchr(_outStem, '.');
    if (dot != NULL) {
        *dot = '\0';
    }
}

static void AF_Project_SanitiseShaderName(const char* _nameInput, char* _outName, uint32_t _outNameSize) {
    if (_outName == NULL || _outNameSize == 0) {
        return;
    }
    _outName[0] = '\0';

    if (_nameInput == NULL || _nameInput[0] == '\0') {
        return;
    }

    char shaderFileName[AF_MAX_PATH_CHAR_SIZE] = {0};
    char shaderStem[AF_MAX_PATH_CHAR_SIZE] = {0};

    AF_Project_GetFileNameOnly(_nameInput, shaderFileName, sizeof(shaderFileName));
    AF_Project_GetFileStemNoExt(shaderFileName, shaderStem, sizeof(shaderStem));

    if (shaderStem[0] != '\0') {
        snprintf(_outName, _outNameSize, "%s", shaderStem);
    }
}

static void AF_Project_MigrateShaderToSelectedPlatform(AF_AppData* _appData, AF_Shader* _shader) {
    if (_appData == NULL || _shader == NULL) {
        return;
    }

    char resolvedShaderName[AF_MAX_PATH_CHAR_SIZE] = {0};

    AF_Project_SanitiseShaderName(_shader->name, resolvedShaderName, sizeof(resolvedShaderName));
    if (resolvedShaderName[0] == '\0') {
        AF_Project_SanitiseShaderName(_shader->vertPath, resolvedShaderName, sizeof(resolvedShaderName));
    }
    if (resolvedShaderName[0] == '\0') {
        AF_Project_SanitiseShaderName(_shader->fragPath, resolvedShaderName, sizeof(resolvedShaderName));
    }

    if (resolvedShaderName[0] != '\0') {
        snprintf(_shader->name, AF_MAX_PATH_CHAR_SIZE, "%s", resolvedShaderName);
    }

    if (_shader->name[0] != '\0' && _appData->projectData.assetsPath[0] != '\0') {
        char selectedVertPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        char selectedFragPath[AF_MAX_PATH_CHAR_SIZE] = {0};

        snprintf(
            selectedVertPath,
            sizeof(selectedVertPath),
            "%s/shaders/%s/%s.vert",
            _appData->projectData.assetsPath,
            AF_Platform_Mappings[_appData->projectData.platformData.platformType].name,
            _shader->name
        );
        snprintf(
            selectedFragPath,
            sizeof(selectedFragPath),
            "%s/shaders/%s/%s.frag",
            _appData->projectData.assetsPath,
            AF_Platform_Mappings[_appData->projectData.platformData.platformType].name,
            _shader->name
        );

        if (AF_File_FileExists(selectedVertPath) == AF_TRUE && AF_File_FileExists(selectedFragPath) == AF_TRUE) {
            snprintf(_shader->vertPath, AF_MAX_PATH_CHAR_SIZE, "%s", selectedVertPath);
            snprintf(_shader->fragPath, AF_MAX_PATH_CHAR_SIZE, "%s", selectedFragPath);
            return;
        }
    }

    // Fallback path keeps legacy scenes loading if selected-platform shaders are missing.
    // For bare filenames (e.g. "sprite.vert"), try to resolve from any platform.
    AF_Project_ResolveShaderPathPlatform(_appData, _shader->vertPath, sizeof(_shader->vertPath));
    AF_Project_ResolveShaderPathPlatform(_appData, _shader->fragPath, sizeof(_shader->fragPath));
    
    // If paths are still bare filenames after resolution, try to ensure full paths are built.
    // This handles the case where JSON contains just "sprite.vert" and we need to expand it.
    if (strstr(_shader->vertPath, "/") == NULL && _appData->projectData.assetsPath[0] != '\0' && _shader->name[0] != '\0') {
        char attemptVertPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        char attemptFragPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        
        // Try ALL platforms to find the shader
        for (uint32_t i = 0; i < AF_PLATFORM_COUNT; ++i) {
            snprintf(attemptVertPath, sizeof(attemptVertPath), "%s/shaders/%s/%s.vert",
                _appData->projectData.assetsPath, AF_Platform_Mappings[i].name, _shader->name);
            snprintf(attemptFragPath, sizeof(attemptFragPath), "%s/shaders/%s/%s.frag",
                _appData->projectData.assetsPath, AF_Platform_Mappings[i].name, _shader->name);
            
            if (AF_File_FileExists(attemptVertPath) == AF_TRUE && AF_File_FileExists(attemptFragPath) == AF_TRUE) {
                snprintf(_shader->vertPath, AF_MAX_PATH_CHAR_SIZE, "%s", attemptVertPath);
                snprintf(_shader->fragPath, AF_MAX_PATH_CHAR_SIZE, "%s", attemptFragPath);
                return;
            }
        }
    }
}

static void AF_Project_NormaliseProjectPath(char* _path, uint32_t _pathSize, const char* _projectRoot, const char* _appDataPath) {
    if (_path == NULL || _path[0] == '\0' || AF_Project_IsAbsolutePath(_path) == AF_TRUE) {
        return;
    }

    char originalPath[MAX_PROJECTDATA_FILE_PATH];
    snprintf(originalPath, sizeof(originalPath), "%s", _path);

    char resolvedPath[MAX_PROJECTDATA_FILE_PATH];
    if (_projectRoot != NULL && _projectRoot[0] != '\0') {
        snprintf(resolvedPath, sizeof(resolvedPath), "%s/%s", _projectRoot, originalPath);
        if (AF_File_FileExists(resolvedPath) == AF_TRUE) {
            snprintf(_path, _pathSize, "%s", resolvedPath);
            return;
        }
    }

    char appDataDirectory[MAX_PROJECTDATA_FILE_PATH];
    AF_Project_GetDirectoryPath(_appDataPath, appDataDirectory, sizeof(appDataDirectory));
    if (appDataDirectory[0] != '\0') {
        snprintf(resolvedPath, sizeof(resolvedPath), "%s/../%s", appDataDirectory, originalPath);
        if (AF_File_FileExists(resolvedPath) == AF_TRUE) {
            snprintf(_path, _pathSize, "%s", resolvedPath);
            return;
        }
    }

    if (AF_File_FileExists(originalPath) == AF_TRUE) {
        return;
    }
}

static void AF_Project_NormaliseProjectDirectory(char* _path, uint32_t _pathSize, const char* _projectRoot, const char* _appDataPath) {
    if (_path == NULL || _path[0] == '\0' || AF_Project_IsAbsolutePath(_path) == AF_TRUE) {
        return;
    }

    char originalPath[MAX_PROJECTDATA_FILE_PATH];
    snprintf(originalPath, sizeof(originalPath), "%s", _path);

    if (_projectRoot != NULL && _projectRoot[0] != '\0') {
        char projectRootCandidate[MAX_PROJECTDATA_FILE_PATH];
        snprintf(projectRootCandidate, sizeof(projectRootCandidate), "%s/%s", _projectRoot, originalPath);
        if (AF_Project_PathExists(projectRootCandidate) == AF_TRUE) {
            snprintf(_path, _pathSize, "%s", projectRootCandidate);
            return;
        }
    }

    char appDataDirectory[MAX_PROJECTDATA_FILE_PATH];
    AF_Project_GetDirectoryPath(_appDataPath, appDataDirectory, sizeof(appDataDirectory));
    if (appDataDirectory[0] != '\0') {
        char appDataCandidate[MAX_PROJECTDATA_FILE_PATH];
        snprintf(appDataCandidate, sizeof(appDataCandidate), "%s/../%s", appDataDirectory, originalPath);
        if (AF_Project_PathExists(appDataCandidate) == AF_TRUE) {
            snprintf(_path, _pathSize, "%s", appDataCandidate);
            return;
        }
    }

    if (AF_Project_PathExists(originalPath) == AF_TRUE) {
        // Preserve the existing relative path only if no project-root or app-data-based resolution applied.
        return;
    }
}

static void AF_Project_NormaliseScenePath(AF_AppData* _appData, char* _path, uint32_t _pathSize) {
    if (_appData == NULL || _path == NULL || _path[0] == '\0') {
        return;
    }

    AF_Project_NormaliseProjectPath(
        _path,
        _pathSize,
        _appData->projectData.projectRoot,
        _appData->projectData.defaultAppDataPath
    );
}

static void AF_Project_ResolveShaderPathPlatform(AF_AppData* _appData, char* _shaderPath, uint32_t _shaderPathSize) {
    if (_appData == NULL || _shaderPath == NULL || _shaderPath[0] == '\0') {
        return;
    }

    char shaderFileName[AF_MAX_PATH_CHAR_SIZE] = {0};
    AF_Project_GetFileNameOnly(_shaderPath, shaderFileName, sizeof(shaderFileName));
    if (shaderFileName[0] != '\0' && _appData->projectData.assetsPath[0] != '\0') {
        char selectedPlatformPath[AF_MAX_PATH_CHAR_SIZE] = {0};
        snprintf(
            selectedPlatformPath,
            sizeof(selectedPlatformPath),
            "%s/shaders/%s/%s",
            _appData->projectData.assetsPath,
            AF_Platform_Mappings[_appData->projectData.platformData.platformType].name,
            shaderFileName
        );

        if (AF_File_FileExists(selectedPlatformPath) == AF_TRUE) {
            snprintf(_shaderPath, _shaderPathSize, "%s", selectedPlatformPath);
            return;
        }
    }

    AF_Project_NormaliseScenePath(_appData, _shaderPath, _shaderPathSize);
    if (AF_File_FileExists(_shaderPath) == AF_TRUE) {
        return;
    }

    const char* shaderMarker = strstr(_shaderPath, "/shaders/");
    if (shaderMarker == NULL) {
        return;
    }

    const char* platformStart = shaderMarker + strlen("/shaders/");
    const char* platformEnd = strchr(platformStart, '/');
    if (platformEnd == NULL) {
        return;
    }

    int prefixLength = (int)(platformStart - _shaderPath);
    const char* fileSuffix = platformEnd + 1;

    char candidatePath[AF_MAX_PATH_CHAR_SIZE];
    for (uint32_t i = 0; i < AF_PLATFORM_COUNT; ++i) {
        snprintf(
            candidatePath,
            sizeof(candidatePath),
            "%.*s%s/%s",
            prefixLength,
            _shaderPath,
            AF_Platform_Mappings[i].name,
            fileSuffix
        );

        if (AF_File_FileExists(candidatePath) == AF_TRUE) {
            snprintf(_shaderPath, _shaderPathSize, "%s", candidatePath);
            return;
        }
    }
}


// AF_Project_ExpandShaderPaths
// Expands bare shader names in loaded ECS to full platform-specific paths
// Does NOT initialize mesh buffers (requires OpenGL context)
// ================
static void AF_Project_ExpandShaderPaths(AF_AppData* _appData) {
    if (_appData == NULL) {
        return;
    }

    // Expand shader paths in mesh components
    for (uint32_t i = 0; i < _appData->ecs.meshSparseSet.count; ++i) {
        AF_CMesh* meshComponent = &_appData->ecs.meshSparseSet.denseComponent[i];
        if (meshComponent != NULL && AF_Component_GetHas(meshComponent->enabled) == AF_TRUE) {
            AF_Project_MigrateShaderToSelectedPlatform(_appData, &meshComponent->shader);
        }
    }

    // Expand shader paths in sprite components
    for (uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i) {
        if (AF_Component_GetHas(_appData->ecs.sprites[i].enabled) == AF_TRUE) {
            AF_Project_MigrateShaderToSelectedPlatform(_appData, &_appData->ecs.sprites[i].spriteMesh.shader);
        }
    }

    // Expand shader paths in text components
    for (uint32_t i = 0; i < _appData->ecs.entitiesCount; ++i) {
        if (AF_Component_GetHas(_appData->ecs.texts[i].enabled) == AF_TRUE) {
            AF_Project_MigrateShaderToSelectedPlatform(_appData, &_appData->ecs.texts[i].mesh.shader);
        }
    }
}

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

        AF_Project_NormaliseScenePath(_appData, meshComponent->meshPath, sizeof(meshComponent->meshPath));
        AF_Project_MigrateShaderToSelectedPlatform(_appData, &meshComponent->shader);
        AF_Project_NormaliseScenePath(_appData, meshComponent->material.diffuseTexture.path, sizeof(meshComponent->material.diffuseTexture.path));
        
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
            AF_Project_NormaliseScenePath(_appData, terrainComponent->heightMapPath, sizeof(terrainComponent->heightMapPath));
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

            AF_Project_NormaliseScenePath(_appData, spriteComponent->spriteMesh.meshPath, sizeof(spriteComponent->spriteMesh.meshPath));
            AF_Project_MigrateShaderToSelectedPlatform(_appData, &spriteComponent->spriteMesh.shader);
            AF_Project_NormaliseScenePath(_appData, spriteComponent->spriteMesh.material.diffuseTexture.path, sizeof(spriteComponent->spriteMesh.material.diffuseTexture.path));

            // Legacy scene compatibility: older editor versions could save sprite components
            // with mesh shaders like "unlit" or "litShadowTexture" which are 3D world-space.
            // Sprite rendering expects the 2D screen-space sprite shader.
            if ((strcmp(spriteComponent->spriteMesh.shader.name, "unlit") == 0) ||
                (strcmp(spriteComponent->spriteMesh.shader.name, "litShadowTexture") == 0)) {
                snprintf(spriteComponent->spriteMesh.shader.name, AF_MAX_PATH_CHAR_SIZE, "%s", "sprite");
                snprintf(
                    spriteComponent->spriteMesh.shader.vertPath,
                    AF_MAX_PATH_CHAR_SIZE,
                    "%s/shaders/%s/sprite.vert",
                    _appData->projectData.assetsPath,
                    AF_Platform_Mappings[_appData->projectData.platformData.platformType].name
                );
                snprintf(
                    spriteComponent->spriteMesh.shader.fragPath,
                    AF_MAX_PATH_CHAR_SIZE,
                    "%s/shaders/%s/sprite.frag",
                    _appData->projectData.assetsPath,
                    AF_Platform_Mappings[_appData->projectData.platformData.platformType].name
                );
            }
            
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
            AF_Project_NormaliseScenePath(_appData, textComponent->mesh.meshPath, sizeof(textComponent->mesh.meshPath));
            AF_Project_MigrateShaderToSelectedPlatform(_appData, &textComponent->mesh.shader);
            AF_Project_NormaliseScenePath(_appData, textComponent->font.fontPath, sizeof(textComponent->font.fontPath));
            AF_Project_NormaliseScenePath(_appData, textComponent->fontPath, sizeof(textComponent->fontPath));
            uint32_t fontSize = 1;
            if(textComponent->font.fontSize > 0) {
                fontSize = textComponent->font.fontSize;
            } 

            // Resolve text font path from scene data. Legacy scenes often store relative
            // paths (e.g. "assets/font/...") that should be relative to projectRoot.
            char resolvedFontPath[AF_MAX_PATH_CHAR_SIZE] = {0};
            const char* preferredFontPath = textComponent->font.fontPath;
            if (AF_STRING_IS_EMPTY(preferredFontPath) && !AF_STRING_IS_EMPTY(textComponent->fontPath)) {
                preferredFontPath = textComponent->fontPath;
            }

            if (!AF_STRING_IS_EMPTY(preferredFontPath)) {
                FILE* fontFile = AF_File_OpenFile(preferredFontPath, "rb");
                if (fontFile != NULL) {
                    AF_File_CloseFile(fontFile);
                    snprintf(resolvedFontPath, AF_MAX_PATH_CHAR_SIZE, "%s", preferredFontPath);
                } else if (!AF_STRING_IS_EMPTY(_appData->projectData.projectRoot)) {
                    snprintf(
                        resolvedFontPath,
                        AF_MAX_PATH_CHAR_SIZE,
                        "%s/%s",
                        _appData->projectData.projectRoot,
                        preferredFontPath
                    );
                    fontFile = AF_File_OpenFile(resolvedFontPath, "rb");
                    if (fontFile != NULL) {
                        AF_File_CloseFile(fontFile);
                    } else {
                        resolvedFontPath[0] = '\0';
                    }
                }
            }

            // Last-resort fallback so text components still render if project font moved.
            if (AF_STRING_IS_EMPTY(resolvedFontPath)) {
                const char* fallbackFontPath = "assets/font/Montserrat/static/Montserrat-Medium.ttf";
                FILE* fallbackFile = AF_File_OpenFile(fallbackFontPath, "rb");
                if (fallbackFile != NULL) {
                    AF_File_CloseFile(fallbackFile);
                    snprintf(resolvedFontPath, AF_MAX_PATH_CHAR_SIZE, "%s", fallbackFontPath);
                    AF_Log_Warning("AF_Project_Load: Falling back to default font for text component %u\n", i);
                }
            }

            if (!AF_STRING_IS_EMPTY(resolvedFontPath)) {
                snprintf(textComponent->font.fontPath, AF_MAX_PATH_CHAR_SIZE, "%s", resolvedFontPath);
                snprintf(textComponent->fontPath, AF_MAX_PATH_CHAR_SIZE, "%s", resolvedFontPath);
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

    char cwd[MAX_PROJECTDATA_FILE_PATH] = {0};
    if (AF_PROJECT_GETCWD(cwd, sizeof(cwd)) == NULL) {
        cwd[0] = '\0';
    }

    char appDataPathAbsolute[MAX_PROJECTDATA_FILE_PATH] = {0};
    snprintf(appDataPathAbsolute, sizeof(appDataPathAbsolute), "%s", _appDataPath);
    AF_Project_MakeAbsolutePath(appDataPathAbsolute, sizeof(appDataPathAbsolute), cwd);
    
    // Load scene stored as default scene in the project data
    FILE* appDataFile = AF_File_OpenFile(appDataPathAbsolute, "rb");// switch to binary read mode as cause// "r");
    if (appDataFile == NULL) {
        AF_Log_Error("Editor_Utils_OpenProject: Failed to open project data file %s\n", appDataPathAbsolute);
        return AF_FALSE;
    }

    af_bool_t result = AF_JSON_LoadProjectDataJson(&_appData->projectData, appDataFile);
    if(result == AF_FALSE){
        AF_Log_Error("AF_Project_Load: Failed to load project data from %s\n", appDataPathAbsolute);
        return AF_FALSE;
    }

    if(appDataFile != NULL){
        AF_File_CloseFile(appDataFile);
        appDataFile = NULL;
    }else{
        AF_Log_Warning("AF_Project_Load: Failed to close project data file %s\n", appDataPathAbsolute);
        return AF_FALSE;
    }

    AF_Project_MakeAbsolutePath(_appData->projectData.projectRoot, sizeof(_appData->projectData.projectRoot), cwd);
    AF_Project_NormaliseProjectDirectory(_appData->projectData.assetsPath, sizeof(_appData->projectData.assetsPath), _appData->projectData.projectRoot, appDataPathAbsolute);
    AF_Project_NormaliseProjectPath(_appData->projectData.defaultAppDataPath, sizeof(_appData->projectData.defaultAppDataPath), _appData->projectData.projectRoot, appDataPathAbsolute);
    AF_Project_NormaliseProjectPath(_appData->projectData.defaultScenePath, sizeof(_appData->projectData.defaultScenePath), _appData->projectData.projectRoot, appDataPathAbsolute);

    AF_Project_MakeAbsolutePath(_appData->projectData.assetsPath, sizeof(_appData->projectData.assetsPath), cwd);
    AF_Project_MakeAbsolutePath(_appData->projectData.defaultAppDataPath, sizeof(_appData->projectData.defaultAppDataPath), cwd);
    AF_Project_MakeAbsolutePath(_appData->projectData.defaultScenePath, sizeof(_appData->projectData.defaultScenePath), cwd);
    

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

    // Expand shader paths from bare names (e.g. "sprite.vert") to full platform-specific paths
    // This is needed because scenes are now persisted with portable shader identity (name only),
    // and must be expanded to the selected platform's shader directory at load time.
    // We do NOT initialize mesh buffers here (requires OpenGL context not yet created).
    AF_Project_ExpandShaderPaths(_appData);

    // Clean up the render objects first as some data is malloc
   AF_Log("AF_Project_Load: Finished Loading\n");
    // Return true only if the platform string was successfully found and extracted
    return AF_TRUE;
}

af_bool_t AF_LoadScene(AF_AppData *_appData, const char *_sceneFilePath)
{
    af_bool_t returnValue = AF_FALSE;
    // Unload everything first
    AF_Script_Call_Destroy(_appData);
    AF_Script_UnloadScripts(&_appData->ecs);

    AF_ECS_Init(&_appData->ecs);

    // Tear down old Bullet world first. Re-init happens after scene JSON is loaded.
    AF_Physics_Shutdown(&_appData->physicsEngineHandle);

    // Load the ECS from the file
    FILE* sceneFile = AF_File_OpenFile(_sceneFilePath, "rb");
    if (sceneFile == NULL) {
        AF_Log_Error("AF_LoadScene: Failed to open scene file %s\n", _sceneFilePath);
        return AF_FALSE;
    }

    af_bool_t sceneLoaded = AF_JSON_LoadSceneJson(_appData, sceneFile);
    AF_File_CloseFile(sceneFile);

    AF_Log("AF_LoadScene: Finished loading scene from %s\n", _sceneFilePath);

    if (sceneLoaded == AF_TRUE) {
        AF_Log("AF_LoadScene: Successfully loaded scene from %s\n", _sceneFilePath);
        // Track the active scene so menu-bar saves go to the right file
        snprintf(_appData->projectData.defaultScenePath, MAX_PROJECTDATA_FILE_PATH, "%s", _sceneFilePath);

        AF_Project_SyncEntities(_appData);

        // Build Bullet bodies from the freshly loaded/synced ECS scene.
        AF_Physics_Init(&_appData->ecs, &_appData->physicsEngineHandle);
        AF_Renderer_InitCollisionGeomtery(&_appData->ecs);

        AF_Script_Load_And_Bind_Functions(&_appData->ecs);
        AF_Script_Call_Start(_appData);

        returnValue = AF_TRUE;
    } else {
        // Keep physics handle valid even when scene load fails.
        AF_Physics_Init(&_appData->ecs, &_appData->physicsEngineHandle);
        AF_Log_Error("Editor_SceneBrowser_RenderSaveFile: Failed to load scene %s\n", _sceneFilePath);
    }
    return returnValue;
}

void AF_RequestSceneChange(AF_AppData* _appData, const char* _sceneFilePath) {
    if (_appData == NULL || _sceneFilePath == NULL) {
        return;
    }
    snprintf(_appData->projectData.pendingScenePath, MAX_PROJECTDATA_FILE_PATH, "%s", _sceneFilePath);
    _appData->projectData.hasPendingSceneChange = AF_TRUE;
    AF_Log("AF_RequestSceneChange: Scene change to '%s' queued\n", _sceneFilePath);
}
