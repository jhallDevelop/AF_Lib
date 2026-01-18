/*
===============================================================================
AF_RENDERER_H

Definition for functions for rendering 
===============================================================================
*/
#ifndef AF_RENDERER_H
#define AF_RENDERER_H
#include "AF_Lib_API.h"
#include "AF_Window.h"
#include "ECS/Components/AF_CCamera.h"
#include "ECS/Components/AF_CTerrain.h"
#include "AF_MeshData.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_LightingData.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_CSprite.h"
#include "AF_Debug.h"
#include "AF_Time.h"
#include "AF_Assets.h"
#include "AF_RenderingData.h"
#include "AF_FrameBufferData.h"

#ifdef __cplusplus
extern "C" {
#endif



// ============================ DEFAULT ================================ 
AF_LIB_API af_bool_t AF_Renderer_Awake(void);
AF_LIB_API af_bool_t AF_Renderer_Start(AF_RenderingData* _renderingData, AF_ECS* _ecs, const char* _platform, uint16_t* _screenWidth, uint16_t* _screenHeight);
AF_LIB_API void AF_Renderer_Update(AF_ECS* _ecs, AF_Time* _time);
AF_LIB_API void AF_Renderer_Render(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, uint32_t _cameraID);
AF_LIB_API void AF_Renderer_Finish(void);
AF_LIB_API void AF_Renderer_DestroyRenderer(AF_RenderingData* _renderingData, AF_ECS* _ecs);
AF_LIB_API void AF_Renderer_Shutdown(AF_ECS* _ecs);


// ============================ TEXTURES ================================ 
// Textures
void AF_Renderer_SetFlipImage(af_bool_t _flipImage);
uint32_t AF_Renderer_LoadTexture(char const * path);
void AF_Renderer_ReLoadTexture(AF_Assets* _assets, AF_Texture* _texture);
void AF_Renderer_SetTexture(const uint32_t _shaderID, const char* _shaderVarName, uint32_t _textureID);
void AF_Renderer_SetTerrainHeightMap(const uint32_t _shaderID, AF_CTerrain* _terrain);

// ============================  Forward Rendering ================================ 
void AF_Renderer_StartForwardRendering(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, uint32_t _cameraID);
void AF_Renderer_EndForwardRendering(void);

// ============================  DRAW ================================ 
//void AF_Renderer_StartRendering(Vec4 _backgroundColor);
void AF_Renderer_EarlyRendering(AF_RenderingData* _renderingData, Vec4 _backgroundColor);
void AF_Renderer_DrawCollisionMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData);
void AF_Renderer_DrawMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData);
void AF_Renderer_DrawMesh(Mat4* _modelMat, Mat4* _viewMat, Mat4* _projMat, AF_CMesh* _mesh, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData);
void AF_Renderer_RenderScreenFBOQuad(AF_RenderingData* _renderingData);
void AF_Renderer_RenderScreenDebugFBOQuad(AF_RenderingData* _renderingData);

// ============================  MESH BUFFERS ================================ 
// NOTE: Buffer management functions have been moved to AF_RendererBuffer.h
// Use AF_RendererBuffer_* functions for buffer operations
void AF_Renderer_InitCollisionGeomtery(AF_ECS* _ecs);

// ============================  FRAME BUFFERS ================================ 
// NOTE: Framebuffer management functions have been moved to AF_RendererFramebuffer.h
// Use AF_RendererFramebuffer_* functions for framebuffer operations
void AF_Renderer_FrameResized(void* _renderingData);

// ============================  DEPTH ================================ 
void AF_Renderer_StartDepthPass(AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_ECS* _ecs, uint32_t _cameraID);
void AF_Renderer_Start_DepthFrameBuffers(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight);


// ============================  LIGHTING ================================ 
void AF_Renderer_RenderForwardPointLights(uint32_t _shader, AF_ECS* _ecs, AF_LightingData* _lightingData);
void AF_Renderer_UpdateLighting(AF_ECS* _ecs, AF_LightingData* _lightingData);

// ============================  ANIMATION ================================ 

void AF_Renderer_PlayAnimation(AF_CSkeletalAnimation* _animation);

// ============================  DRAW TEXT ================================
// NOTE: AF_Renderer_InitTextMeshBuffers moved to AF_RendererBuffer.h
void AF_Renderer_DrawTextMeshes(AF_ECS* _ecs, AF_RenderingData* _renderingData);

// ============================  DRAW SPRITES ================================
// NOTE: AF_Renderer_InitSpriteMeshBuffer moved to AF_RendererBuffer.h
void AF_Renderer_DrawSpriteMeshes(AF_ECS* _ecs, AF_RenderingData* _renderingData);
void AF_Renderer_DrawTestTriangle(void);
void AF_Renderer_DrawTextMeshes(AF_ECS* _ecs, AF_RenderingData* _renderingData);

// ============================  DESTROY / CLEANUP ================================ 
// Destroy
// NOTE: AF_Renderer_DestroyMeshBuffers moved to AF_RendererBuffer.h
// NOTE: Framebuffer deletion functions moved to AF_RendererFramebuffer.h
void AF_Renderer_Destroy_Material_Textures(AF_Material* _material);


// ====================================== HELPER FUNCTIONS =====================================
// NOTE: AF_Renderer_CheckFrameBufferStatus moved to AF_RendererFramebuffer.h
void AF_Renderer_SetPolygonMode(AF_Renderer_PolygonMode_e _polygonMode);

// NOTE: UBO functions moved to AF_RendererBuffer.h
// Use AF_RendererBuffer_CreateCameraUBO() and AF_RendererBuffer_UpdateCameraUBO()

#ifdef __cplusplus
}
#endif

#endif // AF_RENDERER_H
