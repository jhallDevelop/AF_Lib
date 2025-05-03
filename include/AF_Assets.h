/*
===============================================================================
AF_ASSETS_H

Implementation of the assets struct and initialization function
===============================================================================
*/
#ifndef AF_ASSETS_H
#define AF_ASSETS_H
#include <string.h>
#include "AF_Texture.h"
#include "AF_MeshData.h"
#include "AF_Log.h"
#include "AF_Shader.h"


#ifdef __cplusplus
extern "C" {
#endif



#define AF_ASSETS_MAX_TEXTURES 16
#define AF_ASSETS_MAX_MESHES 16
#define AF_ASSETS_MAX_SOUNDS 16
#define AF_ASSETS_MAX_SHADERS 16

typedef struct AF_Assets{
    // array of meshes
    uint32_t nextAvailableTexture;
    uint32_t nextAvailableMesh;
    uint32_t nextAvailableShader;
    AF_Texture textures[AF_ASSETS_MAX_TEXTURES];
    AF_MeshData meshes[AF_ASSETS_MAX_MESHES];
    AF_Shader shaders[AF_ASSETS_MAX_SHADERS];
    // TODO: add sounds
} AF_Assets;

/*
====================
AF_Assets_ZERO
Implementation for initializing the assets struct
====================
*/
static inline AF_Assets AF_Assets_ZERO(void){
    AF_Assets returnAssets;
    for(uint32_t i = 0; i < AF_ASSETS_MAX_TEXTURES; i++){
        returnAssets.textures[i] = AF_Texture_ZERO();
    }

    for (uint32_t i = 0; i < AF_ASSETS_MAX_MESHES; i++){
        returnAssets.meshes[i] = AF_MeshData_ZERO();
    }

    for(uint32_t i = 0; i < AF_ASSETS_MAX_SHADERS; i++){
        returnAssets.shaders[i] = AF_Shader_ZERO();
    }

    returnAssets.nextAvailableMesh = 0;
    returnAssets.nextAvailableTexture = 0;
    returnAssets.nextAvailableShader = 0;

    return returnAssets;
}

/*
====================
AF_Assets_AddShader
Implementation for adding a shader to the assets
returns a pointer to the shader added in the assets/shader array
====================
*/
static inline AF_Shader* AF_Assets_AddShader(AF_Assets* _assets){
    if(_assets == NULL){
        AF_Log_Warning("AF_Assets_AddShader: passed null assets\n");
        return NULL;
    }
    AF_Log("next available shader: %i MAX: %i \n",_assets->nextAvailableShader, AF_ASSETS_MAX_SHADERS);
    if(_assets->nextAvailableShader < AF_ASSETS_MAX_SHADERS){
        _assets->nextAvailableShader++;
        return &_assets->shaders[_assets->nextAvailableShader - 1];
    }else{
        AF_Log_Warning("AF_Assets_AddShader: Max shaders reached\n");
        return NULL;
    }
}

/*
====================
AF_Assets_AddTexture
Implementation for adding a texture to the assets
returns a pointer to the texture added in the assets/texture array
====================
*/
static inline void AF_Assets_AddTexture(AF_Assets* _assets, AF_Texture _texture){
    if(_assets->nextAvailableTexture < AF_ASSETS_MAX_TEXTURES){
        _assets->nextAvailableTexture++;
        _assets->textures[_assets->nextAvailableTexture] = _texture;
    }else{
        AF_Log_Warning("AF_Assets_AddTexture: Max textures reached\n");
    }
}

/*
====================
AF_Assets_GetTexture
Implementation for getting a texture to the assets
returns a pointer to the texture added in the assets/texture array
====================
*/
static inline AF_Texture AF_Assets_GetTexture(AF_Assets* _assets, const char* _texturePath){
    if(_assets == NULL){
        AF_Log_Error("AF_Assets_AddTexture: passed null assets\n");
    }
    
    AF_Texture returnTexture = {0, AF_TEXTURE_TYPE_NONE, "\0"};
    for(unsigned int j = 0; j < AF_ASSETS_MAX_TEXTURES; j++)
    {
        if(strncmp(_assets->textures[j].path, _texturePath, MAX_PATH_CHAR_SIZE) == 0)
        {
            AF_Log("AF_Assets_GetTexture: strncmp %s | %s\n", _texturePath, _assets->textures[j].path);
            // if a texture with the same filepath is already loaded, use this texture data
            //AF_Log("AF_Assets_GetTexture: Found texture %s in assets: path: %s ID: %i\n", _texturePath, _assets->textures[j].path, returnTexturePtr->id);
            returnTexture = _assets->textures[j];
            break;
        }
    }

    return returnTexture;
}

/*
====================
AF_Assets_AddMesh
Implementation for adding a mesh to the assets
returns a pointer to the mesh data added in the assets/meshdata array
====================
*/
static inline AF_MeshData* AF_Assets_AddMesh(AF_Assets* _assets, AF_MeshData _mesh){
    if(_assets == NULL){
        AF_Log_Warning("AF_Assets_AddMesh: passed null assets\n");
        return NULL;
    }
    if(_assets->nextAvailableMesh < AF_ASSETS_MAX_MESHES){
        _assets->meshes[_assets->nextAvailableMesh] = _mesh;
        _assets->nextAvailableMesh++;
        return &_assets->meshes[_assets->nextAvailableTexture - 1];
    }else{
        AF_Log_Warning("AF_Assets_AddMesh: Max meshes reached\n");
        return NULL;
    }
}



#ifdef __cplusplus
}
#endif

#endif //AF_ASSETS_H
