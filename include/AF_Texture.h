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

#include <stdio.h>
#include <stdint.h>
#include "AF_Lib_Define.h"

//#define AF_INPUT_KEYS_MAPPED 10
#define AF_TEXTURE_TYPE_COUNT 8


// Texture types, matched with the first 6 types of assimp textures types
enum AF_TextureType_e {
	AF_TEXTURE_TYPE_NONE = 0,
    AF_TEXTURE_TYPE_DIFFUSE = 1,
    AF_TEXTURE_TYPE_SPECULAR = 2,
	AF_TEXTURE_TYPE_AMBIENT = 3,
    AF_TEXTURE_EMISSIVE = 4,
	AF_TEXTURE_TYPE_HEIGHT = 5,
	AF_TEXTURE_TYPE_NORMALS = 6,
	AF_TEXTURE_TYPE_RENDER_TEXTURE = 7
};

typedef struct { 
    enum AF_TextureType_e type;
    const char* name;
} AF_TextureTypeMap;


static const AF_TextureTypeMap AF_Texture_TypeMappings[] = {
    {AF_TEXTURE_TYPE_NONE, "0"},
    {AF_TEXTURE_TYPE_DIFFUSE, "Diffuse"},
    {AF_TEXTURE_TYPE_SPECULAR, "Specular"},
    {AF_TEXTURE_TYPE_AMBIENT, "Ambient"},
    {AF_TEXTURE_EMISSIVE, "Emissive"},
    {AF_TEXTURE_TYPE_HEIGHT, "Height Map"},
    {AF_TEXTURE_TYPE_NORMALS, "Normal"},
    {AF_TEXTURE_TYPE_RENDER_TEXTURE, "Render Texture"}
};


// Define the key mappings
extern const AF_TextureTypeMap AF_Texture_TypeMappings[];

typedef struct AF_Texture {
	// TODO pack this
	uint32_t id;
	enum AF_TextureType_e type;
	char path[AF_MAX_PATH_CHAR_SIZE];	// TODO: this looks a little small
	AF_FLOAT uvOffsetX; // offset for texture coordinates
	AF_FLOAT uvOffsetY;
	AF_FLOAT uvScaleX;  // scale for texture coordinates
	AF_FLOAT uvScaleY;	
} AF_Texture;

static inline AF_Texture AF_Texture_ZERO(void){
	AF_Texture returnTexture = {
		0,	//.id 
		AF_TEXTURE_TYPE_NONE, //.type = 
		"\0",	// .path =
		0.0f,
		0.0f,
		1.0f,
		1.0f
	};

	return returnTexture;
}


/*
================
AF_Texture_Typemappings_ConvertToCharArray
// Construct and return the keys as an array of chars
================
*/
inline static void AF_Texture_TypeMap_ConvertToCharArray(const AF_TextureTypeMap* _textureMappings, const char** _charArray, uint32_t _size) {
    if (!_textureMappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _textureMappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}

#ifdef __cplusplus
}
#endif

#endif  // AF_TEXTURE_H

