/*
===============================================================================
AF_MeshData_H

===============================================================================
*/
#ifndef AF_MESH_DATA_H
#define AF_MESH_DATA_H
#include "AF_Material.h"
#include "AF_Vertex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // array of meshes
    AF_Vertex* vertices;
    uint16_t vertexCount;
    uint32_t* indices;
    uint32_t indexCount;
    uint32_t vao;
    uint32_t vbo;
    uint32_t ibo;
    AF_Material material;
} AF_MeshData;

static inline AF_MeshData AF_MeshData_ZERO(void){
    AF_MeshData returnMesh = {
        .vertices = NULL,
        .vertexCount = 0,
        .indices = NULL,
        .indexCount = 0,
        .vao = 0,
        .vbo = 0,
        .ibo = 0,
        .material = {0,0,{0,0,0,0}}
    };
    return returnMesh;
}


#ifdef __cplusplus
}
#endif

#endif //AF_MESH_DATA_H
