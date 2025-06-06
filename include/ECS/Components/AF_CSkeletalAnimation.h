/*
===============================================================================
AF_CSKELETALANIMATION_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CSKELETALANIMATION_H
#define AF_CSKELETALANIMATION_H
#include "AF_Component.h"

#ifdef __cplusplus
extern "C" {    
#endif

enum AnimationType {
  ANIMATION_TYPE_IDLE = 0,
  ANIMATION_TYPE_WALK = 1, 
  ANIMATION_TYPE_ATTACK = 2
};

/*
====================
AF_CSkeletalAnimation
====================
*/
// Size is 64 bytes
typedef struct {			// 64 or 128 bytes
	PACKED_CHAR enabled;		// 8 bytes
    void* model; 
    void* skeleton;
    void* skeletonBlend;
    const char* animIdlePath;
    // TODO: make this an array with defined indexs
    void* idleAnimationData;
    const char* animWalkPath;
    void* walkAnimationData;
    const char* animAttackPath;
    void* attackAnimationData;
    AF_FLOAT animationSpeed;
    AF_FLOAT animationBlend;
    enum AnimationType animationTypeID;
	AF_FLOAT nextFrameTime;		// 16 or 32 bytes
	uint8_t currentFrame;		// 8 bytes
	uint8_t animationFrames;	// 8 bytes
	BOOL loop;			// 8 bytes
} AF_CSkeletalAnimation;

AF_CSkeletalAnimation AF_CSkeletalAnimation_ZERO(void);
AF_CSkeletalAnimation AF_CSkeletalAnimation_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CSKELETALANIMATION_H
