/*
===============================================================================
AF_OpenGL_Renderer Implementation

Implementation of the AF_Renderer rendering functions
This implementation is for OpenGL
===============================================================================
*/
#include <stdio.h>
#include "AF_Lib_Define.h"
#include "AF_Renderer.h"
#include "AF_Debug.h"
#include "ECS/Components/AF_Component.h"
#include "AF_Log.h"
#include "AF_Math/AF_Vec3.h"
#include "AF_Math/AF_Mat4.h"
#include "AF_MeshLoad.h"
#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION
#include "AF_Util.h"

#include "AF_Assets.h"
#include "AF_Renderer_Util.h"
#include "ECS/Components/AF_CText.h"
#include "AF_Lighting.h"
#include "AF_TextureLoader.h"

#define NO_SHARED_SHADER 0

// Debug error checking macro - only enabled in debug builds
#ifdef AF_DEBUG
	#define AF_RENDERER_CHECK_GL_ERROR(msg) AF_Renderer_CheckError(msg)
#else
	#define AF_RENDERER_CHECK_GL_ERROR(msg) ((void)0)
#endif

// string to use in logging
const char* openglRendererFileTitle = "AF_OpenGL_Renderer:";

float QUAD_VERTICES[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

// OpenGL errors
const char* invalidEnum = "INVALID_ENUM";
const char* invalidValue = "INVALID_VALUE";
const char* invalidOperation = "INVALID_OPERATION";
const char* stackOverflow = "STACK_OVERFLOW";
const char* stackUnderflow = "STACK_UNDERFLOW";
const char* outOfMemory = "OUT_OF_MEMORY";
const char* invalidFrameBufferOperation = "INVALID_FRAMEBUFFER_OPERATION";
const char* SHADER_ASSET_PATH = "assets/shaders";

/*
====================
AF_Renderer_Awake
Init OpenGL
====================
*/
af_bool_t AF_Renderer_Awake(void){
    af_bool_t success = AF_TRUE;
    AF_Log("AF_Renderer_Awake\n");
    //Initialize GLEW
    #ifndef AF_WEB_BUILD
		//Initialize GLEW
		glewExperimental = GL_TRUE; 
		GLenum glewError = glewInit();
		AF_Renderer_CheckError( "Error initializing GLEW! \n");
	#endif

    // -----------------------------
    // Enable transparent blending
	//glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // if in 2d mode, disable depth testing
    //glDisable(GL_DEPTH_TEST);

	
	// FACE CULLING
	// TODO: Adjust per model
	
	
    AF_Renderer_CheckError( "Error initializing OpenGL! \n");
    //AF_Renderer_CheckError("SLDGameRenderer::Initialise:: finishing up init: ");
    return success;
} 

/*
====================
AF_Renderer_Start
Start function which occurs after everything is loaded in.
====================
*/
af_bool_t AF_Renderer_Start(AF_RenderingData* _renderingData, AF_ECS* _ecs, const char* _platform, uint16_t* _screenWidth, uint16_t* _screenHeight){
	AF_Log("AF_Renderer_Start\n");
	if(_renderingData == NULL || _screenWidth == NULL || _screenHeight == NULL){}
	
	// ==== Setup Screen FBO (for main scene render to ImGui viewport) ====
    if (_screenWidth != NULL && _screenHeight != NULL && *_screenWidth > 0 && *_screenHeight > 0) {
        //AF_Renderer_Start_ScreenFrameBuffers(&_renderingData->screenFBO_ID, &_renderingData->screenRBO_ID, &_renderingData->screenFBO_ShaderID, &_renderingData->screenFBO_TextureID, _screenWidth, _screenHeight, SCREEN_VERT_SHADER_PATH, SCREEN_FRAG_SHADER_PATH, "screenTexture");
		char screenVertShaderFullPath[AF_MAX_PATH_CHAR_SIZE];
		char screenFragShaderFullPath[AF_MAX_PATH_CHAR_SIZE];
		snprintf(screenVertShaderFullPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform,  SCREEN_VERT_SHADER_PATH);
		snprintf(screenFragShaderFullPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, SCREEN_FRAG_SHADER_PATH);
		int32_t screenBufferShaderID = AF_Shader_Load(screenVertShaderFullPath, screenFragShaderFullPath);
		if(screenBufferShaderID < 0){
			AF_Log_Error("AF_Renderer_Start: Failed to loadscreenVertShader\n");
			return AF_FALSE;
		}
		AF_FrameBufferData screenBufferData = {
			.fbo = 0,
			.rbo = 0,
			.shaderID = screenBufferShaderID,
			.textureID = 0,
			.textureWidth = *_screenWidth,
			.textureHeight = *_screenHeight,
			.shaderTextureName = "screenTexture",
			#ifdef AF_WEB_BUILD
                .internalFormat = GL_SRGB8_ALPHA8, // Use sRGB format for WebGL for correct gamma
            #else
                .internalFormat = GL_RGB,
            #endif
			.textureAttatchmentType = GL_COLOR_ATTACHMENT0,
			.drawBufferType = GL_TRUE,
			.readBufferType = GL_TRUE,
			.minFilter = GL_LINEAR,
			.magFilter = GL_LINEAR
		};

		// copy the shader paths in
		snprintf(screenBufferData.shader.name, AF_MAX_PATH_CHAR_SIZE, "%s", SCREEN_SHADER_NAME);
		snprintf(screenBufferData.shader.vertPath, AF_MAX_PATH_CHAR_SIZE, "%s", screenVertShaderFullPath);
		snprintf(screenBufferData.shader.fragPath, AF_MAX_PATH_CHAR_SIZE, "%s", screenFragShaderFullPath);


		
		// Set the screen Frame buffer texture
		AF_Shader_Use(screenBufferData.shaderID);
		AF_Shader_SetInt(screenBufferData.shaderID, screenBufferData.shaderTextureName, 0);
		AF_Shader_Use(0);


		// ============ Screen Buffer ============
		// copy to the render data to use
		_renderingData->screenFrameBufferData = screenBufferData;
	
		// ==== Setup Depth Map and Texture ====
		_renderingData->depthDebugFrameBufferData = screenBufferData;

		// setup depth frame buffer
		char depthVertShaderFullPath[AF_MAX_PATH_CHAR_SIZE];
		char depthFragShaderFullPath[AF_MAX_PATH_CHAR_SIZE];
		snprintf(depthVertShaderFullPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, DEPTH_VERT_SHADER_PATH);
		snprintf(depthFragShaderFullPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, DEPTH_FRAG_SHADER_PATH);
		
		int32_t depthBufferShaderID = AF_Shader_Load(depthVertShaderFullPath, depthFragShaderFullPath);
		if(depthBufferShaderID < 0){
			AF_Log_Error("AF_Renderer_Start: Failed to load depth buffer shader ID\n");
			return AF_FALSE;
		}
		AF_FrameBufferData depthBufferData = {
			.fbo = 0,
			.rbo = 0,
			.shaderID = depthBufferShaderID,
			.textureID = 0,
			.textureWidth = AF_RENDERINGDATA_SHADOW_WIDTH,
			.textureHeight = AF_RENDERINGDATA_SHADOW_HEIGHT,
			.shaderTextureName = "",
			.internalFormat = GL_DEPTH_COMPONENT,//GL_RGB,
			.textureAttatchmentType = GL_DEPTH_ATTACHMENT, //GL_COLOR_ATTACHMENT0,
			.drawBufferType = GL_FALSE, //GL_TRUE,
			.readBufferType = GL_FALSE, //GL_TRUE,
			.minFilter = GL_NEAREST, //GL_LINEAR,
			.magFilter = GL_NEAREST //GL_LINEAR
		};

		// copy the shader paths in
		snprintf(depthBufferData.shader.name, AF_MAX_PATH_CHAR_SIZE, "%s", DEPTH_SHADER_NAME);
		snprintf(depthBufferData.shader.vertPath, AF_MAX_PATH_CHAR_SIZE, "%s", depthVertShaderFullPath);
		snprintf(depthBufferData.shader.fragPath, AF_MAX_PATH_CHAR_SIZE, "%s", depthFragShaderFullPath);

		
		// ============== Depth Debug Buffer ==============
		// Set the screen Frame buffer texture
		_renderingData->depthFrameBufferData = depthBufferData;

		// Setup depth debug frame buffer
		// setup depth frame buffer
		char depthDebugVertShaderFullPath[AF_MAX_PATH_CHAR_SIZE];
		char depthDebugFragShaderFullPath[AF_MAX_PATH_CHAR_SIZE];
		snprintf(depthDebugVertShaderFullPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, DEPTH_DEBUG_VERT_SHADER_PATH);
		snprintf(depthDebugFragShaderFullPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, DEPTH_DEBUG_FRAG_SHADER_PATH);
		
		int32_t depthDebugShaderID = AF_Shader_Load(depthDebugVertShaderFullPath, depthDebugFragShaderFullPath);
		if(depthDebugShaderID < 0){
			AF_Log_Error("AF_Renderer_Start: Failed to load depth debug shader ID\n");
			return AF_FALSE;
		}
		AF_FrameBufferData depthDebugBufferData = {
			.fbo = 0,
			.rbo = 0,
			.shaderID = depthDebugShaderID,
			.textureID = 0,
			.textureWidth = *_screenWidth,
			.textureHeight = *_screenHeight,
			.shaderTextureName = "depthMap",
			.internalFormat = GL_RGB,
			.textureAttatchmentType = GL_COLOR_ATTACHMENT0,
			.drawBufferType = GL_TRUE,
			.readBufferType = GL_TRUE,
			.minFilter = GL_LINEAR,
			.magFilter = GL_LINEAR
		};

		// copy the shader paths in
		snprintf(depthDebugBufferData.shader.name, AF_MAX_PATH_CHAR_SIZE, "%s", DEPTH_DEBUG_SHADER_NAME);
		snprintf(depthDebugBufferData.shader.vertPath, AF_MAX_PATH_CHAR_SIZE, "%s", depthDebugVertShaderFullPath);
		snprintf(depthDebugBufferData.shader.fragPath, AF_MAX_PATH_CHAR_SIZE, "%s", depthDebugFragShaderFullPath);


		// ============== Render Texture Buffers ==============
		// For each render texture in the scene, if there is a camera attatched to it
		// Then create a frame buffer for it
		for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
			AF_CCamera* cameraComponent = &_ecs->cameras[i];
			if(AF_Component_GetHasEnabled(cameraComponent->enabled) == AF_TRUE){
				if(cameraComponent->enableRenderToTexture == AF_FALSE){
					//AF_Log_Error("AF_Renderer_Start: Camera already has a render texture ID, skipping creation\n");
					continue;
				}
				if(cameraComponent->renderTextureWidth > 0 && cameraComponent->renderTextureHeight > 0){
					AF_FrameBufferData renderTextureBufferData = {
						.fbo = 0,
						.rbo = 0,
						.shaderID = screenBufferShaderID,
						.textureID = 0,
						.textureWidth = cameraComponent->renderTextureWidth,
						.textureHeight = cameraComponent->renderTextureHeight, 
						.shaderTextureName = "screenTexture",
						#ifdef AF_WEB_BUILD
							.internalFormat = GL_SRGB8_ALPHA8, // Use sRGB format for WebGL for correct gamma
						#else
							.internalFormat = GL_RGB,
						#endif
						.textureAttatchmentType = GL_COLOR_ATTACHMENT0,
						.drawBufferType = GL_TRUE,
						.readBufferType = GL_TRUE,
						.minFilter = GL_LINEAR,
						.magFilter = GL_LINEAR
					};

					// copy the shader paths in
					snprintf(renderTextureBufferData.shader.name, AF_MAX_PATH_CHAR_SIZE, "%s", SCREEN_SHADER_NAME);
					snprintf(renderTextureBufferData.shader.vertPath, AF_MAX_PATH_CHAR_SIZE, "%s", screenVertShaderFullPath);
					snprintf(renderTextureBufferData.shader.fragPath, AF_MAX_PATH_CHAR_SIZE, "%s", screenFragShaderFullPath);
					
					// Set the screen Frame buffer texture
					AF_Shader_Use(renderTextureBufferData.shaderID);
					AF_Shader_SetInt(renderTextureBufferData.shaderID, renderTextureBufferData.shaderTextureName, 0);
					AF_Shader_Use(0);
					// copy to the render data to use
					cameraComponent->renderTextureData = renderTextureBufferData;

					// create the frame buffer for the camera
					AF_Renderer_CreateFramebuffer(&cameraComponent->renderTextureData);
				} else {
					AF_Log_Error("AF_Renderer_Start: Camera Render Texture has invalid width or height\n");
				}
			}
		}	
	

		AF_Shader_Use(depthDebugBufferData.shaderID);
		AF_Shader_SetInt(depthDebugBufferData.shaderID, depthDebugBufferData.shaderTextureName, 0);
		AF_Shader_Use(0);
		// Set the screen Frame buffer texture
		_renderingData->depthDebugFrameBufferData = depthDebugBufferData;
    } else {
        AF_Log_Error("AF_Renderer_Start: Screen dimensions not valid for initial screen FBO setup. Attempting with default or expect Editor_Viewport_Render to create.\n"); 
		return AF_FALSE;
    }
	
	// Recreate the quad mesh buffers
	AF_Renderer_CreateScreenFBOQuadMeshBuffer(_renderingData);


	// Create the collision Geometry debug shader stuff for rendering debug lines ect.
	char guizmoDebugFragShaderPath[AF_MAX_PATH_CHAR_SIZE];
	char guizmoDebugVertShaderPath[AF_MAX_PATH_CHAR_SIZE];

	snprintf(guizmoDebugFragShaderPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, DEBUG_GEOMETRY_FRAG_SHADER_PATH);
	snprintf(guizmoDebugVertShaderPath, AF_MAX_PATH_CHAR_SIZE, "%s/%s/%s", SHADER_ASSET_PATH, _platform, DEBUG_GEOMETRY_VERT_SHADER_PATH);	
	
	int32_t guizmoShaderID = AF_Shader_Load(guizmoDebugVertShaderPath, guizmoDebugFragShaderPath);
	_renderingData->guizmoDebugShaderID = guizmoShaderID;
	if(guizmoShaderID < 0){
		AF_Log_Error("AF_Renderer_Start: Failed to load guizmoDebugShader\n");
		return AF_FALSE;
	}

	// Create Camera UBO
	_renderingData->cameraUBO = AF_Renderer_CreateCameraUBO();

	return AF_TRUE;
	
}

