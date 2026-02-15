/*
===============================================================================
AF_CTerrain_H

Header only Terrain loading
functions to load meshes, creating memory on the heap based on the size of the mesh

===============================================================================
*/
#ifndef AF_CTERRAIN_H
#define AF_CTERRAIN_H
#include "AF_Lib_Define.h"
#include "AF_Material.h"
#include "ECS/Components/AF_Component.h"
#include "AF_MeshData.h"
#include "AF_Shader.h"


#ifdef __cplusplus
extern "C" {
#endif  


// Terrain Struct
typedef struct AF_CTerrain {
    PACKED_CHAR enabled;
    uint8_t lod0Size;
    uint8_t lod1Size;
    uint8_t lod2Size;
    uint32_t meshComponentIndex;
    char heightMapPath[AF_MAX_PATH_CHAR_SIZE];
    uint32_t heightmapTextureID;
    AF_FLOAT heightScale;
    AF_FLOAT texelSizeX;
    AF_FLOAT texelSizeY;
    uint32_t gridSize;
    AF_FLOAT gridScale;
    uint32_t numChunks;
    AF_FLOAT heightMapUVScaleX;
    AF_FLOAT heightMapUVSCaleY;
    uint32_t lodLevel;
    AF_FLOAT paddingPixels;        // 32.0
    uint32_t atlasTextureSize;       // 256
    uint32_t texturesPerRow;         // 2.0
    uint32_t heightTextureIndices[6][2];
    uint32_t grassTextureIndices[3][2];

    uint32_t physicsResolution; // target resolution for physics heightfield (0 = use raw heightmap)

    // CPU heightmap data
    unsigned char* heightMapData; // pointer to heightmap data array on CPU
    uint32_t heightMapWidth;  // width of heightmap
    uint32_t heightMapHeight; // height of heightmap
} AF_CTerrain;

AF_CTerrain AF_CTerrain_ZERO(void);
AF_CTerrain AF_CTerrain_ADD(void);

#ifdef __cplusplus
}
#endif  

#endif  // AF_CMESH_H
