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
        .animIdlePath = NULL,
        // TODO: make this an array with defined indexs
        .idleAnimationData = NULL,
        .animWalkPath = NULL,
        .walkAnimationData = NULL,
        .animAttackPath = NULL,
        .attackAnimationData = 0,
        .animationSpeed = 0,
        .animationBlend = 0,
        .animationTypeID = ANIMATION_TYPE_IDLE,
        .nextFrameTime = 0,
        .currentFrame = 0,
        .animationFrames = 0,
        .loop = FALSE
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
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);
	AF_CSkeletalAnimation returnAnimation = {
    .enabled = component,
        .model = NULL,
        .skeleton = NULL,
        .skeletonBlend = NULL,
        .animIdlePath = NULL,
        // TODO: make this an array with defined indexs
        .idleAnimationData = NULL,
        .animWalkPath = NULL,
        .walkAnimationData = NULL,
        .animAttackPath = NULL,
        .attackAnimationData = 0,
        .animationSpeed = 0,
        .animationBlend = 0,
        .animationTypeID = ANIMATION_TYPE_IDLE,
        .nextFrameTime = 0,
        .currentFrame = 0,
        .animationFrames = 0,
        .loop = FALSE
	};
	return returnAnimation;
}

