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
		.spriteRotation = 0.0f,
		.flipX = AF_FALSE,
		.flipY = AF_FALSE,
		.spriteSheetSize = {0, 0},
		.spriteColor = {255, 255, 255, 255},
		.spritePath = NULL,
		.spriteData = NULL,
		.isPlaying = AF_FALSE,
		.filtering = AF_FALSE
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
		.spriteSize = {0, 0},
		.spriteScale = {0,0},
		.spriteRotation = 0.0f,
		.flipX = AF_FALSE,
		.flipY = AF_FALSE,
		.spriteSheetSize = {0, 0},
		.spriteColor = {255, 255, 255, 255},
		.spritePath = NULL,
		.spriteData = NULL,
		.isPlaying = AF_FALSE, 
		.filtering = AF_FALSE
	};
	return returnSprite;
}