void AF_Renderer_CreateCollisionGeometryMeshBuffer(AF_CCollider* _collider){
	if(_collider == NULL){
		AF_Log_Error("AF_Renderer_CreateCollisionGeometryMeshBuffer: Collider is NULL\n");
		return;
	}

	// for now, everything is a box
	_collider->collisionMeshData.vertexCount = 8;
	_collider->collisionMeshData.indexCount = 36;
	
	
	_collider->collisionMeshData.vertices = (AF_Vertex*)malloc(sizeof(AF_Vertex) * _collider->collisionMeshData.vertexCount);
	_collider->collisionMeshData.indices = (uint32_t*)malloc(sizeof(uint32_t) * _collider->collisionMeshData.indexCount);
	if(!_collider->collisionMeshData.vertices || !_collider->collisionMeshData.indices){
        AF_Log_Error("AF_Renderer_CreateCollisionGeometryMeshBuffer: malloc failed\n");
        free(_collider->collisionMeshData.vertices);
        free(_collider->collisionMeshData.indices);
		
        return;
    }

	
	// Initialize vertices (zero all fields first, then set positions)
    AF_Vertex vertices[8] = {0};
  
    
    // Set positions for unit cube vertices
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
        // normal, tangent, bitangent, and texCoord are already zero-initialized
        _collider->collisionMeshData.vertices[i] = vertices[i];
    }

    // Indices for cube faces (from OBJ conversion)
    uint32_t indices[36] = {
        0,1,3,  0,3,2,  // Face 1
        2,3,7,  2,7,6,  // Face 2
        6,7,5,  6,5,4,  // Face 3
        4,5,1,  4,1,0,  // Face 4
        2,6,4,  2,4,0,  // Face 5
        7,3,1,  7,1,5   // Face 6
    };

    // Copy indices to allocated memory
    memcpy(_collider->collisionMeshData.indices, indices, sizeof(indices));

	// load the mesh data to the GPU, the Create meshbuffer function handles 
	// The delete of the mesh data vert, and indicies memory, however double check it
	AF_Renderer_CreateMeshBuffer(&_collider->collisionMeshData);
	if(_collider->collisionMeshData.vertices != NULL){
		free(_collider->collisionMeshData.vertices);
		_collider->collisionMeshData.vertices = NULL;
	}

	if(_collider->collisionMeshData.indices != NULL){
		free(_collider->collisionMeshData.indices);
		_collider->collisionMeshData.indices = NULL;
	}
}

void AF_Renderer_InitCollisionGeomtery(AF_ECS* _ecs){
	// for each entity
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		// if the entity has a collider component
		AF_Entity* entity = &_ecs->entities[i];
		AF_CCollider* collider = &_ecs->colliders[i];
		if(AF_Component_GetHasEnabled(collider->enabled) == AF_TRUE){
			// create the collision geometry
			AF_Renderer_CreateCollisionGeometryMeshBuffer(collider);
		}
	}
}

// =================================================================================================
// AF_Renderer_InitGPUTerrainMeshBuffer
// Creates an empty VAO for GPU-generated terrain (vertices generated in shader using gl_VertexID)
// No vertex data is uploaded - the shader generates all geometry procedurally
// =================================================================================================
void AF_Renderer_InitInstancedTerrainMeshBuffer(uint32_t _gridSize, AF_CMesh* _mesh){
	if(_mesh == NULL){
		AF_Log_Error("AF_Renderer_InitGPUTerrainMeshBuffer: Mesh is NULL\n");
		return;
	}

	// Create an empty VAO for the terrain
	// We don't need any vertex buffers since vertices are generated on the GPU
	if(_mesh->meshes[0].vao == 0){
		glGenVertexArrays(1, &_mesh->meshes[0].vao);
	}
	
	// That's it! The shader will generate all vertices using gl_VertexID
	AF_Log("AF_Renderer_InitGPUTerrainMeshBuffer: Created empty VAO for GPU-generated terrain (gridSize=%u)\n", _gridSize);
}




