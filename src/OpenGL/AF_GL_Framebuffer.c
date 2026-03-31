/*
===============================================================================
AF_GL_Framebuffer Implementation

OpenGL framebuffer object management
Handles FBO, RBO, and associated texture creation and binding
===============================================================================
*/
#include "OpenGL/AF_GL_Framebuffer.h"
#include "AF_Log.h"

#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl3.h>
#else
	#include <GL/glew.h>
#endif

// =================================================================================================
// AF_GL_CreateFBO
// Create framebuffer object
// =================================================================================================
uint32_t AF_GL_CreateFBO(void){
	unsigned int fBO;
	glGenFramebuffers(1, &fBO);
	return fBO;
}

// =================================================================================================
// AF_GL_CreateRBO
// Create render buffer object
// =================================================================================================
uint32_t AF_GL_CreateRBO(void){
	unsigned int rBO;
	glGenRenderbuffers(1, &rBO);
	return rBO;
}

// =================================================================================================
// AF_GL_CreateDepthFrameBuffer
// Create depth framebuffer for shadow mapping
// =================================================================================================
void AF_GL_CreateDepthFrameBuffer(AF_FrameBufferData* _frameBufferData) {
	if (_frameBufferData == NULL) {
		AF_Log_Error("AF_GL_CreateDepthFrameBuffer: _frameBufferData is NULL.\n");
		return;
	}
	
	// Delete existing resources if they exist
	AF_GL_DeleteFBO(&_frameBufferData->fbo);
	AF_GL_DeleteTexture(&_frameBufferData->textureID);

	// Generate framebuffer
	_frameBufferData->fbo = AF_GL_CreateFBO();
	AF_GL_BindFrameBuffer(_frameBufferData->fbo);

	// Create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	
	#ifdef AF_WEB_BUILD
		//AF_Log("AF_GL_CreateDepthFrameBuffer: WEB\n");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
			_frameBufferData->textureWidth, _frameBufferData->textureHeight, 0, 
			GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#else
		//AF_Log("AF_GL_CreateDepthFrameBuffer: Desktop\n");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
			_frameBufferData->textureWidth, _frameBufferData->textureHeight, 0, 
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	#endif

	// Attach depth texture as FBO's depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	
	#ifndef AF_WEB_BUILD
		glDrawBuffer(GL_NONE);
	#endif

	// Check framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		AF_Log_Error("AF_GL_CreateDepthFrameBuffer: Framebuffer not complete!\n");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture to avoid state leaking
	_frameBufferData->textureID = depthMap;
}

// =================================================================================================
// AF_GL_CreateFramebuffer
// Create FBO, RBO, and Texture for color framebuffer rendering
// =================================================================================================
void AF_GL_CreateFramebuffer(AF_FrameBufferData* _frameBufferData){
	if (_frameBufferData == NULL) {
		AF_Log_Error("AF_GL_CreateFramebuffer: _frameBufferData is NULL.\n");
		return;
	}

	// Delete existing resources
	AF_GL_DeleteFBO(&_frameBufferData->fbo);
	AF_GL_DeleteRBO(&_frameBufferData->rbo);
	AF_GL_DeleteTexture(&_frameBufferData->textureID);
	
	// Generate and bind framebuffer
	_frameBufferData->fbo = AF_GL_CreateFBO();
	AF_GL_BindFrameBuffer(_frameBufferData->fbo);

	// Generate and attach color texture
	_frameBufferData->textureID = AF_GL_CreateFBOTexture(_frameBufferData);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _frameBufferData->textureID, 0);

	// Generate and attach depth/stencil renderbuffer
	_frameBufferData->rbo = AF_GL_CreateRBO();
	glBindRenderbuffer(GL_RENDERBUFFER, _frameBufferData->rbo);
	
	#ifdef AF_WEB_BUILD
		AF_Log("AF_GL_CreateFramebuffer: WEB glRenderbufferStorage GL_DEPTH_STENCIL\n");
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, _frameBufferData->textureWidth, _frameBufferData->textureHeight);
	#else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _frameBufferData->textureWidth, _frameBufferData->textureHeight);
	#endif
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _frameBufferData->rbo);

	// Check for completeness
	AF_GL_CheckFrameBufferStatus("AF_GL_CreateFramebuffer");
	
	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// =================================================================================================
