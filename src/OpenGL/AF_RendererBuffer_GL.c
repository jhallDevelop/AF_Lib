/*
===============================================================================
AF_RendererBuffer_GL - OpenGL Buffer Management Implementation

OpenGL implementation of the platform-agnostic buffer interface.
Wraps AF_GL_Buffer functions to provide the AF_RendererBuffer API.
===============================================================================
*/

#include "AF_RendererBuffer.h"
#include "OpenGL/AF_GL_Buffer.h"

// =================================================================================================
// Mesh Buffer Operations
// =================================================================================================

void AF_RendererBuffer_CreateMeshBuffer(AF_MeshData* _meshData)
{
	AF_GL_CreateMeshBuffer(_meshData);
}

void AF_RendererBuffer_UpdateMeshBufferData(AF_MeshData* _meshData)
{
	AF_GL_UpdateMeshBufferData(_meshData);
}

void AF_RendererBuffer_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount)
{
	AF_GL_InitMeshBuffers(_mesh, _entityCount);
}

// =================================================================================================
// Specialized Buffer Initialization
// =================================================================================================

void AF_RendererBuffer_InitTextMeshBuffers(AF_CText* _fontComponent)
{
	AF_GL_InitTextMeshBuffers(_fontComponent);
}

void AF_RendererBuffer_InitSpriteMeshBuffer(AF_CSprite* _spriteComponent)
{
	AF_GL_InitSpriteMeshBuffer(_spriteComponent);
}

void AF_RendererBuffer_InitInstancedTerrainMeshBuffer(uint32_t _gridSize, AF_CMesh* _mesh)
{
	AF_GL_InitInstancedTerrainMeshBuffer(_gridSize, _mesh);
}

void AF_RendererBuffer_CreateCollisionGeometryMeshBuffer(AF_CCollider* _collider)
{
	AF_GL_CreateCollisionGeometryMeshBuffer(_collider);
}

void AF_RendererBuffer_CreateScreenFBOQuadMeshBuffer(AF_RenderingData* _renderingData)
{
	AF_GL_CreateScreenFBOQuadMeshBuffer(_renderingData);
}

// =================================================================================================
// Buffer Cleanup/Deletion
// =================================================================================================

void AF_RendererBuffer_DestroyMeshBuffers(AF_CMesh* _mesh)
{
	AF_GL_DestroyMeshBuffers(_mesh);
}

void AF_RendererBuffer_DeleteScreenQuadBuffers(AF_RenderingData* _renderingData)
{
	AF_GL_DeleteScreenQuadBuffers(_renderingData);
}

// =================================================================================================
// Buffer Drawing Operations
// =================================================================================================

void AF_RendererBuffer_UpdateAndDrawSpriteBuffer(AF_CSprite* _spriteComponent, float vertices[6][5])
{
	AF_GL_UpdateAndDrawSpriteBuffer(_spriteComponent, vertices);
}

void AF_RendererBuffer_UpdateAndDrawTextBuffer(AF_CText* _textComponent, float vertices[6][4])
{
	AF_GL_UpdateAndDrawTextBuffer(_textComponent, vertices);
}

// =================================================================================================
// Uniform Buffer Objects (UBO)
// =================================================================================================

uint32_t AF_RendererBuffer_CreateCameraUBO(void)
{
	return AF_GL_CreateCameraUBO();
}

void AF_RendererBuffer_UpdateCameraUBO(uint32_t uboID, AF_FLOAT* viewMatrix, AF_FLOAT* projMatrix, AF_FLOAT* camPos, AF_FLOAT currentTime)
{
	AF_GL_UpdateCameraUBO(uboID, viewMatrix, projMatrix, camPos, currentTime);
}