void AF_Renderer_EarlyRendering(AF_RenderingData* _renderingData, Vec4 _backgroundColor)
{
	// Resize the frame buffers
	// if framebuffer sizes have changed, resize them
	if (_renderingData->windowPtr->isWindowResized == AF_TRUE) {
		AF_Renderer_FrameResized(_renderingData);
		_renderingData->windowPtr->isWindowResized = AF_FALSE; // Reset the flag after resizing
	}
	
	// Clear the Debug buffers
	AF_Renderer_BindFrameBuffer(_renderingData->depthDebugFrameBufferData.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glClearColor(_backgroundColor.x, _backgroundColor.y,_backgroundColor.z, 1.0f);
	AF_Renderer_UnBindFrameBuffer();

	AF_Renderer_BindFrameBuffer(_renderingData->screenFrameBufferData.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glClearColor(_backgroundColor.x, _backgroundColor.y,_backgroundColor.z, 1.0f);
	AF_Renderer_UnBindFrameBuffer();
}

/*
====================
AF_Renderer_Render
Simple render command to decide how to progress other rendering steps
====================
*/
void AF_Renderer_Render(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, uint32_t _cameraID){
	// START RENDERING
	AF_Renderer_CheckError( "AF_Renderer_Render: Error at start of Rendering OpenGL setting color and clearing screen! \n");

	// Update Camera UBO
	AF_CCamera *camera = &_ecs->cameras[_cameraID];
	AF_CTransform3D *cameraTransform = &_ecs->transforms[_cameraID];
	AF_Renderer_UpdateCameraUBO(_renderingData->cameraUBO, (AF_FLOAT*)&camera->viewMatrix, (AF_FLOAT*)&camera->projectionMatrix, (AF_FLOAT*)&cameraTransform->pos, 0.0f);

	// Update lighting data
	AF_Lighting_UpdateLighting(_ecs, _lightingData);


	// Switch Between Renderer
	switch(_renderingData->rendererType)
	{
		// FORWARD RENDERING
		case AF_RENDERER_FORWARD:
			AF_Renderer_StartForwardRendering(_ecs, _renderingData, _lightingData, _cameraID);
			AF_Renderer_EndForwardRendering();
		break;

		// DEFERRED RENDERING
		case AF_RENDERER_DEFERRED:
			AF_Log_Warning("AF_Renderer_Render: Deferred Rendering NOT IMPLEMENTED\n");
		break;

		// FORWARD PLUS
		case AF_RENDERER_FORWARD_PLUS:
			AF_Log_Warning("AF_Renderer_Render: Forward+ Rendering NOT IMPLEMENTED\n");
		break;

	}
}


/*
====================
AF_Renderer_StartForwardRendering
Simple render command to perform forward rendering steps
====================
*/
void AF_Renderer_StartForwardRendering(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, uint32_t _cameraID){
    AF_Renderer_CheckError("AF_Renderer_StartForwardRendering: Start Forward rendering\n");
    AF_CCamera* camera = &_ecs->cameras[_cameraID];
    AF_CTransform3D* cameraTransform = &_ecs->transforms[_cameraID];

    AF_Window* window = _renderingData->windowPtr;
    if(window == NULL){
        AF_Log_Error("AF_Renderer_StartForwardRendering: window ptr is null\n");
        return;
    }
	
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // 1. ==== DEPTH PASS (For Shadow Mapping) ====
    AF_Renderer_BindFrameBuffer(_renderingData->depthFrameBufferData.fbo);
    glViewport(0, 0, _renderingData->depthFrameBufferData.textureWidth, _renderingData->depthFrameBufferData.textureHeight);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);

    // For a depth-only FBO, we must tell it not to write to any color buffer.
    // This is required for the FBO to be complete.
    #ifndef AF_WEB_BUILD
        glDrawBuffer(GL_NONE);
    #endif
    // glReadBuffer(GL_NONE) is not available in WebGL 1 and is not needed for a depth-only pass.

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        AF_Log_Error("AF_Renderer_StartForwardRendering: Depth pass framebuffer is not complete!\n");
    } else {
        AF_Entity* depthCameraEntity = &_ecs->entities[_lightingData->ambientLightEntityIndex];
        uint32_t depthCameraID = AF_ECS_GetID(depthCameraEntity->id_tag);
        AF_Renderer_StartDepthPass(_renderingData, _lightingData, _ecs, depthCameraID);
    }
	AF_Renderer_UnBindFrameBuffer();

	// 2. ==== MAIN COLOR & DEBUG PASS ====
    AF_Renderer_BindFrameBuffer(_renderingData->screenFrameBufferData.fbo);
    glViewport(0, 0, window->frameBufferWidth, window->frameBufferHeight);
    
    glDisable(GL_CULL_FACE);
    
    // --- Main Mesh Drawing ---
    AF_Renderer_DrawMeshes(
        &camera->viewMatrix,
        &camera->projectionMatrix,
        _ecs,
        &cameraTransform->pos,
        _lightingData,
        NO_SHARED_SHADER,
        _renderingData
    );
	AF_Renderer_UnBindFrameBuffer();

	// 1.5 Update the render texture cameras
	
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		AF_CCamera* renderTextureCamera = &_ecs->cameras[i];
		if(AF_Component_GetHasEnabled(renderTextureCamera->enabled) == AF_TRUE){
			if(renderTextureCamera->enableRenderToTexture == AF_TRUE){
				AF_Renderer_BindFrameBuffer(renderTextureCamera->renderTextureData.fbo);
				glViewport(0, 0, renderTextureCamera->renderTextureData.textureWidth, renderTextureCamera->renderTextureData.textureHeight);
				// update the forward rendering for this camera
				//renderTextureCamera->cameraFront = AF_Camera_CalculateFront(cameraTransform->pos.y, cameraTransform->pos.x);//renderTextureCamera->yaw, renderTextureCamera->pitch);
				// flip the z axis for the texture camera
				// render texture camera projection matrix is the same as the main camera
				renderTextureCamera->projectionMatrix = _ecs->cameras[_cameraID].projectionMatrix;
				
				/*
				Vec3 targetPos = Vec3_ADD(cameraTransform->pos, renderTextureCamera->cameraFront);
				renderTextureCamera->viewMatrix = Mat4_Lookat(
					cameraTransform->pos,
					targetPos,
					renderTextureCamera->cameraUp
				);*/

				
				// Set the background color for this camera before clearing
				glClearColor(renderTextureCamera->backgroundColor.x, 
				             renderTextureCamera->backgroundColor.y,
				             renderTextureCamera->backgroundColor.z, 
				             renderTextureCamera->backgroundColor.w);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				AF_Renderer_DrawMeshes(
					&renderTextureCamera->viewMatrix,
					&renderTextureCamera->projectionMatrix,
					_ecs,
					&cameraTransform->pos,
					_lightingData,
					NO_SHARED_SHADER,
					_renderingData
				);
				AF_Renderer_UnBindFrameBuffer();
			}
		}
	}
    
    

	

    // --- Debug Collision Hull Drawing (Desktop Only) ---
    #ifndef AF_WEB_BUILD
        // glPolygonMode is not available in WebGL. This block will only compile for desktop.
        GLint previousPolygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, previousPolygonMode);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        AF_Renderer_DrawCollisionMeshes(
            &camera->viewMatrix,
            &camera->projectionMatrix,
            _ecs,
            &cameraTransform->pos,
            _lightingData,
            _renderingData->guizmoDebugShaderID,
            _renderingData
        );

        // Switch back to the previous polygon mode.
        glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode[0]);
    #endif

	// == Draw Text Meshes ==
	// Render text and UI
	//
	// --- 2D Rendering Pass ---
    // Set OpenGL state for 2D rendering once before drawing all 2D elements.
    glDisable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	AF_Renderer_DrawSpriteMeshes(_ecs, _renderingData);
	AF_Renderer_DrawTextMeshes(_ecs, _renderingData);

	// Restore OpenGL state for 3D/UI rendering
    glEnable(GL_DEPTH_TEST);
	

    AF_Renderer_UnBindFrameBuffer();

	

    // 3. ==== VISUALIZE DEPTH TO TEXTURE (Optional Debug View) ====
    AF_Renderer_BindFrameBuffer(_renderingData->depthDebugFrameBufferData.fbo);
    glViewport(0, 0, window->frameBufferWidth, window->frameBufferHeight);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // AF_Renderer_RenderScreenDebugFBOQuad(_renderingData); // This would draw the depth map visualization
    
    // Unbind everything to return to the default state
    AF_Renderer_UnBindFrameBuffer();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	

    AF_Renderer_CheckError("AF_Renderer_StartForwardRendering: Finished Forward rendering\n");
}


/*
====================
AF_Renderer_EndForwardRendering
Simple render command to cleanup forward rendering steps
====================
*/
void AF_Renderer_EndForwardRendering(void){

	

}

