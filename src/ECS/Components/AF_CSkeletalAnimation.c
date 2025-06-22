#include "ECS/Components/AF_CSkeletalAnimation.h"
#include "ECS/Components/AF_Component.h"
#include <stddef.h> // For NULL

/*
====================
AF_CSkeletalAnimation_ZERO
Empty constructor for the AF_CAnimation component
====================
*/
AF_CSkeletalAnimation AF_CSkeletalAnimation_ZERO(void){
	AF_CSkeletalAnimation returnAnimation = {
		.enabled = 0,
        .model = NULL,
        .skeleton = NULL,
        .skeletonBlend = NULL,
        .animIdlePath = "\0",
        // TODO: make this an array with defined indexs
        .idleAnimationData = NULL,
        .animWalkPath = "\0",
        .walkAnimationData = NULL,
        .animAttackPath = "\0",
        .attackAnimationData = 0,
        .animationSpeed = 0,
        .animationBlend = 0,
        .animationTypeID = ANIMATION_TYPE_IDLE,
        .nextFrameTime = 0,
        .currentFrame = 0,
        .animationFrames = 0,
        .loop = AF_FALSE
	};
	return returnAnimation;
}

/*
====================
AF_CSkeletalAnimation_ADD
ADD component and set default values
====================
*/
AF_CSkeletalAnimation AF_CSkeletalAnimation_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CSkeletalAnimation returnAnimation = {
    .enabled = component,
        .model = NULL,
        .skeleton = NULL,
        .skeletonBlend = NULL,
        .animIdlePath = "\0",
        // TODO: make this an array with defined indexs
        .idleAnimationData = NULL,
        .animWalkPath = "\0",
        .walkAnimationData = NULL,
        .animAttackPath = "\0",
        .attackAnimationData = 0,
        .animationSpeed = 0,
        .animationBlend = 0,
        .animationTypeID = ANIMATION_TYPE_IDLE,
        .nextFrameTime = 0,
        .currentFrame = 0,
        .animationFrames = 0,
        .loop = AF_FALSE
	};
	return returnAnimation;
}

