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
#include "AF_Math/AF_Vec2.h"
#include "AF_CMesh.h"
#ifdef __cplusplus
extern "C" {    
#endif

#define AF_CSPRITE_DEFAULT_SPRITE_MESH_NAME "plane"
#define AF_CSPRITE_DEFAULT_SPRITE_VERT_PATH "sprite.vert"
#define AF_CSPRITE_DEFAULT_SPRITE_FRAG_PATH "sprite.frag"
#define AF_CSPRITE_DEFAULT_SPRITE_TEXTURE_NAME "atlas.png"

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
	Vec2 spriteFramePos;	// atlas position
	Vec2 spriteFrameSize; 	// atlas frame size
	float spriteRotation;	// rotation
	af_bool_t flipX;
	af_bool_t flipY;
	Vec2 spriteSheetSize;    // 8 bytes
	Vec2 spriteSheetPos;     // 8 bytes
	uint8_t spriteColor[4];
	// TODO: move strings out to a separate struct or use a string library
	void* spriteData; // special ptr for sprite data to be cast when known
	af_bool_t isPlaying;
	af_bool_t filtering;
	AF_CMesh spriteMesh; // 64 bytes
	
} AF_CSprite;


AF_CSprite AF_CSprite_ZERO(void);
AF_CSprite AF_CSprite_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CSPRITE_H