/*
====================
AF_Renderer_DrawSpriteMeshes
Render sprite meshes
====================
*/
void AF_Renderer_DrawSpriteMeshes(AF_ECS* _ecs, AF_RenderingData* _renderingData) {
    AF_Renderer_CheckError("AF_Renderer_DrawSpriteMeshes: Start rendering sprite meshes\n");

    // Set OpenGL state for 2D rendering
    glDisable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Create an orthographic projection matrix once for all sprites
    AF_FLOAT screenWidth = (AF_FLOAT)_renderingData->windowPtr->frameBufferWidth;
    AF_FLOAT screenHeight = (AF_FLOAT)_renderingData->windowPtr->frameBufferHeight;
    // Use a top-left origin for 2D rendering
    Mat4 projection = Mat4_Ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
    
    //AF_Log("Sprite rendering - Screen size: %fx%f\n", screenWidth, screenHeight);

    // for each entity
    for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {
        AF_Entity* entity = &_ecs->entities[i];
		AF_CTransform3D* transform = &_ecs->transforms[i];
        if (AF_Component_GetHasEnabled(entity->flags) == AF_FALSE) {
            continue;
        }

        AF_CSprite* spriteComp = &_ecs->sprites[i];
        if (AF_Component_GetHasEnabled(spriteComp->enabled) == AF_FALSE) {
            continue;
        }
        
        //glUseProgram(shaderProgram);
		glUseProgram(spriteComp->spriteMesh.shader.shaderID);

		// send the shader the colour to use
        AF_Shader_SetVec4(spriteComp->spriteMesh.shader.shaderID, "spriteColor", spriteComp->spriteColor[0], spriteComp->spriteColor[1], spriteComp->spriteColor[2], spriteComp->spriteColor[3]);
        // Set screen size uniform
        AF_Shader_SetVec2(spriteComp->spriteMesh.shader.shaderID, "screenSize", screenWidth, screenHeight);

		 // Normalize and set frame uniforms. The shader expects values between 0.0 and 1.0.
        Vec2 normalizedFramePos = {0.0f, 0.0f};
        Vec2 normalizedFrameSize = {1.0f, 1.0f}; // Default to the full texture

        // Prevent division by zero if the sprite sheet size isn't set
        if (spriteComp->spriteSheetSize.x > 0.0f && spriteComp->spriteSheetSize.y > 0.0f) {
            normalizedFramePos.x = spriteComp->spriteFramePos.x / spriteComp->spriteSheetSize.x;
            normalizedFramePos.y = spriteComp->spriteFramePos.y / spriteComp->spriteSheetSize.y;
            normalizedFrameSize.x = spriteComp->spriteFrameSize.x / spriteComp->spriteSheetSize.x;
            normalizedFrameSize.y = spriteComp->spriteFrameSize.y / spriteComp->spriteSheetSize.y;
        }

        AF_Shader_SetVec2(spriteComp->spriteMesh.shader.shaderID, "spriteFramePos", normalizedFramePos.x, normalizedFramePos.y);
        AF_Shader_SetVec2(spriteComp->spriteMesh.shader.shaderID, "spriteFrameSize", normalizedFrameSize.x, normalizedFrameSize.y);
        


		// Tell the shader to use texture unit 0 for the 'text' sampler
        AF_Shader_SetInt(spriteComp->spriteMesh.shader.shaderID, "sprite", 0);
        // Calculate vertex positions based on sprite component data
        float xpos = transform->pos.x;//spriteComp->spritePos.x;
        float ypos = transform->pos.y;//spriteComp->spritePos.y;
        float w = transform->scale.x * spriteComp->spriteSize.x;
        float h = transform->scale.y * spriteComp->spriteSize.y;

        float vertices[6][5] = {
            {xpos,     ypos + h, 0.0f, 0.0f, 1.0f},
            {xpos,     ypos,     0.0f, 0.0f, 0.0f},
            {xpos + w, ypos,     0.0f, 1.0f, 0.0f},

            {xpos,     ypos + h, 0.0f, 0.0f, 1.0f},
            {xpos + w, ypos,     0.0f, 1.0f, 0.0f},
            {xpos + w, ypos + h, 0.0f, 1.0f, 1.0f}
        };

		/*
		
		float xpos = transform.pos.x;
        float ypos = transform.pos.y;
        float w = spriteComp->spriteScale.x;
        float h = spriteComp->spriteScale.y;

        float vertices[6][5] = {
            {xpos,     ypos + h, 0.0f, 0.0f, 1.0f},
            {xpos,     ypos,     0.0f, 0.0f, 0.0f},
            {xpos + w, ypos,     0.0f, 1.0f, 0.0f},

            {xpos,     ypos + h, 0.0f, 0.0f, 1.0f},
            {xpos + w, ypos,     0.0f, 1.0f, 0.0f},
            {xpos + w, ypos + h, 0.0f, 1.0f, 1.0f}
        };
		*/

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, spriteComp->spriteMesh.material.diffuseTexture.id);

        // Render quad
        glBindVertexArray(spriteComp->spriteMesh.meshes[0].vao);
        glBindBuffer(GL_ARRAY_BUFFER, spriteComp->spriteMesh.meshes[0].vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    
    // Unbind the shader
    glUseProgram(0);

    
    //glDeleteProgram(shaderProgram);

    // Restore OpenGL state for 3D rendering
    glEnable(GL_DEPTH_TEST);
	

    AF_Renderer_CheckError("AF_Renderer_DrawSpriteMeshes: Finished rendering sprite meshes\n");
}

/*
====================
AF_Renderer_DrawTextMeshes
Render text meshes
====================
*/
void AF_Renderer_DrawTextMeshes(AF_ECS* _ecs, AF_RenderingData* _renderingData) {
    AF_Renderer_CheckError("AF_Renderer_DrawTextMeshes: Start rendering text meshes\n");
    // Bind the framebuffer 
    //AF_Renderer_BindFrameBuffer(_renderingData->screenFrameBufferData.fbo);

	
    
    // for each entity
    for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {
        AF_Entity* entity = &_ecs->entities[i];
        if (AF_Component_GetHasEnabled(entity->flags) == AF_FALSE) {
            continue;
        }

        AF_CText* textMeshComp = &_ecs->texts[i];
        if (AF_Component_GetHasEnabled(textMeshComp->enabled) == AF_FALSE) {
            continue;
        }

        // ============= Render the text mesh =============
        // use the shader
        AF_Shader_Use(textMeshComp->mesh.shader.shaderID);

        // set the orthographic projection matrix
        AF_FLOAT screenWidth = (AF_FLOAT)_renderingData->windowPtr->frameBufferWidth;
        AF_FLOAT screenHeight = (AF_FLOAT)_renderingData->windowPtr->frameBufferHeight;
        AF_Shader_SetVec2(textMeshComp->mesh.shader.shaderID, "screenSize", screenWidth, screenHeight);

        // send the shader the colour to use
        AF_Shader_SetVec3(textMeshComp->mesh.shader.shaderID, "textColor", textMeshComp->textColor[0], textMeshComp->textColor[1], textMeshComp->textColor[2]);

        // Tell the shader to use texture unit 0 for the 'text' sampler
        AF_Shader_SetInt(textMeshComp->mesh.shader.shaderID, "text", 0);

        // activate the texture
        glActiveTexture(GL_TEXTURE0);

        // bind the VAO
        glBindVertexArray(textMeshComp->mesh.meshes[0].vao);

		
        // 'x' will be our advancing cursor, starting at the component's screen position
        AF_FLOAT x = textMeshComp->screenPos.x;
        AF_FLOAT y = textMeshComp->screenPos.y;

		// Establish a baseline so the text renders correctly.
        // We assume the user provides 'y' as the desired top coordinate.
        // The baseline is then y + the ascender of the font.
        // We'll use the bearing of the first character as an approximation for the ascender.
        AF_Font* font = &textMeshComp->font;
        AF_FLOAT baseline = y + font->characters[(unsigned char)textMeshComp->text[0]].Bearing.y;

        
        // for each character in the text
        for (uint32_t c = 0; c < AF_MAX_PATH_CHAR_SIZE; c++) {
            // break if we reach the null terminator
            if (textMeshComp->text[c] == '\0') {
                break;
            }
            
            AF_Font* chFont = &textMeshComp->font;
            AF_Character ch = chFont->characters[(unsigned char)textMeshComp->text[c]];

			
            // If the character has a texture, render it.
            if (ch.TextureID != 0) {
                // compute the character quad's position and size.
                // compute the character quad's top-left position and size.
                // The y coordinate is the baseline. We subtract the bearingY to find the top of the glyph.
                AF_FLOAT xpos = x + ch.Bearing.x;
                AF_FLOAT ypos = baseline - ch.Bearing.y;
                AF_FLOAT width = ch.Size.x;
                AF_FLOAT height = ch.Size.y;


                // Construct an updated VBO for the character
                AF_FLOAT vertices[6][4] = {
                    { xpos,         ypos + height,   0.0f, 0.0f },            
                    { xpos,         ypos,            0.0f, 1.0f },
                    { xpos + width, ypos,            1.0f, 1.0f },

                    { xpos,         ypos + height,   0.0f, 0.0f },
                    { xpos + width, ypos,            1.0f, 1.0f },
                    { xpos + width, ypos + height,   1.0f, 0.0f }           
                };
                
                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                
                // bind the VBO and update its memory
                glBindBuffer(GL_ARRAY_BUFFER, textMeshComp->mesh.meshes[0].vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                
                // Draw the quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            
            // advance the cursor for the next character
            x += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        
        //unbind the vertex array and texture
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // unbind the shader
    AF_Shader_Use(0);
    // unbind the framebuffer
    //AF_Renderer_UnBindFrameBuffer();

    
    AF_Renderer_CheckError("AF_Renderer_DrawTextMeshes: Finished rendering text meshes\n");
}

// ============================ TEXTURES =================================
// =================================================================================================
// AF_Renderer_SetupTerrainUniforms
// Consolidates all terrain-specific shader uniform setup
// Binds heightmap to texture unit 2 and sets all terrain shader uniforms
// NOTE: Assumes shader is already bound with glUseProgram before calling this
// =================================================================================================
void AF_Renderer_SetupTerrainUniforms(uint32_t _shaderID, AF_CTerrain* _terrain){
	if(_terrain == NULL || _terrain->heightmapTextureID == 0){
		return;
	}
	
	// Bind heightmap texture to unit 2 (0 = diffuse, 1 = shadow map)
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, _terrain->heightmapTextureID);
	
	// Set all terrain uniforms at once
	AF_Shader_SetInt(_shaderID, "heightMap", 2);
	AF_Shader_SetFloat(_shaderID, "heightScale", _terrain->heightScale);
	AF_Shader_SetVec2(_shaderID, "texelSize", _terrain->texelSizeX, _terrain->texelSizeY);
	AF_Shader_SetInt(_shaderID, "gridSize", _terrain->gridSize);
	AF_Shader_SetInt(_shaderID, "gridScale", _terrain->gridScale);
}

// =================================================================================================
// AF_Renderer_BindMeshTextures
// Binds all textures for a mesh (diffuse, shadow map, etc.)
// =================================================================================================
void AF_Renderer_BindMeshTextures(AF_CMesh* _mesh, AF_RenderingData* _renderingData, uint32_t _shader){
	// Diffuse texture (unit 0)
	if(_mesh->material.diffuseTexture.id != 0){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _mesh->material.diffuseTexture.id);
		AF_Shader_SetInt(_shader, "material.diffuse", 0);
	}
	
	// Shadow map (unit 1)
	if(_renderingData->depthFrameBufferData.textureID != 0){
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _renderingData->depthFrameBufferData.textureID);
		AF_Shader_SetInt(_shader, "shadowMap", 1);
	}
}

// =================================================================================================
// AF_Renderer_UnbindTextures
// Unbinds all texture units used by mesh rendering
// =================================================================================================
void AF_Renderer_UnbindTextures(void){
	for(uint32_t i = 0; i < 3; i++){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
}

// =================================================================================================
// AF_Renderer_FindActiveTerrain
// Helper function to find the active terrain component for GPU-generated terrain
// =================================================================================================
AF_CTerrain* AF_Renderer_FindActiveTerrain(AF_ECS* _ecs){
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		AF_CTerrain* terrain = &_ecs->terrains[i];
		if(AF_Component_GetHasEnabled(terrain->enabled) == AF_TRUE){
			return terrain;
		}
	}
	return NULL;
}

// =================================================================================================
// AF_Renderer_ExecuteDrawCall
// Executes the appropriate draw call based on mesh type (instanced vs regular)
// =================================================================================================
void AF_Renderer_ExecuteDrawCall(AF_CMesh* _mesh, AF_ECS* _ecs, uint32_t _shader, uint32_t _indexCount){
	if(_mesh->isInstanced == AF_TRUE){
		// GPU-generated terrain rendering
		AF_CTerrain* terrain = AF_Renderer_FindActiveTerrain(_ecs);
		uint32_t terrainGridSize = terrain ? terrain->gridSize : 65;
		
		// Calculate vertex count: (gridSize-1) * (gridSize-1) quads * 6 vertices per quad
		const uint32_t numQuads = (terrainGridSize - 1) * (terrainGridSize - 1);
		const uint32_t vertexCountToDraw = numQuads * 6;
		
		// Use glDrawArrays because we're generating vertices in the shader
		glDrawArrays(GL_TRIANGLES, 0, vertexCountToDraw);
	}
	else{
		// Regular indexed mesh
		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
	}
}

void AF_Renderer_SetTexture(const uint32_t _shaderID, const char* _shaderVarName, uint32_t _textureID){
	glUseProgram(_shaderID); // Bind the shader program
	glUniform1i(glGetUniformLocation(_shaderID, _shaderVarName), _textureID); // Tell the shader to set the "Diffuse_Texture" variable to use texture id 0
	glUseProgram(0);
}


// ============================  DRAW ================================

/*
====================
AF_Renderer_DrawMeshes
Loop through the entities and draw the meshes that have components attached
====================
*/
void AF_Renderer_DrawMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData){
	
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_Entity* entity = &_ecs->entities[i];
		if(!AF_Component_GetHas(entity->flags)){
			continue;
		}

		AF_CMesh* mesh = &_ecs->meshes[i];
		// Skip if there is no rendering component
		if(!AF_Component_GetHas(mesh->enabled)){ // || hasEnabled == AF_FALSE){
			continue;
		}

		// TODO this is gross, fix it
		// If this entity is a camera, skip rendering its mesh (if any)
		if(mesh->material.diffuseTexture.type == AF_Texture_TypeMappings[AF_TEXTURE_TYPE_RENDER_TEXTURE].type){
			// the camera we use is is stored in a special entity index in the material reserved for render to texture
			if(mesh->material.renderTextureCameraEntityIndex < _ecs->entitiesCount){
				AF_CCamera* camera = &_ecs->cameras[mesh->material.renderTextureCameraEntityIndex];
				if(AF_Component_GetHasEnabled(camera->enabled) == AF_TRUE){
					if(camera->enableRenderToTexture == AF_TRUE){
						// if texture type is renderTexture, make the texture id the same as the screen frame buffer
						mesh->material.diffuseTexture.id = camera->renderTextureData.textureID;
					}
				}
			}
		}

		
		AF_CTransform3D* modelTransform = &_ecs->transforms[i];

		// Make a copy as we will apply some special transformation. e.g. rotation is stored in degrees and needs to be converted to radians
		Vec3 rotationToRadians = {AF_Math_Radians(modelTransform->rot.x),AF_Math_Radians(modelTransform->rot.y), AF_Math_Radians(modelTransform->rot.z)};
		// Update the model matrix
		Mat4 modelMatColumn = Mat4_ToModelMat4(modelTransform->pos, rotationToRadians, modelTransform->scale);
		modelTransform->modelMat = modelMatColumn;

		// Special case for terrain to bind heightmap texture
		AF_CTerrain* terrain = &_ecs->terrains[i];
		if(AF_Component_GetHasEnabled(terrain->enabled) == AF_TRUE){	
			glUseProgram(mesh->material.shaderID);
			AF_Renderer_SetupTerrainUniforms(mesh->material.shaderID, terrain);
		}
			
		AF_Renderer_DrawMesh(&modelTransform->modelMat, _viewMat, _projMat, mesh, _ecs, _cameraPos, _lightingData, _shaderOverride, _renderingData);
	}
	AF_Renderer_CheckError("AF_Renderer_DrawMeshes: Finished drawing all the meshes");
}

