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
#include "AF_Font.h"
#include "AF_Log.h"
#include "AF_Shader.h"


#ifdef __cplusplus
extern "C" {
#endif


#define AF_ASSETS_MAX_ASSETS 32
#define AF_ASSETS_MAX_TEXTURES AF_ASSETS_MAX_ASSETS
#define AF_ASSETS_MAX_MESHES AF_ASSETS_MAX_ASSETS
#define AF_ASSETS_MAX_SOUNDS AF_ASSETS_MAX_ASSETS
#define AF_ASSETS_MAX_SHADERS AF_ASSETS_MAX_ASSETS
#define AF_ASSETS_MAX_FONTS AF_ASSETS_MAX_ASSETS

typedef struct AF_Assets{
    // array of meshes
    uint32_t nextAvailableTexture;
    uint32_t nextAvailableMesh;
    uint32_t nextAvailableShader;
    uint32_t nextAvailableFont;
    AF_Texture textures[AF_ASSETS_MAX_TEXTURES];
    AF_MeshData meshes[AF_ASSETS_MAX_MESHES];
    AF_Shader shaders[AF_ASSETS_MAX_SHADERS];
    AF_Font fonts[AF_ASSETS_MAX_FONTS];
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
    for(uint32_t i = 0; i < AF_ASSETS_MAX_FONTS; i++){
        returnAssets.fonts[i] = AF_Font_Zero();
    }

    returnAssets.nextAvailableMesh = 0;
    returnAssets.nextAvailableTexture = 0;
    returnAssets.nextAvailableShader = 0;
    returnAssets.nextAvailableFont = 0;

    return returnAssets;
}


AF_Shader* AF_Assets_AddShader(AF_Assets* _assets);
void AF_Assets_AddTexture(AF_Assets* _assets, AF_Texture _texture);
AF_Texture AF_Assets_GetTexture(AF_Assets* _assets, const char* _texturePath);
AF_MeshData* AF_Assets_AddMesh(AF_Assets* _assets, AF_MeshData _mesh);
AF_Font* AF_Assets_AddFont(AF_Assets* _assets, AF_Font _font);



#ifdef __cplusplus
}
#endif

#endif //AF_ASSETS_H
