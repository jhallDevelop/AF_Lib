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

//#define DEFAULT_ASSET_PATH "./assets/"
#define DEFAULT_MODEL_PATH "./assets/models/UV_Cube/UV_Cube.obj"

#ifdef __cplusplus
extern "C" {
#endif  


// Terrain Struct
typedef struct AF_CTerrain {
    PACKED_CHAR enabled;
    uint32_t lod0Size;
    uint32_t lod1Size;
    uint32_t lod2Size;
    uint32_t meshComponentIndex;
} AF_CTerrain;

AF_CTerrain AF_CTerrain_ZERO(void);
AF_CTerrain AF_CTerrain_ADD(void);

#ifdef __cplusplus
}
#endif  

#endif  // AF_CMESH_H
