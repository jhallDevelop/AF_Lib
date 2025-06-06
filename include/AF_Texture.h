/*
===============================================================================
AF_TEXTURE_H defninitions

Implementation of texture struct and initialization function
===============================================================================
*/

#ifndef AF_TEXTURE_H
#define AF_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Texture types, matched with the first 6 types of assimp textures types
enum AF_TextureType {
	AF_TEXTURE_TYPE_NONE = 0,
    AF_TEXTURE_TYPE_DIFFUSE = 1,
    AF_TEXTURE_TYPE_SPECULAR = 2,
	AF_TEXTURE_TYPE_AMBIET = 3,
    AF_TEXTURE_EMISSIVE = 4,
	AF_TEXTURE_TYPE_HEIGHT = 5,
	AF_TEXTURE_TYPE_NORMALS = 6
};

typedef struct AF_Texture {
	// TODO pack this
	uint32_t id;
	enum AF_TextureType type;
	char path[256];	// TODO: this looks a little small
} AF_Texture;

static inline AF_Texture AF_Texture_ZERO(void){
	AF_Texture returnTexture = {
		0,	//.id 
		AF_TEXTURE_TYPE_NONE, //.type = 
		"\0"	// .path =
	};

	return returnTexture;
}

#ifdef __cplusplus
}
#endif

#endif  // AF_TEXTURE_H

