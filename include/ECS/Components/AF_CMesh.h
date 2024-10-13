/*
===============================================================================
AF_CMesh_H

Header only Mesh loading
functions to load meshes, creating memory on the heap based on the size of the mesh

===============================================================================
*/
#ifndef AF_CMESH_H
#define AF_CMESH_H
#include "AF_Vertex.h"
#include "AF_Lib_Define.h"
#include "AF_Material.h"
#include "ECS/Components/AF_Component.h"

#ifdef __cplusplus
extern "C" {
#endif  


// Mesh Struct
typedef struct {
    //BOOL has;
    PACKED_CHAR enabled;
    AF_Vertex* vertices;
    uint16_t vertexCount;
    uint32_t* indices;
    int indexCount;
    // TODO: pack this
    uint32_t vao;
    uint32_t vbo;
    uint32_t ibo;
    AF_Material material;
    BOOL showDebug;
} AF_CMesh;

/*
====================
AF_CMesh_ZERO
Function used to create an empty mesh component
====================
*/
static inline AF_CMesh AF_CMesh_ZERO(void){
    AF_CMesh returnMesh = {
	//.has = FALSE,
	.enabled = FALSE,
	.vertices = NULL,
	.vertexCount = 0,
	.indices = NULL,
	.indexCount = 0,
	.vao = 0,
	.vbo = 0,
	.ibo = 0,
	.material = {0,0},
	.showDebug = FALSE
    };
    return returnMesh;
}

/*
====================
AF_CMesh_ADD
Function used to Add the component
====================
*/
static inline AF_CMesh AF_CMesh_ADD(void){
    PACKED_CHAR component = TRUE;
    component = AF_Component_SetHas(component, TRUE);
    component = AF_Component_SetEnabled(component, TRUE);

    AF_CMesh returnMesh = {
	//.has = TRUE,
	.enabled = component,
	.vertices = NULL,
	.vertexCount = 0,
	.indices = NULL,
	.indexCount = 0,
	.vao = 0,
	.vbo = 0,
	.ibo = 0,
	.material = {0,0},
	.showDebug = FALSE
    };
    return returnMesh;
}



#ifdef __cplusplus
}
#endif  

#endif  // AF_CMESH_H
