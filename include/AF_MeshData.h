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

// TODO: don't need to keep a copy of the vertices and indices in the mesh data
// The data lives in the gpu accessed by the vaos and vbos
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
        .material = AF_Material_ZERO()  // {shaderid, diffuse, normal, specular, color, shininess}
    };
    return returnMesh;
}


#ifdef __cplusplus
}
#endif

#endif //AF_MESH_DATA_H