/*
====================
AF_Renderer_DrawCollisionMeshes
Loop through the entities and draw the meshes that have components attached
====================
*/
void AF_Renderer_DrawCollisionMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData){
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_Entity* entity = &_ecs->entities[i];
		if(!AF_Component_GetHas(entity->flags)){
			continue;
		}

		AF_CTransform3D* modelTransform = &_ecs->transforms[i];

		AF_CCollider* collider = &_ecs->colliders[i];
		// Skip if there is no rendering component
		if(!AF_Component_GetHas(collider->enabled)){
			continue;
		}

		if(collider->showDebug == AF_FALSE){
			continue;
		}

		// Get the collider mesh
		AF_CMesh colliderMesh = AF_CMesh_ZERO();
		colliderMesh.enabled = AF_Component_SetHas(colliderMesh.enabled, AF_TRUE);
		colliderMesh.enabled = AF_Component_SetEnabled(colliderMesh.enabled, AF_TRUE);
		colliderMesh.meshCount = 1;

		// construct a mesh from the collider
		// need to either send verts direct to the geometry shader or construct a mesh, then send that mesh data.
		// Update the 
		// store the newly created mesh data
		colliderMesh.meshes[0] = collider->collisionMeshData;
		colliderMesh.shader.shaderID = _shaderOverride;
		/**/
		//AF_CMesh* colliderMesh = &_ecs->meshes[i];
		// Skip if there is no rendering component
		if(!AF_Component_GetHas(colliderMesh.enabled)){ // || hasEnabled == AF_FALSE){
			continue;
		}
		
		// construct debug mesh from the collider bounds
		AF_CTransform3D* trans = &_ecs->transforms[i];
		// update the transform based on the position offset

		// Make a copy as we will apply some special transformation. e.g. rotation is stored in degrees and needs to be converted to radians
		Vec3 rotationToRadians = {AF_Math_Radians(trans->rot.x),AF_Math_Radians(trans->rot.y), AF_Math_Radians(trans->rot.z)};
		// Update the model matrix
		// Bounding volume is measured as half extents, so scale by 2
		Mat4 modelMatColumn = Mat4_ToModelMat4(collider->boundingPos, rotationToRadians,  collider->boundingVolume);//_ecs->transforms[i].scale);


		
		AF_Renderer_DrawMesh(&modelMatColumn, _viewMat, _projMat, &colliderMesh, _ecs, _cameraPos, _lightingData, _shaderOverride, _renderingData);
	}
	AF_Renderer_CheckError("AF_Renderer_DrawMeshes: Finished drawing all the meshes");
}

// =================================================================================================
// AF_Renderer_DrawMesh
// Loop through the meshes in a component and draw using opengl
// =================================================================================================
void AF_Renderer_DrawMesh(Mat4* _modelMat, Mat4* _viewMat, Mat4* _projMat, AF_CMesh* _mesh, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData){
	// Validate parameters and early exit conditions
	if(_modelMat == NULL || _viewMat == NULL || _projMat == NULL || _mesh == NULL){
		AF_Log_Error("AF_Renderer_DrawMesh: Passed Null reference \n");
		return;
	}
	
	if(!AF_Component_GetHasEnabled(_mesh->enabled)){
		return;
	}
	
	// Setup shader
	uint32_t shader = (_shaderOverride == NO_SHARED_SHADER) ? _mesh->shader.shaderID : _shaderOverride;
	glUseProgram(shader);

	for(uint32_t i = 0; i < _mesh->meshCount; i++){

		// --- FEEDBACK LOOP DETECTION ---
		// TODO this is likely slow. need a faster solution if we want to support many FBOs and textures
        // Query the currently bound framebuffer's color attachment texture id.
        // If the mesh's diffuse texture is the same texture attached to the FBO we are rendering to,
        // drawing would create a feedback loop. Skip drawing this mesh in that case.
        GLint currentFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
        if (currentFBO != 0) {
            GLint attachmentType = 0;
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &attachmentType);
            if (attachmentType == GL_TEXTURE) {
                GLint attachedTex = 0;
                glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachedTex);
                // Compare attached texture with mesh diffuse texture id (if any)
                if (_mesh->material.diffuseTexture.id != 0 && (GLuint)attachedTex == _mesh->material.diffuseTexture.id) {
                    //AF_Log_Warning("AF_Renderer_DrawMesh: Skipping draw to avoid feedback loop (mesh uses framebuffer's attached texture)\n");
                    continue;
                }
                // Also check shadow/depth texture or other bound textures if you want to be thorough
            }
        }

		// TODO: Render based on shader type 
		// Does the shader use Textures?
		if(_mesh->textured == AF_TRUE){
			if(_mesh->meshes[i].vao == 0) {
				AF_Log_Error("AF_Renderer_DrawMesh: VAO is 0 for mesh %u\n", i);
				continue;
			}

			if(_mesh->meshes[i].indexCount == 0) {
				AF_Log_Error("AF_Renderer_DrawMesh: indexCount is 0 for mesh %u\n", i);
				continue;
			}

			// Check if VAO is valid
			if(!glIsVertexArray(_mesh->meshes[i].vao)) {
				AF_Log_Error("AF_Renderer_DrawMesh: Invalid VAO %u for mesh %u\n", _mesh->meshes[i].vao, i);
				continue;
			}

			// Bind textures if not using shared shader
			if(_shaderOverride == NO_SHARED_SHADER){	
				AF_Renderer_BindMeshTextures(_mesh, _renderingData, shader);
			}
		}

		// Set camera position for lighting calculations
		AF_Shader_SetVec3(shader, "viewPos", _cameraPos->x, _cameraPos->y, _cameraPos->z);

		// Get the next available lights and send data to shader
		if(_mesh->recieveLights == AF_TRUE){
			AF_Lighting_RenderForwardPointLights(shader, _ecs, _lightingData);
		}
		
		// Debug: Check VAO before binding
		if(_mesh->meshes[i].vao == 0){
			AF_Log_Error("AF_Renderer_DrawMesh: Attempting to bind VAO 0 for mesh %u\n", i);
			continue;
		}

		glBindVertexArray(_mesh->meshes[i].vao);//_meshList->vao);
		AF_Renderer_CheckError( "Error bind vao Rendering OpenGL! \n");

		// If you want to explicitly bind the VBO (usually not necessary if VBOs are part of the VAO):
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->meshes[i].vbo);
		AF_Renderer_CheckError("Error binding VBO for drawing!");

		// Send matrices to shader (GL_TRUE = row-major order)
		int projLocation = glGetUniformLocation(shader, "projection");
		glUniformMatrix4fv(projLocation, 1, GL_TRUE, (float*)&_projMat->rows);
		
		int viewLocation = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_TRUE, (float*)&_viewMat->rows);

		int modelLocation = glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(modelLocation, 1, GL_TRUE, (float*)&_modelMat->rows);

		// Prep drawing
		unsigned int indexCount = _mesh->meshes[i].indexCount;
		if(indexCount == 0){
			AF_Log_Warning("AF_Renderer_DrawMesh: indexCount is 0. Can't draw elements\n");
			return;
		}

		// TODO: sort transparent objects before rendering
		//https://learnopengl.com/Advanced-OpenGL/Blending

		// Update the UV coords if animated texture, or scaling
		// Update UVs if needed here
		// update the uv data

		AF_Shader_SetVec2(shader, "uvOffset", _mesh->material.diffuseTexture.uvOffsetX, _mesh->material.diffuseTexture.uvOffsetY);
		AF_Shader_SetVec2(shader, "uvScale", _mesh->material.diffuseTexture.uvScaleX, _mesh->material.diffuseTexture.uvScaleY);
		
		// Execute the appropriate draw call (instanced or regular)
		AF_Renderer_ExecuteDrawCall(_mesh, _ecs, shader, indexCount);
			
		AF_Renderer_CheckError( "AF_Renderer_DrawMesh_Error drawElements Rendering OpenGL! \n");

		glBindVertexArray(0);
		AF_Renderer_CheckError( "Error bindvertexarray(0) Rendering OpenGL! \n");
		
	}
	// Unbind shader
    glUseProgram(0);

    // Unbind all textures
    AF_Renderer_UnbindTextures();
}

/*
====================
AF_Renderer_RenderScreenDebugFBOQuad
Render the quad to the screen and swap the debug frame buffers over.
====================
*/
void AF_Renderer_RenderScreenDebugFBOQuad(AF_RenderingData* _renderingData){
	AF_Renderer_CheckError("AF_Renderer_RenderScreenDebugFBOQuad: Start Render debug quad\n");
	
    glUseProgram(_renderingData->depthDebugFrameBufferData.shaderID);
	
    // Uniforms for linearization (optional, shader dependent)
    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, _renderingData->depthFrameBufferData.textureID); // Bind your actual depth map texture

    if (_renderingData->screenQUAD_VAO == 0) { // Lazy init, good
        AF_Renderer_CreateScreenFBOQuadMeshBuffer(_renderingData);
    }
	
    glBindVertexArray(_renderingData->screenQUAD_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
    glBindVertexArray(0);
    glUseProgram(0);
	
	AF_Renderer_CheckError("AF_Renderer_RenderScreenDebugFBOQuad: Finish Render debug quad\n");
}

/*
====================
AF_Renderer_CreateScreenFBOQuadMeshBuffer
Render the quad to the screen and swap the frame buffers over.
====================
*/
void AF_Renderer_RenderScreenFBOQuad(AF_RenderingData* _renderingData){
	AF_Renderer_CheckError("AF_Renderer_RenderScreenFBOQuad: Start Render debug quad\n");
	AF_Renderer_BindFrameBuffer(0);

    glViewport(0, 0, _renderingData->windowPtr->frameBufferWidth, _renderingData->windowPtr->frameBufferHeight);
    
	
    // Clear the screen and disable depth testing for the final 2D quad draw
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
	

    glUseProgram(_renderingData->screenFrameBufferData.shaderID);
	
    // Uniforms for linearization (optional, shader dependent)
    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, _renderingData->screenFrameBufferData.textureID); // Bind your actual depth map texture

    if (_renderingData->screenQUAD_VAO == 0) { // Lazy init, good
        AF_Renderer_CreateScreenFBOQuadMeshBuffer(_renderingData);
    }
    glBindVertexArray(_renderingData->screenQUAD_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// set background
    glBindVertexArray(0);
    glUseProgram(0);
	
	
	AF_Renderer_CheckError("AF_Renderer_RenderScreenFBOQuad: Finish Render debug quad\n");
}


