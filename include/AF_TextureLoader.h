#ifndef AF_TEXTURE_LOADER_H
#define AF_TEXTURE_LOADER_H

#include "AF_Lib_Define.h"
#include "AF_Texture.h"
#include "AF_Assets.h"

#ifdef __cplusplus
extern "C" {
#endif

// =================================================================================================
// AF_TextureLoader_LoadTexture
// Load a texture from file path and return OpenGL texture ID
// Returns 0 if loading fails
// =================================================================================================
uint32_t AF_TextureLoader_LoadTexture(const char* _path);

// =================================================================================================
// AF_TextureLoader_ReLoadTexture
// Reload a texture from its path, checking asset cache first
// Updates the provided texture structure with loaded data
// =================================================================================================
void AF_TextureLoader_ReLoadTexture(AF_Assets* _assets, AF_Texture* _texture);

// =================================================================================================
// AF_TextureLoader_SetFlipImage
// Set whether loaded textures should be flipped vertically (for OpenGL coordinate systems)
// =================================================================================================
void AF_TextureLoader_SetFlipImage(af_bool_t _flipImage);

#ifdef __cplusplus
}
#endif

#endif // AF_TEXTURE_LOADER_H
