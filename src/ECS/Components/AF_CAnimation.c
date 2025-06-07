#include "ECS/Components/AF_CAnimation.h"
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CAnimation_ZERO
Empty constructor for the AF_CAnimation component
====================
*/
AF_CAnimation AF_CAnimation_ZERO(void){
	AF_CAnimation returnAnimation = {
		//.has = false,
		.enabled = AF_FALSE,
		.animationSpeed = 0,
		.nextFrameTime = 0,
		.currentFrame = 0,
		.animationFrames = 0,
		.loop = AF_TRUE
	};
	return returnAnimation;
}

/*
====================
AF_CAnimation_ADD
ADD component and set default values
====================
*/
AF_CAnimation AF_CAnimation_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CAnimation returnAnimation = {
		//.has = true,
		.enabled = component,
		.animationSpeed = 0,
		.nextFrameTime = 0,
		.currentFrame = 0,
		.animationFrames = 0,
		.loop = AF_TRUE
	};
	return returnAnimation;
}
