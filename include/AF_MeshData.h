/*
===============================================================================
AF_MeshData_H

===============================================================================
*/
#ifndef AF_MESH_DATA_H
#define AF_MESH_DATA_H
#include "AF_Material.h"
#include "AF_Vertex.h"
#include <stddef.h> // For NULL
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
} AF_MeshData;

static inline AF_MeshData AF_MeshData_ZERO(void){
    AF_MeshData returnMesh;
    returnMesh.vertices = NULL;
    returnMesh.vertexCount = 0;
    returnMesh.indices = NULL;
    returnMesh.indexCount = 0;
    returnMesh.vao = 0;
    returnMesh.vbo = 0;
    returnMesh.ibo = 0;
    return returnMesh;
}


#ifdef __cplusplus
}
#endif

#endif //AF_MESH_DATA_H
