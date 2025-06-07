#include "ECS/Components/AF_CMesh.h"
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CMesh_ZERO
Function used to create an empty mesh component
====================
*/
AF_CMesh AF_CMesh_ZERO(void){
    AF_CMesh returnMesh = {
	//.has = AF_FALSE,
	.enabled = AF_FALSE,
	//.meshes = NULL,
	.meshCount = 0,
	.showDebug = AF_FALSE,
	.meshType = AF_MESH_TYPE_PLANE,
	.shader = AF_Shader_ZERO(),
	.isImageFlipped = AF_FALSE,
	.meshID = 0,
	.isAnimating = AF_FALSE,
	.textured = AF_FALSE,
	.transparent = AF_FALSE,
	.recieveLights = AF_FALSE,
	.recieveShadows = AF_FALSE,
	.castShadows = AF_FALSE,
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
AF_CMesh AF_CMesh_ADD(void){
    PACKED_CHAR component = AF_TRUE;
    component = AF_Component_SetHas(component, AF_TRUE);
    component = AF_Component_SetEnabled(component, AF_TRUE);

    AF_CMesh returnMesh = {
	//.has = AF_TRUE,
	.enabled = component,
	//.meshes = NULL,
	.meshCount = 0,
	.showDebug = AF_FALSE,
	.meshType = AF_MESH_TYPE_PLANE,
	.meshPath = DEFAULT_MODEL_PATH,
	.shader = AF_Shader_ZERO(),
	.isImageFlipped = AF_FALSE,
	.meshID = 0,
	.isAnimating = AF_FALSE,
	.textured = AF_TRUE,
	.transparent = AF_TRUE,
	.recieveLights = AF_TRUE,
	.recieveShadows = AF_TRUE,
	.castShadows = AF_TRUE,
	.modelMatrix = NULL,
	.displayListBuffer = NULL
	};

	// init the meshes
	for(uint32_t i = 0; i < MAX_MESH_COUNT; i++){
		returnMesh.meshes[i] = AF_MeshData_ZERO();
	}
    return returnMesh;
}

