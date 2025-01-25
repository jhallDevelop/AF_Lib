/*
===============================================================================
AF_TEXTURE_H defninitions

Implementation of texture struct and initialization function
===============================================================================
*/

#ifndef AF_TEXTUREL_H
#define AF_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum AF_TextureType {
    AF_TEXTURE_DIFFUSE = 0,
    AF_TEXTURE_SPECULAR = 1,
    AF_TEXTURE_NORMAL = 2
};

typedef struct AF_Texture {
	// TODO pack this
	uint32_t id;
	AF_TextureType type;
	uint32_t path;
} AF_Texture;

static inline AF_Texture AF_Texture_ZERO(void){
	AF_Texture returnTexture = {
		.id = 0,
		.type = AF_TEXTURE_DIFFUSE, 
		.path = 0
	};

	return returnTexture;
}

#ifdef __cplusplus
}
#endif

#endif  // AF_TEXTURE_H

