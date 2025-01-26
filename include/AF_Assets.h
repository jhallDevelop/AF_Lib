/*
===============================================================================
AF_ASSETS_H

Implementation of the assets struct and initialization function
===============================================================================
*/
#ifndef AF_ASSETS_H
#define AF_ASSETS_H

#include "AF_Texture.h"
#include "AF_MeshData.h"
#include "AF_Log.h"

#ifdef __cplusplus
extern "C" {
#endif



#define AF_ASSETS_MAX_TEXTURES 16
#define AF_ASSETS_MAX_MESHES 16
#define AF_ASSETS_MAX_SOUNDS 16

typedef struct AF_Assets{
    // array of meshes
    uint32_t nextAvailableTexture;
    uint32_t nextAvailableMesh;
    AF_Texture textures[AF_ASSETS_MAX_TEXTURES];
    AF_MeshData meshes[AF_ASSETS_MAX_MESHES];
    
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

    returnAssets.nextAvailableMesh = 0;
    returnAssets.nextAvailableTexture = 0;

    return returnAssets;
}


/*
====================
AF_Assets_AddTexture
Implementation for adding a texture to the assets
returns a pointer to the texture added in the assets/texture array
====================
*/
static inline AF_Texture* AF_Assets_AddTexture(AF_Assets* _assets){
    if(_assets->nextAvailableTexture < AF_ASSETS_MAX_TEXTURES){
        _assets->nextAvailableTexture++;
        return &_assets->textures[_assets->nextAvailableTexture - 1];
    }else{
        AF_Log_Warning("AF_Assets_AddTexture: Max textures reached\n");
        return NULL;
    }
}

/*
====================
AF_Assets_AddMesh
Implementation for adding a mesh to the assets
returns a pointer to the mesh data added in the assets/meshdata array
====================
*/
static inline AF_MeshData* AF_Assets_AddMesh(AF_Assets* _assets, AF_MeshData _mesh){
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