/*
===============================================================================
AF_CMesh_H

Header only Mesh loading
functions to load meshes, creating memory on the heap based on the size of the mesh

===============================================================================
*/
#ifndef AF_CMESH_H
#define AF_CMESH_H
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


#define AF_MAX_MESH_COUNT 32

// Static quad vertices for text rendering (positions + texCoords)
static AF_Vertex AF_CMESH_QUAD_VERTS[6] = {
    // Triangle 1: top-left, bottom-left, bottom-right
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // top-left
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-left
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-right
    
    // Triangle 2: top-left, bottom-right, top-right
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // top-left
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-right
    {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}  // top-right
};

enum AF_MESH_TYPE{
	AF_MESH_TYPE_PLANE, 
	AF_MESH_TYPE_CUBE,
	AF_MESH_TYPE_SPHERE,
	AF_MESH_TYPE_MESH
};


// Mesh Struct
typedef struct AF_CMesh {
    PACKED_CHAR enabled;
    AF_MeshData meshes[AF_MAX_MESH_COUNT];
	uint32_t meshCount;
    af_bool_t showDebug;
	enum AF_MESH_TYPE meshType;
	// TODO: move strings out to a separate struct or use a string library
	char meshPath[AF_MAX_PATH_CHAR_SIZE];
	AF_Shader shader;
	AF_Material material;
	af_bool_t isImageFlipped;		// flip the textures on load
	// TODO: re-evaluate do we really need the following in the struct. Was put in for rapid n64 dev, but its a bit messy
	uint8_t meshID;		// only fit 255 mesh types
	af_bool_t isAnimating;	// belongs in animation component
	af_bool_t textured;
    af_bool_t transparent;
    af_bool_t recieveLights;
    af_bool_t recieveShadows;
    af_bool_t castShadows;
	void* modelMatrix;	// belongs in transform, unless nessisary
	void* displayListBuffer;
	af_bool_t isInstanced;	
	uint32_t instanceCount;
	uint32_t instanceVBO;
} AF_CMesh;

// ====================
// AF_CMesh_SparseSet
// Sparse set for the Mesh component
// ====================
typedef struct AF_CMesh_SparseSet {
    // List of entity IDs that have this component, indexed by the dense array index
    uint32_t sparseEntityIDs[AF_ECS_TOTAL_ENTITIES]; // Maps sparse index to entity ID

    // Dense array of components for iteration
    AF_CMesh denseComponent[AF_ECS_TOTAL_ENTITIES];  // Maps dense index to entity ID

    // reverse mapping from entity ID to dense index for O(1) access
    uint32_t denseToSparse[AF_ECS_TOTAL_ENTITIES]; // Maps dense index to sparse index
    uint32_t count; // Number of active components
} AF_CMesh_SparseSet;


AF_CMesh AF_CMesh_ZERO(void);
AF_CMesh AF_CMesh_ADD(void);


#ifdef __cplusplus
}
#endif  

#endif  // AF_CMESH_H
