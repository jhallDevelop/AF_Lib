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
AF_CMesh AF_CMesh_ADD(void){
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