// ============================  MESH BUFFERS ================================ 

/*
====================
AF_Renderer_InitMeshBuffers
Init the mesh buffers for OpenGL
====================
*/
void AF_Renderer_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount){ 
    if (_entityCount == 0) {
    AF_Log_Error("No meshes to draw!\n");
    	return;
    }

    for(uint32_t i = 0; i < _entityCount; i++){
	   //AF_CMesh* mesh = _entities[i].mesh;

	    af_bool_t hasMesh = AF_Component_GetHas(_mesh->enabled);
	    // Skip setting up if we don't have a mesh component
	    if(hasMesh == AF_FALSE){
			continue;
	    }

		AF_Renderer_CheckError( "Mesh has no indices!\n");

		// for each sub mesh. setup the mesh buffers
		for(uint32_t j = 0; j < _mesh->meshCount; j++){
			if(_mesh->meshes[j].vertexCount < 1){
				// skip creating mesh buffer as we don't have any vetices
				AF_Log_Warning("AF_Renderer_InitMeshBuffers: skip creating mesh buffer as we don't have any vetices\n");
				continue;
			}
			AF_Renderer_CreateMeshBuffer(&_mesh->meshes[j]);
		}
    }
}

void AF_Renderer_InitTextMeshBuffers(AF_CText* _fontComponent){
	if (_fontComponent == NULL) {
        AF_Log_Error("AF_Renderer_InitTextMeshBuffers: _fontComponent is NULL!\n");
        return;
    }
    AF_Renderer_CheckError( "AF_Renderer_InitTextMeshBuffers: before create text mesh buffers\n");
    // setup the font mesh data
    _fontComponent->mesh.meshCount = 1; // only one mesh for font
    _fontComponent->mesh.meshes[0].vertexCount = 6; // A quad is 6 vertices (2 triangles)
    _fontComponent->mesh.meshes[0].indexCount = 0; // no indices for font mesh, using glDrawArrays
    _fontComponent->mesh.meshes[0].vertices = NULL; // Data is dynamic, no static vertex array needed.
    glGenVertexArrays(1, &_fontComponent->mesh.meshes[0].vao);
    glGenBuffers(1, &_fontComponent->mesh.meshes[0].vbo);
    glBindVertexArray(_fontComponent->mesh.meshes[0].vao);
    glBindBuffer(GL_ARRAY_BUFFER, _fontComponent->mesh.meshes[0].vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(AF_FLOAT) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(AF_FLOAT), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    AF_Renderer_CheckError( "AF_Renderer_InitTextMeshBuffers: after create text mesh buffers\n");
}

void AF_Renderer_InitSpriteMeshBuffer(AF_CSprite* _spriteComponent){
	if (_spriteComponent == NULL) {
		AF_Log_Error("AF_Renderer_InitSpriteMeshBuffer: _spriteComponent is NULL!\n");
		return;
	}
	AF_Renderer_CheckError( "AF_Renderer_InitSpriteMeshBuffer: before create sprite mesh buffers\n");
	// A simple quad
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
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	_spriteComponent->spriteMesh.meshCount = 1; // only one mesh for sprite
	_spriteComponent->spriteMesh.meshes[0].vertexCount = 6; // A quad
	_spriteComponent->spriteMesh.meshes[0].indexCount = 0; // no indices for sprite mesh, using glDrawArrays
	_spriteComponent->spriteMesh.meshes[0].vao = VAO;
	_spriteComponent->spriteMesh.meshes[0].vbo = VBO;

	AF_Renderer_CheckError( "AF_Renderer_InitSpriteMeshBuffer: after create sprite mesh buffers\n");
}	

/*
====================
AF_Renderer_CreateMeshBuffer
Do the initial setup for a models mesh buffer
====================
*/
void AF_Renderer_CreateMeshBuffer(AF_MeshData* _meshData){
	if(_meshData == NULL){
		AF_Log_Error("Invalid _meshData, is NULL!\n");
		return;
	}
	
	if (_meshData->vertexCount == 0 || _meshData->indexCount == 0) {
		AF_Log_Error("Invalid vertex or index data!\n");
		return;
	}
		
	//int vertexBufferSize = _entityCount * (mesh->vertexCount * sizeof(AF_Vertex));
	int vertexBufferSize = _meshData->vertexCount * sizeof(AF_Vertex);
	//AF_Log("Init GL Buffers for vertex buffer size of: %i\n",vertexBufferSize);
	AF_Renderer_CheckError( "OpenGL error occurred just before gVAO, gVBO, gEBO buffer creation.\n");
		
	glGenVertexArrays(1, &_meshData->vao);
	glGenBuffers(1, &_meshData->vbo);
	glGenBuffers(1, &_meshData->ibo);
	AF_Renderer_CheckError( "OpenGL error occurred during gVAO, gVBO, gEBO buffer creation.\n");

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s)
	glBindVertexArray(_meshData->vao);
	glBindBuffer(GL_ARRAY_BUFFER, _meshData->vbo);
	AF_Renderer_CheckError( "OpenGL error occurred during binding of the gVAO, gVBO.\n");

	// our buffer needs to be 8 floats (3*pos, 3*normal, 2*tex)
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, _meshData->vertices, GL_STATIC_DRAW);
	AF_Renderer_CheckError( "OpenGL error occurred during glBufferData for the verts.\n");
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the IBO and set the buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshData->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _meshData->indexCount * sizeof(uint32_t), &_meshData->indices[0], GL_STATIC_DRAW);
	AF_Renderer_CheckError( "OpenGL error occurred during glBufferData for the indexes.\n");

	// Stride is 8 floats wide, 3*pos, 3*normal, 2*tex
	// Vertex positions
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, position));
	

	// Vertex normals
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, normal));
	
	// Vertex texture coords
	//glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, texCoord));


	// Vertex tangent attributes
	glEnableVertexAttribArray(3);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(6 * sizeof(float)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, tangent));
	

	// Vertex bi tangent attributes
	glEnableVertexAttribArray(4);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(9 * sizeof(float)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)offsetof(AF_Vertex, bitangent));
	

	
	AF_Renderer_CheckError( "OpenGL error occurred during assignment of vertexAttribs.\n");

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0); 

	_meshData->vao = _meshData->vao;
	_meshData->vbo = _meshData->vbo;
	_meshData->ibo = _meshData->ibo;
	// Bind the IBO and set the buffer data
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, _meshList->meshes->indices, GL_STATIC_DRAW);

	
	AF_Renderer_CheckError("Error InitMesh Buffers for OpenGL! \n");
}

/*
=================================================================================================
AF_Renderer_UpdateMeshBufferData
Updates the vertex data of an existing VBO on the GPU. The buffer must have been created
with GL_DYNAMIC_DRAW for optimal performance.
=================================================================================================
*/
void AF_Renderer_UpdateMeshBufferData(AF_MeshData* _meshData) {
	if(_meshData == NULL || _meshData->vertices == NULL) {
		AF_Log_Error("AF_Renderer_UpdateMeshBufferData: Invalid _meshData or vertices is NULL!\n");
		return;
	}	

	if(_meshData->vbo == 0) {
		AF_Log_Error("AF_Renderer_UpdateMeshBufferData: VBO is 0, cannot update!\n");
		return;
	}

	uint32_t vertextBufferSize = _meshData->vertexCount * sizeof(AF_Vertex);
	glBindBuffer(GL_ARRAY_BUFFER, _meshData->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertextBufferSize, _meshData->vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	
	AF_Renderer_CheckError("AF_Renderer_UpdateMeshBufferData: Error updating mesh buffer data!\n");
}

/*
====================
AF_Renderer_CreateScreenFBOQuadMeshBuffer
Create the screen quad mesh buffers
====================
*/
void AF_Renderer_CreateScreenFBOQuadMeshBuffer(AF_RenderingData* _renderingData){
	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), &QUAD_VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	_renderingData->screenQUAD_VAO = quadVAO;
	_renderingData->screenQUAD_VBO = quadVBO;
}


// ============================  FRAME BUFFERS ================================ 
/*
====================
AF_Renderer_FrameResized
Called by event or callback
Update the Framebuffer as the window size has changed
====================
*/
void AF_Renderer_FrameResized(void* _renderingData){
	if(_renderingData == NULL){
		AF_Log_Error("AF_Renderer_FrameResized: passed null reference\n");
		return;
	}
	AF_RenderingData* renderingDataPtr = (AF_RenderingData*)_renderingData;

	AF_Window* window = renderingDataPtr->windowPtr;
	if(window == NULL){
		AF_Log_Error("AF_Renderer_FrameResized: window is null\n");
		return;
	}
	if ((window->frameBufferWidth <= 0 || window->frameBufferHeight <= 0)){
		AF_Log_Error("AF_Renderer_FrameResized: width: %i height: %i = to or less than 0\n", window->frameBufferWidth, window->frameBufferHeight);
		return;
	}
    // Call the resize function
	// TODO this is a bit messy
	//AF_Renderer_CreateFramebuffer(&renderingDataPtr->screenFBO_ID, &renderingDataPtr->screenRBO_ID, &renderingDataPtr->screenFBO_TextureID, &window->frameBufferWidth, &window->frameBufferHeight, GL_RGB, GL_COLOR_ATTACHMENT0, GL_TRUE, GL_TRUE, GL_LINEAR, GL_LINEAR);
	//update the screen size
	renderingDataPtr->screenFrameBufferData.textureWidth = window->frameBufferWidth;
	renderingDataPtr->screenFrameBufferData.textureHeight = window->frameBufferHeight;
	AF_Renderer_CreateFramebuffer(&renderingDataPtr->screenFrameBufferData);

	// resize the render to texture frame buffer for shadows
	

	renderingDataPtr->depthFrameBufferData.textureWidth = AF_RENDERINGDATA_SHADOW_WIDTH;//window->frameBufferWidth;
	renderingDataPtr->depthFrameBufferData.textureHeight = AF_RENDERINGDATA_SHADOW_HEIGHT;//window->frameBufferHeight;
	//AF_Renderer_CreateFramebuffer(&renderingDataPtr->depthFrameBufferData);
	AF_Renderer_CreateDepthFrameBuffer(&renderingDataPtr->depthFrameBufferData);

	// resize the debug frame buffer
	renderingDataPtr->depthDebugFrameBufferData.textureWidth = window->frameBufferWidth;
	renderingDataPtr->depthDebugFrameBufferData.textureHeight = window->frameBufferHeight;
	AF_Renderer_CreateFramebuffer(&renderingDataPtr->depthDebugFrameBufferData);

}


