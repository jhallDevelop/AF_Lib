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
	.heightMapPath = "\0",
	.heightmapTextureID = 0,
	.heightScale = 100.0f,
	.texelSizeX = 1.0f / 1024.0f,
	.texelSizeY = 1.0f / 1024.0f,
	.gridSize = 17,
	.gridScale = 15.625f,
	.numChunks = 1024,
	.heightMapUVScaleX = 0.1f,
	.heightMapUVSCaleY = 0.1f,
	.lodLevel = 0,
	.paddingPixels = 16.0,
    .atlasTextureSize = 256,
    .texturesPerRow = 3,
    .heightTextureIndices = { {0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {1,2} },
    .grassTextureIndices = { {2,0}, {2,1}, {2,2} },
	.physicsResolution = 0,
	.heightMapData = NULL,
	.heightMapWidth = 0,
	.heightMapHeight = 0
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
	.heightMapPath = "\0",
	.heightmapTextureID = 0,
	.heightScale = 100.0f,
	.texelSizeX = 1.0f / 1024.0f,
	.texelSizeY = 1.0f / 1024.0f,
	.gridSize = 17,
	.gridScale = 15.625f,
	.numChunks = 1024,
	.heightMapUVScaleX = 0.1f,
	.heightMapUVSCaleY = 0.1f,
	.lodLevel = 0,
	.paddingPixels = 16.0,
    .atlasTextureSize = 256,
    .texturesPerRow = 3,
    .heightTextureIndices = { {0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {1,2} },
    .grassTextureIndices = { {2,0}, {2,1}, {2,2} },
	.physicsResolution = 0,
	.heightMapData = NULL,
	.heightMapWidth = 0,
	.heightMapHeight = 0
	};
    return returnTerrain;
}

