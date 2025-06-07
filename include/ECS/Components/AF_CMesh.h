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

#define MAX_PATH_CHAR_SIZE 128
#define MAX_MESH_COUNT 32

enum AF_MESH_TYPE{
	AF_MESH_TYPE_PLANE, 
	AF_MESH_TYPE_CUBE,
	AF_MESH_TYPE_SPHERE,
	AF_MESH_TYPE_MESH
};


// Mesh Struct
typedef struct AF_CMesh {
    PACKED_CHAR enabled;
    AF_MeshData meshes[MAX_MESH_COUNT];
	uint32_t meshCount;
    af_bool_t showDebug;
	enum AF_MESH_TYPE meshType;
	char meshPath[MAX_PATH_CHAR_SIZE];
	AF_Shader shader;
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
} AF_CMesh;

AF_CMesh AF_CMesh_ZERO(void);
AF_CMesh AF_CMesh_ADD(void);

#ifdef __cplusplus
}
#endif  

#endif  // AF_CMESH_H