/*
====================
AF_Renderer_CreateDepthMapFBO
Create frame buffer object
return framebuffer index uint32_t
====================
*/
uint32_t AF_Renderer_CreateFBO(void){
	unsigned int fBO;
	glGenFramebuffers(1, &fBO);
	return fBO;
}

void AF_Renderer_CreateDepthFrameBuffer(AF_FrameBufferData* _frameBufferData) {
    if (_frameBufferData == NULL) {
        AF_Log_Error("AF_Renderer_CreateDepthFrameBuffer: _frameBufferData is NULL.\n");
        return;
    }
    // Delete the existing framebuffer, texture, and renderbuffer if they exist
    AF_Renderer_DeleteFBO(&_frameBufferData->fbo);
    AF_Renderer_DeleteTexture(&_frameBufferData->textureID);

    // Generate the framebuffer id
    _frameBufferData->fbo = AF_Renderer_CreateFBO();
    AF_Renderer_BindFrameBuffer(_frameBufferData->fbo);

    // Create the texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    
    #ifdef AF_WEB_BUILD
        AF_Log("AF_Renderer_CreateDepthFrameBuffer: WEB\n");
        // WebGL depth texture requirements. GL_DEPTH_COMPONENT16 is a good default for WebGL 2.
        // For WebGL 1, you might need to check for the WEBGL_depth_texture extension.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
            _frameBufferData->textureWidth, _frameBufferData->textureHeight, 0, 
            GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
        
        // WebGL-compatible texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #else
        AF_Log("AF_Renderer_CreateDepthFrameBuffer: Desktop\n");
        // Desktop OpenGL depth texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
            _frameBufferData->textureWidth, _frameBufferData->textureHeight, 0, 
            GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        
        // Desktop OpenGL texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    #endif

    // Attach depth texture as FBO's depth buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    
    // On desktop, you must explicitly tell OpenGL not to draw to any color buffer.
    // On WebGL, this is implicit if no color attachment is present.
    #ifndef AF_WEB_BUILD
        glDrawBuffer(GL_NONE);
    #endif

    // FIX: Remove glReadBuffer. It is not available in WebGL 1 and is not needed.
    // glReadBuffer(GL_NONE);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        AF_Log_Error("AF_Renderer_CreateDepthFrameBuffer: Framebuffer not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _frameBufferData->textureID = depthMap;

	AF_Renderer_CheckError("AF_Renderer_CreateDepthFrameBuffer: Finished creating depth framebuffer\n");
}

/*
====================
AF_Renderer_CreateFramebuffer
Create FBO, RBO, and Texture to use in frame buffer rendering for color
====================
*/
void AF_Renderer_CreateFramebuffer(AF_FrameBufferData* _frameBufferData)
{
    if (_frameBufferData == NULL) {
        AF_Log_Error("AF_Renderer_CreateFramebuffer: _frameBufferData is NULL.\n");
        return;
    }

    // Delete the existing framebuffer, texture, and renderbuffer if they exist
    AF_Renderer_DeleteFBO(&_frameBufferData->fbo);
    AF_Renderer_DeleteRBO(&_frameBufferData->rbo);
    AF_Renderer_DeleteTexture(&_frameBufferData->textureID);
    
    // 1. Generate and bind the framebuffer
    _frameBufferData->fbo = AF_Renderer_CreateFBO();
    AF_Renderer_BindFrameBuffer(_frameBufferData->fbo);

    // 2. Generate and attach the color texture
    _frameBufferData->textureID = AF_Renderer_CreateFBOTexture(_frameBufferData);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _frameBufferData->textureID, 0);

    // 3. Generate and attach the depth/stencil renderbuffer
    _frameBufferData->rbo = AF_Renderer_CreateRBO();
    glBindRenderbuffer(GL_RENDERBUFFER, _frameBufferData->rbo);
    
    // Use GL_DEPTH_STENCIL for WebGL 1 compatibility, which is a safe default.
    // WebGL 2 and Desktop GL also support GL_DEPTH24_STENCIL8.
    #ifdef AF_WEB_BUILD
		AF_Log("AF_Renderer_CreateFramebuffer: WEB glRenderbufferStorage GL_DEPTH_STENCIL\n");
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, _frameBufferData->textureWidth, _frameBufferData->textureHeight);
    #else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _frameBufferData->textureWidth, _frameBufferData->textureHeight);
    #endif
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _frameBufferData->rbo);

    // 4. Check for completeness
    AF_Renderer_CheckFrameBufferStatus("AF_Renderer_CreateFramebuffer");
    
    // 5. Unbind the framebuffer to return to the default state
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
====================
AF_Renderer_CreateRBO
Create render buffer object
return renderbuffer index uint32_t
====================
*/
uint32_t AF_Renderer_CreateRBO(void)
{
    unsigned int rBO;
	glGenRenderbuffers(1, &rBO);
	return rBO;
}



/*
====================
AF_Renderer_BindDepthFrameBuffer
Bind the depth FBO to the framebuffer command on the gpu
====================
*/
void AF_Renderer_BindFrameBuffer(uint32_t _fBOID){
	glBindFramebuffer(GL_FRAMEBUFFER, _fBOID);
}

/*
====================
AF_Renderer_UnBindFrameBuffer
UnBind the depth FBO to the framebuffer command on the gpu
====================
*/
void AF_Renderer_UnBindFrameBuffer(void){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
====================
AF_Renderer_BindFrameBufferToTexture
Bind the depth FBO and Texture to the framebuffer command on the gpu
====================
*/
void AF_Renderer_BindFrameBufferToTexture(uint32_t _fBOID, uint32_t _textureID, uint32_t _textureAttatchmentType){
	glBindFramebuffer(GL_FRAMEBUFFER, _fBOID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, _textureAttatchmentType, GL_TEXTURE_2D, _textureID, 0);
}


/*
====================
AF_Renderer_BindRenderBuffer
bind the render buffer to the frame buffer
====================
*/
void AF_Renderer_BindRenderBuffer(uint32_t _rbo, uint32_t _screenWidth, uint32_t _screenHeight){
	// MUST bind the RBO first!
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
	// Now allocate storage for the currently bound RBO
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _screenWidth, _screenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    // Attach the RBO to the currently bound FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo); // now actually attach it
	// Unbind RBO (optional but good practice)
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


/*
====================
AF_Renderer_CreateDepthMapTexture
Create Depth map texture and return the texture id
====================
*/
// Modified Texture Creation Function
uint32_t AF_Renderer_CreateFBOTexture(AF_FrameBufferData* _frameBufferData) {
    unsigned int fboTextureID = 0;
    glGenTextures(1, &fboTextureID);
    glBindTexture(GL_TEXTURE_2D, fboTextureID);

    GLenum internalFormat;
    GLenum format;
    GLenum type;

    // Determine the correct formats and type based on the requested internal format
    if ((GLenum)_frameBufferData->internalFormat == GL_DEPTH_COMPONENT) {
        format = GL_DEPTH_COMPONENT;
        #ifdef AF_WEB_BUILD
            // WebGL 1 requires unsized internal format and a specific type for depth.
            // GL_DEPTH_COMPONENT16 is available in WebGL 2 for better precision.
            internalFormat = GL_DEPTH_COMPONENT;
            type = GL_UNSIGNED_SHORT;
        #else
            // Desktop can use a more precise sized format.
            internalFormat = GL_DEPTH_COMPONENT24;
            type = GL_FLOAT;
        #endif
    } else if ((GLenum)_frameBufferData->internalFormat == GL_RGBA || (GLenum)_frameBufferData->internalFormat == GL_RGBA16F || (GLenum)_frameBufferData->internalFormat == GL_SRGB8_ALPHA8) {
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE; // Standard for 8-bit per channel color
        #ifdef AF_WEB_BUILD
            // WebGL 1 requires the internal format to match the base format.
            // WebGL 2 supports sized formats like GL_RGBA16F.
            // WebGL 2 supports sized sRGB formats.
            // If the request is for sRGB, use it. Otherwise, default to linear RGBA.
            if ((GLenum)_frameBufferData->internalFormat == GL_SRGB8_ALPHA8) {
                internalFormat = GL_SRGB8_ALPHA8;
            } else {
                internalFormat = GL_RGBA;
            }
        #else
            internalFormat = _frameBufferData->internalFormat; // Use GL_RGBA, GL_RGBA16F, or GL_SRGB8_ALPHA8 on desktop
        #endif
    } else { // Default to RGB
        format = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        #ifdef AF_WEB_BUILD
            internalFormat = GL_RGB;
        #else
            internalFormat = GL_RGB8;
        #endif
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
        _frameBufferData->textureWidth, _frameBufferData->textureHeight, 0,
        format, type, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)_frameBufferData->minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)_frameBufferData->magFilter);

    // Set texture wrapping parameters
    #ifdef AF_WEB_BUILD
        // WebGL requires GL_CLAMP_TO_EDGE for non-power-of-two textures and for depth textures.
        AF_Log("AF_Renderer_CreateFBOTexture: WEB GL_CLAMP_TO_EDGE\n");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #else
        // Desktop can use GL_CLAMP_TO_BORDER for depth maps to avoid sampling outside the map.
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

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
    return fboTextureID;
}


