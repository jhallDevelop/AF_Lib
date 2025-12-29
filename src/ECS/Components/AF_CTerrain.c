#include "ECS/Components/AF_CTerrain.h"
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CMesh_ZERO
Function used to create an empty terrain component
====================
*/
AF_CTerrain AF_CTerrain_ZERO(void){
    AF_CTerrain returnTerrain = {
	.enabled = AF_FALSE,
	.lod0Size = 0,
	.lod1Size = 0,
	.lod2Size = 0,
	.meshComponentIndex = 0,
	};
    return returnTerrain;
}

/*
====================
AF_CTerrain_ADD
Function used to Add the component
====================
*/
AF_CTerrain AF_CTerrain_ADD(void){
    PACKED_CHAR component = AF_Component_SetEnabled(AF_TRUE, AF_TRUE);

    AF_CTerrain returnTerrain = {
	.enabled = component,
	.lod0Size = 0,
	.lod1Size = 0,
	.lod2Size = 0,
	.meshComponentIndex = 0,
	};
    return returnTerrain;
}

