#ifndef AF_RENDERER_BUFFER_H
#define AF_RENDERER_BUFFER_H

/*
===============================================================================
AF_RendererBuffer - Platform-agnostic buffer management interface

This header defines the API for buffer operations (creation, updating, deletion)
without exposing platform-specific details. Implementations exist for:
- OpenGL (AF_RendererBuffer_GL.c)
- DirectX (future)
- Vulkan (future)

The renderer uses these functions without knowing the underlying graphics API.
===============================================================================
*/

#include "AF_Lib_Define.h"
#include "AF_MeshData.h"
#include "ECS/Components/AF_CMesh.h"
#include "ECS/Components/AF_CText.h"
#include "ECS/Components/AF_CSprite.h"
#include "ECS/Components/AF_CCollider.h"
#include "AF_RenderingData.h"

#ifdef __cplusplus
extern "C" {
#endif

// =================================================================================================
// Structs
// =================================================================================================

// Camera UBO
typedef struct {
    AF_FLOAT view[16];       // 64 bytes
    AF_FLOAT projection[16]; // 64 bytes
    AF_FLOAT cameraPos[4];   // 16 bytes
    AF_FLOAT time;           // 4 bytes (perfectly fills the 16-byte chunk)
                          // Total: 160 bytes
} AF_CameraUBO_s;

// =================================================================================================
// Mesh Buffer Operations
// Create, update, and initialize vertex buffers for mesh rendering
// =================================================================================================

void AF_RendererBuffer_CreateMeshBuffer(AF_MeshData* _meshData);
void AF_RendererBuffer_UpdateMeshBufferData(AF_MeshData* _meshData);
void AF_RendererBuffer_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount);

// =================================================================================================
// Specialized Buffer Initialization
// Type-specific buffer setup for different rendering systems
// =================================================================================================

void AF_RendererBuffer_InitTextMeshBuffers(AF_CText* _fontComponent);
void AF_RendererBuffer_InitSpriteMeshBuffer(AF_CSprite* _spriteComponent);
void AF_RendererBuffer_InitInstancedTerrainMeshBuffer(uint32_t _gridSize, AF_CMesh* _mesh);
void AF_RendererBuffer_CreateCollisionGeometryMeshBuffer(AF_CCollider* _collider);
void AF_RendererBuffer_CreateScreenFBOQuadMeshBuffer(AF_RenderingData* _renderingData);

// =================================================================================================
// Buffer Cleanup/Deletion
// Free GPU resources
// =================================================================================================

void AF_RendererBuffer_DestroyMeshBuffers(AF_CMesh* _mesh);
void AF_RendererBuffer_DeleteScreenQuadBuffers(AF_RenderingData* _renderingData);

// =================================================================================================
// Buffer Drawing Operations
// Update buffer data and issue draw calls
// =================================================================================================

void AF_RendererBuffer_UpdateAndDrawSpriteBuffer(AF_CSprite* _spriteComponent, float vertices[6][5]);
void AF_RendererBuffer_UpdateAndDrawTextBuffer(AF_CText* _textComponent, float vertices[6][4]);

// =================================================================================================
// Uniform Buffer Objects (UBO)
// Create and update uniform buffers for shader data
// =================================================================================================

uint32_t AF_RendererBuffer_CreateCameraUBO(void);
void AF_RendererBuffer_UpdateCameraUBO(uint32_t uboID, AF_FLOAT* viewMatrix, AF_FLOAT* projMatrix, AF_FLOAT* camPos, AF_FLOAT currentTime);

#ifdef __cplusplus
}
#endif

#endif // AF_RENDERER_BUFFER_H
