/*
===============================================================================
AF_GL_Buffer Implementation

OpenGL buffer management for VAO/VBO/EBO creation
Handles mesh, text, sprite, terrain, and collision geometry buffers
===============================================================================
*/
#include "OpenGL/AF_GL_Buffer.h"
#include "AF_RendererBuffer.h"
#include "AF_Log.h"
#include "AF_Vertex.h"
#include "AF_Debug.h"
#include "ECS/Components/AF_Component.h"
#include <string.h>
#include <stdlib.h>

#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl3.h>
#else
	#include <GL/glew.h>
#endif

// External quad vertices for screen rendering
extern float QUAD_VERTICES[];

// Debug error checking
#ifdef AF_DEBUG
	extern void AF_Renderer_CheckError(const char* msg);
	#define AF_GL_CHECK_ERROR(msg) AF_Renderer_CheckError(msg)
#else
	#define AF_GL_CHECK_ERROR(msg) ((void)0)
#endif

// =================================================================================================
// AF_GL_CreateMeshBuffer
// Create VAO/VBO/EBO for a mesh and upload vertex/index data
// =================================================================================================
void AF_GL_CreateMeshBuffer(AF_MeshData* _meshData){
	if(_meshData == NULL){
		AF_Log_Error("AF_GL_CreateMeshBuffer: Invalid _meshData, is NULL!\n");
		return;
	}
	
	if (_meshData->vertexCount == 0 || _meshData->indexCount == 0) {
		AF_Log_Error("AF_GL_CreateMeshBuffer: Invalid vertex or index data!\n");
		return;
	}
		
	int vertexBufferSize = _meshData->vertexCount * sizeof(AF_Vertex);
	AF_GL_CHECK_ERROR("OpenGL error occurred just before VAO, VBO, EBO buffer creation.\n");
		
	glGenVertexArrays(1, &_meshData->vao);
	glGenBuffers(1, &_meshData->vbo);
	glGenBuffers(1, &_meshData->ibo);
	AF_GL_CHECK_ERROR("OpenGL error occurred during VAO, VBO, EBO buffer creation.\n");

	// Bind VAO first, then set up buffers and attributes
	glBindVertexArray(_meshData->vao);
	glBindBuffer(GL_ARRAY_BUFFER, _meshData->vbo);
	AF_GL_CHECK_ERROR("OpenGL error occurred during binding of the VAO, VBO.\n");

	// Upload vertex data
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, _meshData->vertices, GL_STATIC_DRAW);
	AF_GL_CHECK_ERROR("OpenGL error occurred during glBufferData for the verts.\n");

	// Bind and upload index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshData->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _meshData->indexCount * sizeof(uint32_t), &_meshData->indices[0], GL_STATIC_DRAW);
	AF_GL_CHECK_ERROR("OpenGL error occurred during glBufferData for the indexes.\n");

	// Vertex positions (location = 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, position));

	// Vertex normals (location = 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, normal));
	
	// Vertex texture coords (location = 2)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, texCoord));

	// Vertex tangent (location = 3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, tangent));

	// Vertex bitangent (location = 4)
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, bitangent));

	AF_GL_CHECK_ERROR("OpenGL error occurred during assignment of vertexAttribs.\n");

	// Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 

	AF_GL_CHECK_ERROR("Error AF_GL_CreateMeshBuffer for OpenGL! \n");
}

