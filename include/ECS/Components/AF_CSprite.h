/*
===============================================================================
AF_CSPRITE_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CSPRITE_H
#define AF_CSPRITE_H
#include <stdio.h>
#include "AF_Component.h"
#include "AF_Vec2.h"
#ifdef __cplusplus
extern "C" {    
#endif
/*
====================
AF_CSprite
====================
*/
// size is 64 bytes
typedef struct {
	//af_bool_t has;
	//af_bool_t enabled;
	PACKED_CHAR enabled;	    // 1 byte
	af_bool_t loop;		    // 1 byte
	char currentFrame;	    // 1 byte
	char animationFrames;	    // 1 byte
	AF_FLOAT currentFrameTime;
	AF_FLOAT nextFrameTime;     // 4 bytes
	AF_FLOAT animationSpeed;    // 4 bytes
	Vec2 spritePos;		    // 8 bytes
	Vec2 spriteSize;    	// size of sprite in pixels
	Vec2 spriteScale;		// transform scale
	float spriteRotation;	// rotation
	af_bool_t flipX;
	af_bool_t flipY;
	Vec2 spriteSheetSize;    // 8 bytes
	uint8_t spriteColor[4];
	const char* spritePath;
	void* spriteData; // special ptr for sprite data to be cast when known
	af_bool_t isPlaying;
	af_bool_t filtering;
	
} AF_CSprite;


AF_CSprite AF_CSprite_ZERO(void);
AF_CSprite AF_CSprite_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CSPRITE_H

