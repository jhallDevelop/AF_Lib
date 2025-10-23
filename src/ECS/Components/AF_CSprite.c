#include "ECS/Components/AF_CSprite.h"
#include "ECS/Components/AF_Component.h"





/*
====================
AF_CSprite_ZERO
Empty constructor for the AF_CSprite component
====================
*/
AF_CSprite AF_CSprite_ZERO(void){
	AF_CSprite returnSprite = {
		//.has = false,
		.enabled = AF_FALSE,
		.loop = AF_TRUE,
		.currentFrame = 0,
		.animationFrames = 0,
		.currentFrameTime = 0,
		.nextFrameTime = 0,
		.animationSpeed = 0,
		.spritePos = {0, 0},
		.spriteSize = {0, 0},
		.spriteScale = {0,0},
		.spriteFramePos = {0,0},
		.spriteFrameSize = {0,0},
		.spriteRotation = 0.0f,
		.flipX = AF_FALSE,
		.flipY = AF_FALSE,
		.spriteSheetSize = {0, 0},
		.spriteSheetPos = {0, 0},
		.spriteColor = {255, 255, 255, 255},
		.spriteData = NULL,
		.isPlaying = AF_FALSE,
		.filtering = AF_FALSE,
		.spriteMesh = AF_CMesh_ZERO()
		};
	return returnSprite;
}

/*
====================
AF_CSprite_ADD
ADD component and set default values
====================
*/
AF_CSprite AF_CSprite_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CSprite returnSprite = {
		//.has = true,
		.enabled = component,
		.loop = AF_TRUE,
		.currentFrame = 0,
		.animationFrames = 0,
		.currentFrameTime = 0,
		.nextFrameTime = 0,
		.animationSpeed = 0,
		.spritePos = {0, 0},
		.spriteSize = {64, 64},
		.spriteScale = {1,1},
		.spriteFramePos = {0,0},
		.spriteFrameSize = {64,64},
		.spriteRotation = 0.0f,
		.flipX = AF_FALSE,
		.flipY = AF_FALSE,
		.spriteSheetSize = {512, 512},
		.spriteSheetPos = {0, 0},
		.spriteColor = {255, 255, 255, 255},
		.spriteData = NULL,
		.isPlaying = AF_FALSE, 
		.filtering = AF_FALSE,
		.spriteMesh = AF_CMesh_ZERO()
	};

	snprintf(returnSprite.spriteMesh.meshPath, AF_MAX_PATH_CHAR_SIZE, "%s", AF_CSPRITE_DEFAULT_SPRITE_MESH_NAME);

    // Initialize the mesh to be a quad for rendering text
    returnSprite.spriteMesh.meshCount = 1;
    returnSprite.spriteMesh.meshes[0].vertexCount = 6;
    returnSprite.spriteMesh.meshes[0].indexCount = 6;
    // assign the static quad vertices
    //returnSprite.spriteMesh.meshes[0].vertices = AF_CMESH_QUAD_VERTS;
    // set the default shader paths
    snprintf(returnSprite.spriteMesh.shader.vertPath, sizeof(returnSprite.spriteMesh.shader.vertPath), "%s", AF_CSPRITE_DEFAULT_SPRITE_VERT_PATH);
    snprintf(returnSprite.spriteMesh.shader.fragPath, sizeof(returnSprite.spriteMesh.shader.fragPath), "%s", AF_CSPRITE_DEFAULT_SPRITE_FRAG_PATH);
	return returnSprite;
}
