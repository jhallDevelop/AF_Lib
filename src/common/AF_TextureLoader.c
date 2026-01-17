#include "AF_TextureLoader.h"
#include "AF_Log.h"

#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl3.h>
#else
	#include <GL/glew.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../../../stb/stb_image.h"

// =================================================================================================
// AF_TextureLoader_SetFlipImage
// Set the flip image for stb_image.h
// =================================================================================================
void AF_TextureLoader_SetFlipImage(af_bool_t _flipImage) {
	bool isFlipped = false;
	if(_flipImage == AF_FALSE) {
		isFlipped = false;
	}
	else {
		isFlipped = true;
	}
	
	stbi_set_flip_vertically_on_load(isFlipped);
}

// =================================================================================================
// AF_TextureLoader_LoadTexture
// Load textures from file path
// =================================================================================================
uint32_t AF_TextureLoader_LoadTexture(const char* path) {
	if (!path || path[0] == '\0') {
		AF_Log_Error("AF_TextureLoader_LoadTexture: Null or empty texture path provided.\n");
		return 0; // Return 0 for invalid path
	}

	uint32_t textureID = 0; // Initialize to 0
	int width, height, nrComponents;
	
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data) {
		glBindTexture(GL_TEXTURE_2D, 0); // free the old bind texture if deleted
		glGenTextures(1, &textureID); // Generate ID only if data is loaded
		glBindTexture(GL_TEXTURE_2D, textureID);

		GLenum internalFormat = GL_RGB;
		GLenum dataFormat = GL_RGB;
		if (nrComponents == 1) {
			internalFormat = GL_RED; 
			dataFormat = GL_RED;
		}
		else if (nrComponents == 3) {
			#ifdef AF_WEB_BUILD
				internalFormat = GL_RGB;
			#else
				internalFormat = GL_RGB8; 
			#endif
			dataFormat = GL_RGB; // Use sized internal format
		}
		else if (nrComponents == 4) {
			#ifdef AF_WEB_BUILD
				internalFormat = GL_RGBA;
			#else
				internalFormat = GL_RGBA8; // Use sized internal format on desktop
			#endif
			dataFormat = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0); // Good practice: unbind after configuring
		stbi_image_free(data);
		return textureID;
	}
	else {
		AF_Log_Error("AF_TextureLoader_LoadTexture: Texture failed to load at path: \"%s\" (stbi_load error: %s)\n", path, stbi_failure_reason());
		// No textureID was generated and bound with data, so return 0
		return 0;
	}
}

// =================================================================================================
// AF_TextureLoader_ReLoadTexture
// Reload textures
// =================================================================================================
void AF_TextureLoader_ReLoadTexture(AF_Assets* _assets, AF_Texture* _texture) {
	if (!_texture || _texture->path[0] == '\0') {
		AF_Log_Error("AF_TextureLoader_ReLoadTexture: Null or empty texture path provided.\n");
		return;
	}

	// Check cache first if you don't want to *always* reload from disk
	AF_Texture cachedTexture = AF_Assets_GetTexture(_assets, _texture->path);
	if (cachedTexture.type != AF_TEXTURE_TYPE_NONE) {
		// copy the texture data from the cached version
		*_texture = cachedTexture;
		return;
	}

	// Load texture for first time
	_texture->id = AF_TextureLoader_LoadTexture(_texture->path);

	if (_texture->id == 0) {
		AF_Log_Error("AF_TextureLoader_ReLoadTexture: Call to AF_TextureLoader_LoadTexture failed for path: %s\n", _texture->path);
		return;
	}

	// Add/update in asset manager
	AF_Assets_AddTexture(_assets, *_texture);
}