// =================================================================================================
// AF_GL_UpdateMeshBufferData
// Update existing VBO data (requires GL_DYNAMIC_DRAW buffer)
// =================================================================================================
void AF_GL_UpdateMeshBufferData(AF_MeshData* _meshData) {
	if(_meshData == NULL || _meshData->vertices == NULL) {
		AF_Log_Error("AF_GL_UpdateMeshBufferData: Invalid _meshData or vertices is NULL!\n");
		return;
	}	

	if(_meshData->vbo == 0) {
		AF_Log_Error("AF_GL_UpdateMeshBufferData: VBO is 0, cannot update!\n");
		return;
	}

	uint32_t vertexBufferSize = _meshData->vertexCount * sizeof(AF_Vertex);
	glBindBuffer(GL_ARRAY_BUFFER, _meshData->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBufferSize, _meshData->vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	
	AF_GL_CHECK_ERROR("AF_GL_UpdateMeshBufferData: Error updating mesh buffer data!\n");
}

// =================================================================================================
// AF_GL_InitMeshBuffers
// Initialize buffers for all meshes in a CMesh component
// =================================================================================================
void AF_GL_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount){ 
	if (_entityCount == 0) {
		AF_Log_Error("AF_GL_InitMeshBuffers: No meshes to draw!\n");
		return;
	}

	for(uint32_t i = 0; i < _entityCount; i++){
		af_bool_t hasMesh = AF_Component_GetHas(_mesh->enabled);
		if(hasMesh == AF_FALSE){
			continue;
		}

		AF_GL_CHECK_ERROR("Mesh has no indices!\n");

		// Create buffer for each sub-mesh
		for(uint32_t j = 0; j < _mesh->meshCount; j++){
			if(_mesh->meshes[j].vertexCount < 1){
				AF_Log_Warning("AF_GL_InitMeshBuffers: skip creating mesh buffer as we don't have any vertices\n");
				continue;
			}
			AF_GL_CreateMeshBuffer(&_mesh->meshes[j]);
		}
	}
}

// =================================================================================================
// AF_GL_InitTextMeshBuffers
// Create dynamic VBO for text rendering (updated per character)
// =================================================================================================
void AF_GL_InitTextMeshBuffers(AF_CText* _fontComponent){
	if (_fontComponent == NULL) {
		AF_Log_Error("AF_GL_InitTextMeshBuffers: _fontComponent is NULL!\n");
		return;
	}
	
	AF_GL_CHECK_ERROR("AF_GL_InitTextMeshBuffers: before create text mesh buffers\n");
	
	_fontComponent->mesh.meshCount = 1;
	_fontComponent->mesh.meshes[0].vertexCount = 6; // Quad = 6 vertices
	_fontComponent->mesh.meshes[0].indexCount = 0; // Using glDrawArrays
	_fontComponent->mesh.meshes[0].vertices = NULL; // Dynamic data
	
	glGenVertexArrays(1, &_fontComponent->mesh.meshes[0].vao);
	glGenBuffers(1, &_fontComponent->mesh.meshes[0].vbo);
	glBindVertexArray(_fontComponent->mesh.meshes[0].vao);
	glBindBuffer(GL_ARRAY_BUFFER, _fontComponent->mesh.meshes[0].vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AF_FLOAT) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(AF_FLOAT), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	AF_GL_CHECK_ERROR("AF_GL_InitTextMeshBuffers: after create text mesh buffers\n");
}

