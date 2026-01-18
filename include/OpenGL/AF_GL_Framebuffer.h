#ifndef AF_GL_FRAMEBUFFER_H
#define AF_GL_FRAMEBUFFER_H

#include "AF_Lib_Define.h"
#include "AF_FrameBufferData.h"

#ifdef __cplusplus
extern "C" {
#endif

// =================================================================================================
// Framebuffer Object Creation
// =================================================================================================
uint32_t AF_GL_CreateFBO(void);
uint32_t AF_GL_CreateRBO(void);
uint32_t AF_GL_CreateFBOTexture(AF_FrameBufferData* _frameBufferData);

// =================================================================================================
// Framebuffer Setup Functions
// =================================================================================================
void AF_GL_CreateDepthFrameBuffer(AF_FrameBufferData* _frameBufferData);
void AF_GL_CreateFramebuffer(AF_FrameBufferData* _frameBufferData);

// =================================================================================================
// Framebuffer Binding Operations
// =================================================================================================
void AF_GL_BindFrameBuffer(uint32_t _fBOID);
void AF_GL_UnBindFrameBuffer(void);
void AF_GL_BindFrameBufferToTexture(uint32_t _fBOID, uint32_t _textureID, uint32_t _textureAttatchmentType);
void AF_GL_BindRenderBuffer(uint32_t _rbo, uint32_t _screenWidth, uint32_t _screenHeight);

// =================================================================================================
// Framebuffer Destruction
// =================================================================================================
void AF_GL_DeleteFBO(uint32_t* _fboID);
void AF_GL_DeleteRBO(uint32_t* _rboID);
void AF_GL_DeleteTexture(uint32_t* _textureID);

// =================================================================================================
// Framebuffer Utilities
// =================================================================================================
void AF_GL_CheckFrameBufferStatus(const char* _message);

#ifdef __cplusplus
}
#endif

#endif // AF_GL_FRAMEBUFFER_H
