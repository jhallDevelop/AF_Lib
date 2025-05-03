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
    BOOL showDebug;
	enum AF_MESH_TYPE meshType;
	char meshPath[MAX_PATH_CHAR_SIZE];
	AF_Shader shader;
	BOOL isImageFlipped;		// flip the textures on load
	// TODO: re-evaluate do we really need the following in the struct. Was put in for rapid n64 dev, but its a bit messy
	uint8_t meshID;		// only fit 255 mesh types
	BOOL isAnimating;	// belongs in animation component
	BOOL textured;
    BOOL transparent;
    BOOL recieveLights;
    BOOL recieveShadows;
    BOOL castShadows;
	void* modelMatrix;	// belongs in transform, unless nessisary
	void* displayListBuffer;	
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
	//.meshes = NULL,
	.meshCount = 0,
	.showDebug = FALSE,
	.meshType = AF_MESH_TYPE_PLANE,
	.shader = AF_Shader_ZERO(),
	.isImageFlipped = FALSE,
	.meshID = 0,
	.isAnimating = FALSE,
	.textured = FALSE,
	.transparent = FALSE,
	.recieveLights = FALSE,
	.recieveShadows = FALSE,
	.castShadows = FALSE,
	.modelMatrix = NULL,
	.displayListBuffer = NULL
	};
	// ensure its init and safe with null terminator
	returnMesh.meshPath[0] = '\0';  // Ensure the path is empty

	// init the meshes
	for(uint32_t i = 0; i < MAX_MESH_COUNT; i++){
		returnMesh.meshes[i] = AF_MeshData_ZERO();
	}
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
	//.meshes = NULL,
	.meshCount = 0,
	.showDebug = FALSE,
	.meshType = AF_MESH_TYPE_PLANE,
	.meshPath = DEFAULT_MODEL_PATH,
	.shader = AF_Shader_ZERO(),
	.isImageFlipped = FALSE,
	.meshID = 0,
	.isAnimating = FALSE,
	.textured = TRUE,
	.transparent = TRUE,
	.recieveLights = TRUE,
	.recieveShadows = TRUE,
	.castShadows = TRUE,
	.modelMatrix = NULL,
	.displayListBuffer = NULL
	};

	// init the meshes
	for(uint32_t i = 0; i < MAX_MESH_COUNT; i++){
		returnMesh.meshes[i] = AF_MeshData_ZERO();
	}
    return returnMesh;
}



#ifdef __cplusplus
}
#endif  

#endif  // AF_CMESH_H