// =================================================================================================
// AF_GL_InitSpriteMeshBuffer
// Create static quad buffer for sprite rendering
// =================================================================================================
void AF_GL_InitSpriteMeshBuffer(AF_CSprite* _spriteComponent){
	if (_spriteComponent == NULL) {
		AF_Log_Error("AF_GL_InitSpriteMeshBuffer: _spriteComponent is NULL!\n");
		return;
	}
	
	AF_GL_CHECK_ERROR("AF_GL_InitSpriteMeshBuffer: before create sprite mesh buffers\n");
	
	// Simple quad vertices
	float vertices[] = {
		// positions        // texture Coords
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,

		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Position attribute (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// Texture coord attribute (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	_spriteComponent->spriteMesh.meshCount = 1;
	_spriteComponent->spriteMesh.meshes[0].vertexCount = 6;
	_spriteComponent->spriteMesh.meshes[0].indexCount = 0;
	_spriteComponent->spriteMesh.meshes[0].vao = VAO;
	_spriteComponent->spriteMesh.meshes[0].vbo = VBO;

	AF_GL_CHECK_ERROR("AF_GL_InitSpriteMeshBuffer: after create sprite mesh buffers\n");
}

// =================================================================================================
// AF_GL_InitInstancedTerrainMeshBuffer
// Create empty VAO for GPU-generated terrain (vertices created in shader via gl_VertexID)
// =================================================================================================
void AF_GL_InitInstancedTerrainMeshBuffer(uint32_t _gridSize, AF_CMesh* _mesh){
	if(_mesh == NULL){
		AF_Log_Error("AF_GL_InitInstancedTerrainMeshBuffer: Mesh is NULL\n");
		return;
	}

	// Create empty VAO - shader generates all vertices via gl_VertexID
	if(_mesh->meshes[0].vao == 0){
		glGenVertexArrays(1, &_mesh->meshes[0].vao);
	}
	
	AF_Log("AF_GL_InitInstancedTerrainMeshBuffer: Created empty VAO for GPU-generated terrain (gridSize=%u)\n", _gridSize);
}

// =================================================================================================
// AF_GL_CreateCollisionGeometryMeshBuffer
// Generate unit cube mesh for collision visualization
// =================================================================================================
void AF_GL_CreateCollisionGeometryMeshBuffer(AF_CCollider* _collider){
	if(_collider == NULL){
		AF_Log_Error("AF_GL_CreateCollisionGeometryMeshBuffer: Collider is NULL\n");
		return;
	}

	// Unit cube: 8 vertices, 36 indices (12 triangles)
	_collider->collisionMeshData.vertexCount = 8;
	_collider->collisionMeshData.indexCount = 36;
	
	_collider->collisionMeshData.vertices = (AF_Vertex*)malloc(sizeof(AF_Vertex) * _collider->collisionMeshData.vertexCount);
	_collider->collisionMeshData.indices = (uint32_t*)malloc(sizeof(uint32_t) * _collider->collisionMeshData.indexCount);
	
	if(!_collider->collisionMeshData.vertices || !_collider->collisionMeshData.indices){
		AF_Log_Error("AF_GL_CreateCollisionGeometryMeshBuffer: malloc failed\n");
		free(_collider->collisionMeshData.vertices);
		free(_collider->collisionMeshData.indices);
		return;
	}

	// Initialize vertices (zero all fields first)
	AF_Vertex vertices[8] = {0};
	
	// Unit cube vertex positions
	Vec3 positions[8] = {
		{-1.0f, -1.0f,  1.0f}, // 0
		{-1.0f,  1.0f,  1.0f}, // 1
		{-1.0f, -1.0f, -1.0f}, // 2
		{-1.0f,  1.0f, -1.0f}, // 3
		{ 1.0f, -1.0f,  1.0f}, // 4
		{ 1.0f,  1.0f,  1.0f}, // 5
		{ 1.0f, -1.0f, -1.0f}, // 6
		{ 1.0f,  1.0f, -1.0f}  // 7
	};
	
	for(int i = 0; i < 8; i++) {
		vertices[i].position = positions[i];
		_collider->collisionMeshData.vertices[i] = vertices[i];
	}

	// Cube face indices
	uint32_t indices[36] = {
		0,1,3,  0,3,2,  // Face 1
		2,3,7,  2,7,6,  // Face 2
		6,7,5,  6,5,4,  // Face 3
		4,5,1,  4,1,0,  // Face 4
		2,6,4,  2,4,0,  // Face 5
		7,3,1,  7,1,5   // Face 6
	};

	memcpy(_collider->collisionMeshData.indices, indices, sizeof(indices));

	// Upload to GPU
	AF_GL_CreateMeshBuffer(&_collider->collisionMeshData);
	
	// Free CPU memory after upload
	if(_collider->collisionMeshData.vertices != NULL){
		free(_collider->collisionMeshData.vertices);
		_collider->collisionMeshData.vertices = NULL;
	}

	if(_collider->collisionMeshData.indices != NULL){
		free(_collider->collisionMeshData.indices);
		_collider->collisionMeshData.indices = NULL;
	}
}

// =================================================================================================
// AF_GL_CreateScreenFBOQuadMeshBuffer
// Create fullscreen quad for FBO rendering
// =================================================================================================
void AF_GL_CreateScreenFBOQuadMeshBuffer(AF_RenderingData* _renderingData){
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, &QUAD_VERTICES, GL_STATIC_DRAW); // 6 vertices * 4 floats
	
	// Position attribute (location = 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	
	// Texture coord attribute (location = 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	_renderingData->screenQUAD_VAO = quadVAO;
	_renderingData->screenQUAD_VBO = quadVBO;
}

// =================================================================================================
// AF_GL_DestroyMeshBuffers
// Delete OpenGL buffers (VAO, VBO, IBO) and free CPU memory for mesh vertices and indices
// =================================================================================================
void AF_GL_DestroyMeshBuffers(AF_CMesh* _mesh)
{
	// for each mesh
	for(uint32_t j = 0; j < _mesh->meshCount; j++)
	{
		AF_MeshData* mesh = &_mesh->meshes[j];
		if(mesh == NULL)
		{
			AF_Log_Warning("AF_GL_DestroyMeshBuffers: skipping destroy of mesh %i\n", j);
			continue;
		}

		glDeleteVertexArrays(1, &mesh->vao);
		glDeleteBuffers(1, &mesh->vbo);
		glDeleteBuffers(1, &mesh->ibo);

		// Free CPU memory for vertices and indices
		if(mesh->vertices != NULL)
		{
			free(mesh->vertices);
		}
		mesh->vertices = NULL;

		if(mesh->indices != NULL)
		{
			free(mesh->indices);
		}
		mesh->indices = NULL;
	}
}

// =================================================================================================
// AF_GL_DeleteScreenQuadBuffers
// Delete the screen quad VAO and VBO used for framebuffer rendering
// =================================================================================================
void AF_GL_DeleteScreenQuadBuffers(AF_RenderingData* _renderingData)
{
	if(_renderingData == NULL)
	{
		AF_Log_Error("AF_GL_DeleteScreenQuadBuffers: _renderingData is NULL!\n");
		return;
	}

	glDeleteVertexArrays(1, &_renderingData->screenQUAD_VAO);
	glDeleteBuffers(1, &_renderingData->screenQUAD_VBO);
}

// =================================================================================================
// AF_GL_UpdateAndDrawSpriteBuffer
// Update sprite VBO with new vertex data and draw the sprite quad
// =================================================================================================
void AF_GL_UpdateAndDrawSpriteBuffer(AF_CSprite* _spriteComponent, float vertices[6][5])
{
	if(_spriteComponent == NULL)
	{
		AF_Log_Error("AF_GL_UpdateAndDrawSpriteBuffer: _spriteComponent is NULL!\n");
		return;
	}

	glBindVertexArray(_spriteComponent->spriteMesh.meshes[0].vao);
	glBindBuffer(GL_ARRAY_BUFFER, _spriteComponent->spriteMesh.meshes[0].vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 5, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// =================================================================================================
// AF_GL_UpdateAndDrawTextBuffer
// Update text VBO with character glyph data and draw the character quad
// =================================================================================================
void AF_GL_UpdateAndDrawTextBuffer(AF_CText* _textComponent, float vertices[6][4])
{
	if(_textComponent == NULL)
	{
		AF_Log_Error("AF_GL_UpdateAndDrawTextBuffer: _textComponent is NULL!\n");
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _textComponent->mesh.meshes[0].vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 4, vertices);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// =================================================================================================
// AF_GL_CreateCameraUBO
// Create a uniform buffer object for camera data (view, projection, position, time)
// =================================================================================================
uint32_t AF_GL_CreateCameraUBO(void)
{
	uint32_t uboID;
	glGenBuffers(1, &uboID);
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(AF_CameraUBO_s), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Associate the UBO (uboID) with the global binding point 0.
	// This makes the buffer's data available to any shader that binds its uniform block
	// to this same point. This only needs to be done once.
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboID);

	return uboID;
}

// =================================================================================================
// AF_GL_UpdateCameraUBO
// Updates the camera UBO with view/projection matrices, camera position, and current time
// =================================================================================================
void AF_GL_UpdateCameraUBO(uint32_t uboID, AF_FLOAT* viewMatrix, AF_FLOAT* projMatrix, AF_FLOAT* camPos, AF_FLOAT currentTime)
{
	AF_CameraUBO_s uboData;

	// Copy view matrix
	memcpy(uboData.view, viewMatrix, sizeof(AF_FLOAT) * 16);
	memcpy(uboData.projection, projMatrix, sizeof(AF_FLOAT) * 16);

	// Copy vector and scalar cam position
	uboData.cameraPos[0] = camPos[0];
	uboData.cameraPos[1] = camPos[1];
	uboData.cameraPos[2] = camPos[2];
	uboData.cameraPos[3] = 1.0f; // padding
	uboData.time = currentTime;

	// upload to GPU
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(AF_CameraUBO_s), &uboData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);	
}
