#ifndef AF_RENDERER_FRAMEBUFFER_H
#define AF_RENDERER_FRAMEBUFFER_H

/*
===============================================================================
AF_RendererFramebuffer - Platform-agnostic framebuffer management interface

This header defines the API for framebuffer operations (creation, binding, deletion)
without exposing platform-specific details. Implementations exist for:
- OpenGL (AF_RendererFramebuffer_GL.c)
- DirectX (future)
- Vulkan (future)

The renderer uses these functions without knowing the underlying graphics API.
===============================================================================
*/

#include "AF_Lib_Define.h"
#include "AF_FrameBufferData.h"
#include "AF_RenderingData.h"

#ifdef __cplusplus
extern "C" {
#endif

// =================================================================================================
// Framebuffer Object Creation
// Create FBO, RBO, and FBO textures
// =================================================================================================

uint32_t AF_RendererFramebuffer_CreateFBO(void);
uint32_t AF_RendererFramebuffer_CreateRBO(void);
uint32_t AF_RendererFramebuffer_CreateFBOTexture(AF_FrameBufferData* _frameBufferData);

// =================================================================================================
// Framebuffer Setup Functions
// Initialize framebuffers for depth and general rendering
// =================================================================================================

void AF_RendererFramebuffer_CreateDepthFrameBuffer(AF_FrameBufferData* _frameBufferData);
void AF_RendererFramebuffer_CreateFramebuffer(AF_FrameBufferData* _frameBufferData);

// =================================================================================================
// Framebuffer Binding Operations
// Bind/unbind framebuffers and attach textures/renderbuffers
// =================================================================================================

void AF_RendererFramebuffer_BindFrameBuffer(uint32_t _fBOID);
void AF_RendererFramebuffer_UnBindFrameBuffer(void);
void AF_RendererFramebuffer_BindFrameBufferToTexture(uint32_t _fBOID, uint32_t _textureID, uint32_t _textureAttatchmentType);
void AF_RendererFramebuffer_BindRenderBuffer(uint32_t _rbo, uint32_t _screenWidth, uint32_t _screenHeight);

// =================================================================================================
// Framebuffer Destruction
// Free GPU resources for FBO, RBO, and textures
// =================================================================================================

void AF_RendererFramebuffer_DeleteFBO(uint32_t* _fboID);
void AF_RendererFramebuffer_DeleteRBO(uint32_t* _rboID);
void AF_RendererFramebuffer_DeleteTexture(uint32_t* _textureID);

// =================================================================================================
// Framebuffer Utilities
// Validation and helper functions
// =================================================================================================

void AF_RendererFramebuffer_CheckFrameBufferStatus(const char* _message);

#ifdef __cplusplus
}
#endif

#endif // AF_RENDERER_FRAMEBUFFER_H
