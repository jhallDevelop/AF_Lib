/*
===============================================================================
AF_RendererFramebuffer_GL - OpenGL Framebuffer Management Implementation

OpenGL implementation of the platform-agnostic framebuffer interface.
Wraps AF_GL_Framebuffer functions to provide the AF_RendererFramebuffer API.
===============================================================================
*/

#include "AF_RendererFramebuffer.h"
#include "OpenGL/AF_GL_Framebuffer.h"

// =================================================================================================
// Framebuffer Object Creation
// =================================================================================================

uint32_t AF_RendererFramebuffer_CreateFBO(void)
{
	return AF_GL_CreateFBO();
}

uint32_t AF_RendererFramebuffer_CreateRBO(void)
{
	return AF_GL_CreateRBO();
}

uint32_t AF_RendererFramebuffer_CreateFBOTexture(AF_FrameBufferData* _frameBufferData)
{
	return AF_GL_CreateFBOTexture(_frameBufferData);
}

// =================================================================================================
// Framebuffer Setup Functions
// =================================================================================================

void AF_RendererFramebuffer_CreateDepthFrameBuffer(AF_FrameBufferData* _frameBufferData)
{
	AF_GL_CreateDepthFrameBuffer(_frameBufferData);
}

void AF_RendererFramebuffer_CreateFramebuffer(AF_FrameBufferData* _frameBufferData)
{
	AF_GL_CreateFramebuffer(_frameBufferData);
}

// =================================================================================================
// Framebuffer Binding Operations
// =================================================================================================

void AF_RendererFramebuffer_BindFrameBuffer(uint32_t _fBOID)
{
	AF_GL_BindFrameBuffer(_fBOID);
}

void AF_RendererFramebuffer_UnBindFrameBuffer(void)
{
	AF_GL_UnBindFrameBuffer();
}

void AF_RendererFramebuffer_BindFrameBufferToTexture(uint32_t _fBOID, uint32_t _textureID, uint32_t _textureAttatchmentType)
{
	AF_GL_BindFrameBufferToTexture(_fBOID, _textureID, _textureAttatchmentType);
}

void AF_RendererFramebuffer_BindRenderBuffer(uint32_t _rbo, uint32_t _screenWidth, uint32_t _screenHeight)
{
	AF_GL_BindRenderBuffer(_rbo, _screenWidth, _screenHeight);
}

// =================================================================================================
// Framebuffer Destruction
// =================================================================================================

void AF_RendererFramebuffer_DeleteFBO(uint32_t* _fboID)
{
	AF_GL_DeleteFBO(_fboID);
}

void AF_RendererFramebuffer_DeleteRBO(uint32_t* _rboID)
{
	AF_GL_DeleteRBO(_rboID);
}

void AF_RendererFramebuffer_DeleteTexture(uint32_t* _textureID)
{
	AF_GL_DeleteTexture(_textureID);
}

// =================================================================================================
// Framebuffer Utilities
// =================================================================================================

void AF_RendererFramebuffer_CheckFrameBufferStatus(const char* _message)
{
	AF_GL_CheckFrameBufferStatus(_message);
}
