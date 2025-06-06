/*
===============================================================================
AF_CANIMATION_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CANIMATION_H
#define AF_CANIMATION_H
#include "AF_Component.h"

#ifdef __cplusplus
extern "C" {    
#endif
/*
====================
AF_CAnimation
====================
*/
// Size is 64 bytes
typedef struct {			// 64 or 128 bytes
	PACKED_CHAR enabled;		// 8 bytes
	AF_FLOAT animationSpeed;	// 16 or 32 bytes
	AF_FLOAT nextFrameTime;		// 16 or 32 bytes
	uint8_t currentFrame;		// 8 bytes
	uint8_t animationFrames;	// 8 bytes
	BOOL loop;			// 8 bytes
} AF_CAnimation;

AF_CAnimation AF_CAnimation_ZERO(void);
AF_CAnimation AF_CAnimation_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CANIMATION_H