// AF_GL_CreateFBOTexture
// Create texture for framebuffer attachment
// =================================================================================================
uint32_t AF_GL_CreateFBOTexture(AF_FrameBufferData* _frameBufferData) {
	unsigned int fboTextureID = 0;
	glGenTextures(1, &fboTextureID);
	glBindTexture(GL_TEXTURE_2D, fboTextureID);

	GLenum internalFormat;
	GLenum format;
	GLenum type;

	// Determine correct formats and type based on requested internal format
	if ((GLenum)_frameBufferData->internalFormat == GL_DEPTH_COMPONENT) {
		format = GL_DEPTH_COMPONENT;
		#ifdef AF_WEB_BUILD
			internalFormat = GL_DEPTH_COMPONENT;
			type = GL_UNSIGNED_SHORT;
		#else
			internalFormat = GL_DEPTH_COMPONENT24;
			type = GL_FLOAT;
		#endif
	} else if ((GLenum)_frameBufferData->internalFormat == GL_RGBA || (GLenum)_frameBufferData->internalFormat == GL_RGBA16F || (GLenum)_frameBufferData->internalFormat == GL_SRGB8_ALPHA8) {
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		#ifdef AF_WEB_BUILD
			internalFormat = GL_RGBA8;
		#else
			internalFormat = _frameBufferData->internalFormat;
		#endif
	} else { // Default to RGB
		#ifdef AF_WEB_BUILD
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
		#else
			format = GL_RGB;
			internalFormat = GL_RGB8;
		#endif
		type = GL_UNSIGNED_BYTE;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
		_frameBufferData->textureWidth, _frameBufferData->textureHeight, 0,
		format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)_frameBufferData->minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)_frameBufferData->magFilter);

	// Set texture wrapping parameters
	#ifdef AF_WEB_BUILD
		AF_Log("AF_GL_CreateFBOTexture: WEB GL_CLAMP_TO_EDGE\n");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#else
		if (format == GL_DEPTH_COMPONENT) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	#endif

	glBindTexture(GL_TEXTURE_2D, 0);
	return fboTextureID;
}

// =================================================================================================
// AF_GL_BindFrameBuffer
// Bind framebuffer to GPU
// =================================================================================================
void AF_GL_BindFrameBuffer(uint32_t _fBOID){
	glBindFramebuffer(GL_FRAMEBUFFER, _fBOID);
}

// =================================================================================================
// AF_GL_UnBindFrameBuffer
// Unbind framebuffer (bind default framebuffer)
// =================================================================================================
void AF_GL_UnBindFrameBuffer(void){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// =================================================================================================
// AF_GL_BindFrameBufferToTexture
// Bind framebuffer and attach texture
// =================================================================================================
void AF_GL_BindFrameBufferToTexture(uint32_t _fBOID, uint32_t _textureID, uint32_t _textureAttatchmentType){
	glBindFramebuffer(GL_FRAMEBUFFER, _fBOID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, _textureAttatchmentType, GL_TEXTURE_2D, _textureID, 0);
}

// =================================================================================================
// AF_GL_BindRenderBuffer
// Bind renderbuffer and allocate storage
// =================================================================================================
void AF_GL_BindRenderBuffer(uint32_t _rbo, uint32_t _screenWidth, uint32_t _screenHeight){
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _screenWidth, _screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// =================================================================================================
// AF_GL_DeleteFBO
// Delete framebuffer object
// =================================================================================================
void AF_GL_DeleteFBO(uint32_t* _fboID){
	if (_fboID && *_fboID != 0) {
		glDeleteFramebuffers(1, _fboID);
		*_fboID = 0;
	}
}

// =================================================================================================
// AF_GL_DeleteRBO
// Delete renderbuffer object
// =================================================================================================
void AF_GL_DeleteRBO(uint32_t* _rboID){
	if (_rboID && *_rboID != 0) {
		glDeleteRenderbuffers(1, _rboID);
		*_rboID = 0;
	}
}

// =================================================================================================
// AF_GL_DeleteTexture
// Delete texture object
// =================================================================================================
void AF_GL_DeleteTexture(uint32_t* _textureID){
	if (_textureID && *_textureID != 0) {
		glDeleteTextures(1, _textureID);
		*_textureID = 0;
	}
}

// =================================================================================================
// AF_GL_CheckFrameBufferStatus
// Check and log framebuffer completion status
// =================================================================================================
void AF_GL_CheckFrameBufferStatus(const char* _message){
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		const char* statusStr = "";
		switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED:                     statusStr = "GL_FRAMEBUFFER_UNDEFINED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         statusStr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        statusStr = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        statusStr = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED:                   statusStr = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
			default:                                           statusStr = "Unknown Error"; break;
		}
		AF_Log_Error("AF_GL_CheckFrameBufferStatus: ERROR::FRAMEBUFFER:: Framebuffer is not complete! Status: 0x%x (%s): %s\n", status, statusStr, _message);
	}
}