// ============================  DEPTH ================================ 
/*
====================
AF_Render_StartDepthPath
Do the initial setup for rendering a depth pass in opengl
====================
*/
void AF_Renderer_StartDepthPass(AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_ECS* _ecs, uint32_t _cameraID){
	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	// if ambientLightEntityIndex is not set, then we can't render the depth pass
	if(_lightingData->ambientLightEntityIndex <= 0){
		//AF_Log_Error("AF_Renderer_StartDepthPass: ambientLightEntityIndex is not set, can't render depth pass\n");
		return;
	}
	
	AF_CCamera* depthCamera = &_ecs->cameras[_cameraID];//AF_CCamera_ZERO();
	AF_ShadowData* shadowData = &_lightingData->shadowData;
	shadowData->shadowCameraID = _cameraID;
	depthCamera->orthographic = AF_TRUE; // Set to orthographic for depth pass

	AF_CTransform3D* depthCamTransform = &_ecs->transforms[_lightingData->ambientLightEntityIndex];//&_ecs->transforms[_cameraID];

	if(depthCamera->orthographic){
		//depthCamera->projectionMatrix = Mat4_Ortho(-outerBounds, outerBounds, -outerBounds, outerBounds, depthCamera->nearPlane, depthCamera->farPlane);
		depthCamera->projectionMatrix = Mat4_Ortho(-shadowData->outerBounds, shadowData->outerBounds, -shadowData->outerBounds, shadowData->outerBounds, shadowData->nearPlane, shadowData->farPlane);
	}else{
		//depthCamera->projectionMatrix = AF_Camera_GetPerspectiveProjectionMatrix(depthCamera, _renderingData->depthFrameBufferData.textureWidth,  _renderingData->depthFrameBufferData.textureHeight);
	}
	// flip the y axis
		// For a row-major matrix, you negate the middle element of the second row.
	depthCamera->projectionMatrix.rows[1].y *= -1.0f;
	
	//AF_Log("=========shadowLightProjection========\n");
	_lightingData->shadowData.lightPos = depthCamTransform->pos;
	Vec3 worldUp = {0.0f, 1.0f, 0.0f}; // Assuming Y is up in your world
	
    // calculate up
	depthCamera->cameraUp = worldUp;
	Mat4 viewMatrix = Mat4_Lookat(_lightingData->shadowData.lightPos, _lightingData->shadowData.lightTarget, _lightingData->shadowData.worldUp);//, , );
	depthCamera->viewMatrix = viewMatrix;
	


	// copy the matrix to the lighting data
	_lightingData->shadowData.shadowLightSpaceMatrix = Mat4_MULT_M4(depthCamera->projectionMatrix, depthCamera->viewMatrix);
	// Transpose it
	_lightingData->shadowData.shadowLightSpaceMatrix = Mat4_Transpose(&_lightingData->shadowData.shadowLightSpaceMatrix);
	// flip the y
	//_lightingData->shadowLightSpaceMatrix.rows[1].y *= -1.0f;


	//AF_Util_Mat4_Log(_lightingData->shadowData.shadowLightSpaceMatrix);
	//AF_Log("=========shadowLightSpaceMatrix========\n");
	//AF_Util_Mat4_Log(shadowLightSpaceMatrix);
	// render scene from light's point of view
	
	
	//glDepthFunc(GL_LESS);
    // AF_Renderer_DrawMeshes renders all visible entities using their respective materials,
    // shaders, lighting, and applies shadows using depthMapTextureID and lightSpaceMatrix.
    AF_Renderer_DrawMeshes(
        &depthCamera->viewMatrix,//camera->viewMatrix,
        &depthCamera->projectionMatrix,//camera->projectionMatrix,
        _ecs,
        &depthCamTransform->pos,//_cameraEntity->transform->pos, // Camera position for lighting calculations
        _lightingData,
		_renderingData->depthFrameBufferData.shaderID, //NO_SHARED_SHADER, //
		_renderingData
    );

}

// ============================  DESTROY / CLEANUP ================================ 

/*
====================
AF_Renderer_DestroyRenderer
Destroy the renderer
====================
*/
void AF_Renderer_DestroyRenderer(AF_RenderingData* _renderingData, AF_ECS* _ecs){
    AF_Log("%s Destroyed\n", openglRendererFileTitle);
	if(_ecs == NULL){
		AF_Log_Error("AF_Renderer_DestroyRenderer: ECS is NULL\n");
		return;
	}
	// Destroy the meshes
    for(uint32_t i  = 0; i < _ecs->entitiesCount; i++){
		AF_CMesh* meshComponent = &_ecs->meshes[i];
		if(meshComponent == NULL){
			AF_Log_Error("AF_Renderer_DestroyRenderer: MeshComponent is NULL\n");
			continue;
		}
		af_bool_t hasMesh = AF_Component_GetHas(meshComponent->enabled);
		af_bool_t hasEnabled = AF_Component_GetEnabled(meshComponent->enabled);
		// Skip if there is no rendering component
		if(hasMesh == AF_FALSE || hasEnabled == AF_FALSE){
			continue;
		}
		// Destroy the mesh buffers
		
		// Destory the material textures
		AF_Renderer_Destroy_Material_Textures(&meshComponent->material);
		
		// Destroy mesh shader
		AF_Shader_Delete(meshComponent->shader.shaderID);

		// Destroy the mesh buffers
		AF_Renderer_DestroyMeshBuffers(meshComponent);

		

		// zero the component
		*meshComponent = AF_CMesh_ZERO();
    }

	// Delete Frame buffer stuff
	// Destroy renderbuffers
	// Screen buffers

	glDeleteFramebuffers(1, &_renderingData->screenFrameBufferData.fbo);
	glDeleteRenderbuffers(1, &_renderingData->screenFrameBufferData.rbo);

	// Depth Buffers
	glDeleteFramebuffers(1, &_renderingData->depthDebugFrameBufferData.fbo);
	glDeleteFramebuffers(1, &_renderingData->depthDebugFrameBufferData.rbo);

	// Depth Buffers
	glDeleteFramebuffers(1, &_renderingData->depthFrameBufferData.fbo);
	glDeleteFramebuffers(1, &_renderingData->depthFrameBufferData.rbo);

	// Screen Quad
	glDeleteVertexArrays(1, &_renderingData->screenQUAD_VAO);
	glDeleteVertexArrays(1, &_renderingData->screenQUAD_VBO);
	// Delete textures

	// Delete Shaders
       
        //glDeleteTexture(_meshList->materials[0].textureID);
    AF_Renderer_CheckError( "Error Destroying Renderer OpenGL! \n");
}



/*
====================
AF_Renderer_Destroy_Material_Textures
Destroy the material textures
====================
*/
void AF_Renderer_Destroy_Material_Textures(AF_Material* _material){
	// Diffuse
	if(_material->diffuseTexture.type != AF_TEXTURE_TYPE_NONE){
		AF_Renderer_DeleteTexture(&_material->diffuseTexture.id);
	}

	// Specular
	if(_material->specularTexture.type != AF_TEXTURE_TYPE_NONE){
		AF_Renderer_DeleteTexture(&_material->specularTexture.id);
	}

	// Normal
	if(_material->normalTexture.type != AF_TEXTURE_TYPE_NONE){
		AF_Renderer_DeleteTexture(&_material->normalTexture.id);
	}
}

/*
====================
AF_Renderer_DestroyMeshComponent
Destroy the mesh renderer component renderer data
====================
*/
void AF_Renderer_DestroyMeshBuffers(AF_CMesh* _mesh){
		// for each mesh
		for(uint32_t j = 0; j < _mesh->meshCount; j++){
			// ------------------------------------------------------------------------
			AF_MeshData* mesh = &_mesh->meshes[j];
			if(mesh == NULL){
				AF_Log_Warning("AF_Renderer_DestroyMeshBuffers: skipping destroy of mesh %i\n", j);
				continue;
			}

		
			glDeleteVertexArrays(1, &mesh->vao); // ✅ Correct: Delete VAO
			glDeleteBuffers(1, &mesh->vbo);
			glDeleteBuffers(1, &mesh->ibo);

			// Now that the mesh is loaded, we can delete the memory created for the verts and indices
			if(mesh->vertices != NULL){
				free(mesh->vertices);
			}
			mesh->vertices = NULL;

			if(mesh->indices != NULL){
				free(mesh->indices);
			}
			mesh->indices = NULL;
		}
}

/*====================
AF_Renderer_DeleteFBO
Delete a frame buffer to render to
====================*/
void AF_Renderer_DeleteFBO(uint32_t* _fboID)
{
	glDeleteFramebuffers(1, _fboID);
	*_fboID = 0;
}


/*====================
AF_Renderer_DeleteRBO
Delete a render buffer object
====================*/
void AF_Renderer_DeleteRBO(uint32_t* _rboID)
{
	glDeleteRenderbuffers(1, _rboID);
	*_rboID = 0;
}

/*====================
AF_Renderer_DeleteTexture
Delete a texture buffer object
====================*/
void AF_Renderer_DeleteTexture(uint32_t* _textureID)
{
	glDeleteTextures(1, _textureID);
	*_textureID = 0;
}


// ====================================== HELPER FUNCTIONS =====================================


/*
====================
AF_Renderer_CheckFrameBufferStatus
Helper function for checking for GL errors for frame buffers
====================
*/
void AF_Renderer_CheckFrameBufferStatus(const char* _message){
	// In AF_Renderer_Start_ScreenFrameBuffers, replace the check with this:
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		// Log the specific error code!
		const char* statusStr = "";
		switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED:                     statusStr = "GL_FRAMEBUFFER_UNDEFINED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         statusStr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        statusStr = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        statusStr = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED:                   statusStr = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
			// case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:   statusStr = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break; // If using newer GL
			default:                                           statusStr = "Unknown Error"; break;
		}
		// Make sure the error message accurately reflects where it's coming from
		AF_Log_Error("AF_Renderer_CheckFrameBufferStatus: ERROR::FRAMEBUFFER:: Framebuffer is not complete! Status: 0x%x (%s): %s\n", status, statusStr, _message);
	}
}


void AF_Renderer_SetPolygonMode(AF_Renderer_PolygonMode_e _polygonMode){
	switch(_polygonMode){
		case AF_RENDERER_POLYGON_MODE_FILL:
			#ifndef AF_WEB_BUILD
				//AF_Log_Warning("AF_Renderer_SetPolygonMode: glPolygonMode not implemented\n");
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			#endif
		break;
		case AF_RENDERER_POLYGON_MODE_POINT:
			#ifndef AF_WEB_BUILD
				//AF_Log_Warning("AF_Renderer_SetPolygonMode: glPolygonMode not implemented\n");
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			#endif
		break;
		case AF_RENDERER_POLYGON_MODE_LINE:
			#ifndef AF_WEB_BUILD
				//AF_Log_Warning("AF_Renderer_SetPolygonMode: glPolygonMode not implemented\n");
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			#endif
		break;

	}
}


void AF_Renderer_DrawTestTriangle(void) {
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}


// ============================
// AF_Renderer_UpdateCameraUBO(uint32_t uboID, AF_FLOAT* viewMatrix, AF_FLOAT* projMatrix, AF_FLOAT* camPos, AF_FLOAT currentTime);
// Updates the camera UBO with the provided view and projection matrices, camera position, and current time.
// ============================
void AF_Renderer_UpdateCameraUBO(uint32_t uboID, AF_FLOAT* viewMatrix, AF_FLOAT* projMatrix, AF_FLOAT* camPos, AF_FLOAT currentTime) {
	AF_CameraUBO_s uboData;

	// Copy view matrix
	memcpy(uboData.view, viewMatrix, sizeof(AF_FLOAT) * 16);
	memcpy(uboData.projection, projMatrix, sizeof(AF_FLOAT) * 16);

	// Copy vector and scalr cam position
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

uint32_t AF_Renderer_CreateCameraUBO(void){
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


