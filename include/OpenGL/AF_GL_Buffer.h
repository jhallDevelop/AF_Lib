#ifndef AF_GL_BUFFER_H
#define AF_GL_BUFFER_H

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
// Mesh Buffer Operations
// =================================================================================================
void AF_GL_CreateMeshBuffer(AF_MeshData* _meshData);
void AF_GL_UpdateMeshBufferData(AF_MeshData* _meshData);
void AF_GL_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount);

// =================================================================================================
// Specialized Buffer Initialization
// =================================================================================================
void AF_GL_InitTextMeshBuffers(AF_CText* _fontComponent);
void AF_GL_InitSpriteMeshBuffer(AF_CSprite* _spriteComponent);
void AF_GL_InitInstancedTerrainMeshBuffer(uint32_t _gridSize, AF_CMesh* _mesh);
void AF_GL_CreateCollisionGeometryMeshBuffer(AF_CCollider* _collider);

// =================================================================================================
// Screen Quad Buffer
// =================================================================================================
void AF_GL_CreateScreenFBOQuadMeshBuffer(AF_RenderingData* _renderingData);

// =================================================================================================
// Buffer Cleanup/Deletion
// =================================================================================================
void AF_GL_DestroyMeshBuffers(AF_CMesh* _mesh);
void AF_GL_DeleteScreenQuadBuffers(AF_RenderingData* _renderingData);

// =================================================================================================
// Buffer Drawing Operations
// =================================================================================================
void AF_GL_UpdateAndDrawSpriteBuffer(AF_CSprite* _spriteComponent, float vertices[6][5]);
void AF_GL_UpdateAndDrawTextBuffer(AF_CText* _textComponent, float vertices[6][4]);

// =================================================================================================
// Uniform Buffer Objects (UBO)
// =================================================================================================
uint32_t AF_GL_CreateCameraUBO(void);
void AF_GL_UpdateCameraUBO(uint32_t uboID, AF_FLOAT* viewMatrix, AF_FLOAT* projMatrix, AF_FLOAT* camPos, AF_FLOAT currentTime);

#ifdef __cplusplus
}
#endif

#endif // AF_GL_BUFFER_H
