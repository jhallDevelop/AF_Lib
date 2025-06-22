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
    // TODO: move strings out to a separate struct or use a string library
    char animIdlePath[AF_MAX_PATH_CHAR_SIZE];
    // TODO: make this an array with defined indexs
    void* idleAnimationData;
    char animWalkPath[AF_MAX_PATH_CHAR_SIZE];
    void* walkAnimationData;
    char animAttackPath[AF_MAX_PATH_CHAR_SIZE]; // 256 bytes
    void* attackAnimationData;
    AF_FLOAT animationSpeed;
    AF_FLOAT animationBlend;
    enum AnimationType animationTypeID;
	AF_FLOAT nextFrameTime;		// 16 or 32 bytes
	uint8_t currentFrame;		// 8 bytes
	uint8_t animationFrames;	// 8 bytes
	af_bool_t loop;			// 8 bytes
} AF_CSkeletalAnimation;

AF_CSkeletalAnimation AF_CSkeletalAnimation_ZERO(void);
AF_CSkeletalAnimation AF_CSkeletalAnimation_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CSKELETALANIMATION_H
