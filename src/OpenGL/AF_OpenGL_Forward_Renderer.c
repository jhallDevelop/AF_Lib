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
#include "AF_RendererFramebuffer.h"
#include "AF_RendererBuffer.h"

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
					AF_RendererFramebuffer_CreateFramebuffer(&cameraComponent->renderTextureData);
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
	AF_RendererBuffer_CreateScreenFBOQuadMeshBuffer(_renderingData);


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
	_renderingData->cameraUBO = AF_RendererBuffer_CreateCameraUBO();

	return AF_TRUE;
	
}

// ============================  BUFFER MANAGEMENT ================================
void AF_Renderer_InitCollisionGeomtery(AF_ECS* _ecs){
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		AF_Entity* entity = &_ecs->entities[i];
		AF_CCollider* collider = &_ecs->colliders[i];
		if(AF_Component_GetHasEnabled(collider->enabled) == AF_TRUE){
			AF_RendererBuffer_CreateCollisionGeometryMeshBuffer(collider);
		}
	}
}


// ============================  INITIALIZATION & SETUP ================================

void AF_Renderer_EarlyRendering(AF_RenderingData* _renderingData, Vec4 _backgroundColor)
{
	// Resize the frame buffers
	// if framebuffer sizes have changed, resize them
	if (_renderingData->windowPtr->isWindowResized == AF_TRUE) {
		AF_Renderer_FrameResized(_renderingData);
		_renderingData->windowPtr->isWindowResized = AF_FALSE; // Reset the flag after resizing
	}
	
	// Clear the Debug buffers
	AF_RendererFramebuffer_BindFrameBuffer(_renderingData->depthDebugFrameBufferData.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glClearColor(_backgroundColor.x, _backgroundColor.y,_backgroundColor.z, 1.0f);
	AF_RendererFramebuffer_UnBindFrameBuffer();

	AF_RendererFramebuffer_BindFrameBuffer(_renderingData->screenFrameBufferData.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glClearColor(_backgroundColor.x, _backgroundColor.y,_backgroundColor.z, 1.0f);
	AF_RendererFramebuffer_UnBindFrameBuffer();
}

// ============================  MAIN RENDERING PASSES ================================

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
	AF_RendererBuffer_UpdateCameraUBO(_renderingData->cameraUBO, (AF_FLOAT*)&camera->viewMatrix, (AF_FLOAT*)&camera->projectionMatrix, (AF_FLOAT*)&cameraTransform->pos, 0.0f);

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
    AF_RendererFramebuffer_BindFrameBuffer(_renderingData->depthFrameBufferData.fbo);
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
	AF_RendererFramebuffer_UnBindFrameBuffer();

	// 2. ==== MAIN COLOR & DEBUG PASS ====
    AF_RendererFramebuffer_BindFrameBuffer(_renderingData->screenFrameBufferData.fbo);
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
	AF_RendererFramebuffer_UnBindFrameBuffer();

	// 1.5 Update the render texture cameras
	
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		AF_CCamera* renderTextureCamera = &_ecs->cameras[i];
		if(AF_Component_GetHasEnabled(renderTextureCamera->enabled) == AF_TRUE){
			if(renderTextureCamera->enableRenderToTexture == AF_TRUE){
				AF_RendererFramebuffer_BindFrameBuffer(renderTextureCamera->renderTextureData.fbo);
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
				AF_RendererFramebuffer_UnBindFrameBuffer();
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
	

    AF_RendererFramebuffer_UnBindFrameBuffer();

	

    // 3. ==== VISUALIZE DEPTH TO TEXTURE (Optional Debug View) ====
    AF_RendererFramebuffer_BindFrameBuffer(_renderingData->depthDebugFrameBufferData.fbo);
    glViewport(0, 0, window->frameBufferWidth, window->frameBufferHeight);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // AF_Renderer_RenderScreenDebugFBOQuad(_renderingData); // This would draw the depth map visualization
    
    // Unbind everything to return to the default state
    AF_RendererFramebuffer_UnBindFrameBuffer();
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
        AF_RendererBuffer_UpdateAndDrawSpriteBuffer(spriteComp, vertices);
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
                
                // Update VBO and draw the quad
                AF_RendererBuffer_UpdateAndDrawTextBuffer(textMeshComp, vertices);
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
	AF_Shader_SetFloat(_shaderID, "gridScale", _terrain->gridScale);
	AF_Shader_SetInt(_shaderID, "lodSkipInterval", _terrain->lodLevel);
	// Set LOD uniform (1 = full detail, no culling)


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
	// TODO: chunk up terrain mesh and render/cull chunks based on camera position for large terrains
	if(_mesh->isInstanced == AF_TRUE){
		// GPU-generated terrain rendering
		/*
		AF_CTerrain* terrain = AF_Renderer_FindActiveTerrain(_ecs);
		uint32_t terrainGridSize = terrain ? terrain->gridSize : 65;
		uint32_t lodLevel = terrain ? terrain->lodLevel : 0;
		
		// Calculate LOD skip: 2^lodLevel (1, 2, 4, 8, 16...)
		uint32_t lodSkip = 1 << lodLevel;
		
		// Calculate LOD-reduced grid size
		uint32_t lodGridSize = (terrainGridSize + lodSkip - 1) / lodSkip;
		
		// Calculate vertex count for LOD grid: (lodGridSize-1)^2 quads * 6 verts per quad
		const uint32_t numQuadsPerRow = lodGridSize - 1;
		const uint32_t numQuads = numQuadsPerRow * numQuadsPerRow;
		const uint32_t vertexCountToDraw = numQuads * 6;

		// Use glDrawArrays because we're generating vertices in the shader
		glDrawArrays(GL_TRIANGLES, 0, vertexCountToDraw);
		*/
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


// ============================  MESH DRAWING SYSTEMS ================================
// These functions iterate through ECS entities and draw different mesh types
// Tightly coupled with OpenGL - keep here for now

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
			
			AF_Renderer_DrawTerrain(i, terrain, &modelTransform->modelMat, _viewMat, _projMat, mesh, _ecs, _cameraPos, _lightingData, _shaderOverride, _renderingData);
		}else{
			AF_Renderer_DrawMesh(&modelTransform->modelMat, _viewMat, _projMat, mesh, _ecs, _cameraPos, _lightingData, _shaderOverride, _renderingData);
		}
			
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
        AF_RendererBuffer_CreateScreenFBOQuadMeshBuffer(_renderingData);
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
	AF_RendererFramebuffer_BindFrameBuffer(0);

    glViewport(0, 0, _renderingData->windowPtr->frameBufferWidth, _renderingData->windowPtr->frameBufferHeight);
    
	
    // Clear the screen and disable depth testing for the final 2D quad draw
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
	

    glUseProgram(_renderingData->screenFrameBufferData.shaderID);
	
    // Uniforms for linearization (optional, shader dependent)
    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, _renderingData->screenFrameBufferData.textureID); // Bind your actual depth map texture

    if (_renderingData->screenQUAD_VAO == 0) { // Lazy init, good
        AF_RendererBuffer_CreateScreenFBOQuadMeshBuffer(_renderingData);
    }
    glBindVertexArray(_renderingData->screenQUAD_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// set background
    glBindVertexArray(0);
    glUseProgram(0);
	
	
	AF_Renderer_CheckError("AF_Renderer_RenderScreenFBOQuad: Finish Render debug quad\n");
}


// ============================  BUFFER MANAGEMENT ================================
// OpenGL VAO/VBO/EBO creation and initialization for meshes, text, sprites

void AF_Renderer_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount){ 
	AF_RendererBuffer_InitMeshBuffers(_mesh, _entityCount);
}


// ============================  FRAMEBUFFER OPERATIONS ================================ 
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
	AF_RendererFramebuffer_CreateFramebuffer(&renderingDataPtr->screenFrameBufferData);

	// resize the render to texture frame buffer for shadows
	

	renderingDataPtr->depthFrameBufferData.textureWidth = AF_RENDERINGDATA_SHADOW_WIDTH;//window->frameBufferWidth;
	renderingDataPtr->depthFrameBufferData.textureHeight = AF_RENDERINGDATA_SHADOW_HEIGHT;//window->frameBufferHeight;
	//AF_RendererFramebuffer_CreateFramebuffer(&renderingDataPtr->depthFrameBufferData);
	AF_RendererFramebuffer_CreateDepthFrameBuffer(&renderingDataPtr->depthFrameBufferData);

	// resize the debug frame buffer
	renderingDataPtr->depthDebugFrameBufferData.textureWidth = window->frameBufferWidth;
	renderingDataPtr->depthDebugFrameBufferData.textureHeight = window->frameBufferHeight;
	AF_RendererFramebuffer_CreateFramebuffer(&renderingDataPtr->depthDebugFrameBufferData);

}


/*
====================
AF_Renderer_CreateDepthMapFBO
Create frame buffer object
return framebuffer index uint32_t
====================
*/

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
		AF_RendererBuffer_DestroyMeshBuffers(meshComponent);

		

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
	AF_RendererBuffer_DeleteScreenQuadBuffers(_renderingData);
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
		AF_RendererFramebuffer_DeleteTexture(&_material->diffuseTexture.id);
	}

	// Specular
	if(_material->specularTexture.type != AF_TEXTURE_TYPE_NONE){
		AF_RendererFramebuffer_DeleteTexture(&_material->specularTexture.id);
	}

	// Normal
	if(_material->normalTexture.type != AF_TEXTURE_TYPE_NONE){
		AF_RendererFramebuffer_DeleteTexture(&_material->normalTexture.id);
	}
}


// ============================  RESOURCE CLEANUP & DESTRUCTION ================================

/*====================
AF_Renderer_DeleteFBO
Delete a frame buffer to render to
====================*/


// ====================================== HELPER FUNCTIONS =====================================


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

// =================================================================================================
// AF_Terrain_CalculateLODInterval
// Calculates LOD skip interval based on camera distance
// Returns 1 (full detail), 2 (half), 4 (quarter), 8 (eighth), etc.
// =================================================================================================
int32_t AF_Renderer_CalculateLODInterval(Vec3 cameraPos, Vec3 terrainCenter, AF_FLOAT patchSize, uint32_t gridSize) {
    // Calculate distance
    float dx = cameraPos.x - terrainCenter.x;
    float dy = cameraPos.y - terrainCenter.y;
    float dz = cameraPos.z - terrainCenter.z;
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);
	
	// Convert distance to LOD level directly
	// Base LOD on chunk size (patchSize)
	// Switch LOD every 2 chunks (256m if chunk is 128m) - Harsher drop-off
	int32_t lodLevel = (int32_t)(distance / (patchSize * 2.0f)); 
	if (lodLevel < 0) lodLevel = 0;

    // Clamp LOD level based on gridSize to prevent quadsPerRow < 1
    // e.g. if gridSize is 17, max lodSkip is 16 (LOD 4)
    uint32_t maxLOD = 0;
    while (((gridSize - 1) >> (maxLOD + 1)) > 0) {
        maxLOD++;
    }
	if (lodLevel > (int32_t)maxLOD) lodLevel = (int32_t)maxLOD;
	
	return lodLevel;
}

// =================================================================================================
// AF_Renderer_DrawTerrain
// Draws a terrain mesh with LOD based on camera distance
// =================================================================================================
void AF_Renderer_DrawTerrain(uint32_t _terrainID, AF_CTerrain* _terrain, Mat4* _modelMat, Mat4* _viewMat, Mat4* _projMat, AF_CMesh* _mesh, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData, uint32_t _shaderOverride, AF_RenderingData* _renderingData){
	// Validate parameters and early exit conditions
    if(_terrain == NULL || _modelMat == NULL || _viewMat == NULL || _projMat == NULL || _mesh == NULL){
        AF_Log_Error("AF_Renderer_DrawTerrain: Passed Null reference \n");
        return;
    }
    
    // Determine which shader to use (prioritize override, then material, then mesh)
    uint32_t shaderID = (_shaderOverride == NO_SHARED_SHADER) ? _mesh->material.shaderID : _shaderOverride;
    if (shaderID == 0) {
        shaderID = _mesh->shader.shaderID;
    }
	
    glUseProgram(shaderID);

    // 1. Bind all textures (Diffuse unit 0, Shadow unit 1)
    AF_Renderer_BindMeshTextures(_mesh, _renderingData, shaderID);

    // 2. Setup Terrain Specifics (Heightmap unit 2, scales, etc.)
    // Bind heightmap texture to unit 2 (0 = diffuse, 1 = shadow map)
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, _terrain->heightmapTextureID);
	
	// Set all terrain uniforms at once
	AF_Shader_SetInt(shaderID, "heightMap", 2);
	AF_Shader_SetFloat(shaderID, "heightScale", _terrain->heightScale);
	AF_Shader_SetVec2(shaderID, "texelSize", _terrain->texelSizeX, _terrain->texelSizeY);
	AF_Shader_SetInt(shaderID, "gridSize", _terrain->gridSize);
	
	// Safety check for numChunks and gridScale
	if (_terrain->numChunks == 0) _terrain->numChunks = 8;
	if (_terrain->gridScale == 0) _terrain->gridScale = 100;
	
	AF_Shader_SetInt(shaderID, "numChunks", _terrain->numChunks);
	
	
    

    // 3. Set Lighting Uniforms
    AF_Shader_SetVec3(shaderID, "viewPos", _cameraPos->x, _cameraPos->y, _cameraPos->z);
    //AF_Lighting_RenderForwardPointLights(shaderID, _ecs, _lightingData);
    
    // 4. Set UV adjustments
    AF_Shader_SetVec2(shaderID, "uvOffset", _mesh->material.diffuseTexture.uvOffsetX, _mesh->material.diffuseTexture.uvOffsetY);
    AF_Shader_SetVec2(shaderID, "uvScale", _mesh->material.diffuseTexture.uvScaleX, _mesh->material.diffuseTexture.uvScaleY);
	
	// Determine number of vertices to draw based on LOD
	// Calculate vertex count for LOD grid
	
	
	// For each terrain chunk, draw with LOD
	// Terrain chunk are gridSize * gridSize
	// each chunk can have dynamic verts set by Lod levels in the editor
	// each chunk needs a model matrix that offsets it to the correct position
	// gridScale now defines the physical size of the chunk, and gridSize defines the vertex density
    AF_FLOAT patchSize = (AF_FLOAT)_terrain->gridScale;
    AF_Shader_SetFloat(shaderID, "patchSize", patchSize);
    
    AF_FLOAT numbOfChunks = (AF_FLOAT)_terrain->numChunks;
    
    // Ensure VAO is initialized for GPU-generated terrain
    if(_mesh->meshes[0].vao == 0){
        AF_RendererBuffer_InitInstancedTerrainMeshBuffer(_terrain->gridSize, _mesh);
    }
    
    glBindVertexArray(_mesh->meshes[0].vao);

    // --- OPTIMIZED CHUNK LOOP ---
    // Instead of looping through ALL chunks, we find the range of chunks visible to the camera.
    // This prevents performance tanking when numChunks is large.
    
    const float visibilityRadius = 8000.0f; // 8km visibility
    
    // Calculate which chunk the camera is currently over
    // Entity position is the center of the total terrain
    float terrainCenterX = _ecs->transforms[_terrainID].pos.x;
    float terrainCenterZ = _ecs->transforms[_terrainID].pos.z;
    
    // Offset relative to the start of the grid (i=0, j=0)
    float gridStartOffsetX = - (numbOfChunks / 2.0f) * patchSize;
    float gridStartOffsetZ = - (numbOfChunks / 2.0f) * patchSize;
    
    // Camera position relative to the grid start
    float relativeCamX = _cameraPos->x - (terrainCenterX + gridStartOffsetX);
    float relativeCamZ = _cameraPos->z - (terrainCenterZ + gridStartOffsetZ);
    
    // Current chunk indices
    int32_t centerI = (int32_t)(relativeCamX / patchSize);
    int32_t centerJ = (int32_t)(relativeCamZ / patchSize);
    
    // Number of chunks to check in each direction
    int32_t chunkRadius = (int32_t)(visibilityRadius / patchSize) + 1;
    
    int32_t startI = centerI - chunkRadius;
    int32_t endI = centerI + chunkRadius;
    int32_t startJ = centerJ - chunkRadius;
    int32_t endJ = centerJ + chunkRadius;
    
    // Clamp to grid boundaries
    if (startI < 0) startI = 0;
    if (endI >= (int32_t)numbOfChunks) endI = (int32_t)numbOfChunks - 1;
    if (startJ < 0) startJ = 0;
    if (endJ >= (int32_t)numbOfChunks) endJ = (int32_t)numbOfChunks - 1;

    for(int32_t i = startI; i <= endI; i++){
        for(int32_t j = startJ; j <= endJ; j++){
            // Centering logic: matches the original coordinate system
            AF_FLOAT xOffset = ((AF_FLOAT)i - (numbOfChunks / 2.0f)) * patchSize;
            AF_FLOAT zOffset = ((AF_FLOAT)j - (numbOfChunks / 2.0f)) * patchSize;

			// Calculate chunk center position
			Vec3 chunkPos = {
				terrainCenterX + xOffset + (patchSize / 2.0f),
				_ecs->transforms[_terrainID].pos.y,
				terrainCenterZ + zOffset + (patchSize / 2.0f)
			};

            // Double check distance for circular culling
            float dx = _cameraPos->x - chunkPos.x;
            float dz = _cameraPos->z - chunkPos.z;
            float distSq = dx*dx + dz*dz;
            if (distSq > (visibilityRadius + patchSize) * (visibilityRadius + patchSize)) {
                continue;
            }

			// Calculate lod level
			_terrain->lodLevel = AF_Renderer_CalculateLODInterval(*_cameraPos, chunkPos, patchSize, _terrain->gridSize);

			uint32_t lodSkip = 1 << _terrain->lodLevel;
			uint32_t lodGridSize = (_terrain->gridSize + lodSkip - 1) / lodSkip;
			const uint32_t vertexCountToDraw = (lodGridSize - 1) * (lodGridSize - 1) * 6;

            Mat4 finalModelMat = *_modelMat;
            finalModelMat.rows[0].w += xOffset;
            finalModelMat.rows[2].w += zOffset;
            
            AF_Shader_SetMat4(shaderID, "model", finalModelMat);
			AF_Shader_SetInt(shaderID, "lodSkipInterval", _terrain->lodLevel);
            AF_Shader_SetVec2(shaderID, "chunkOffset", (AF_FLOAT)i, (AF_FLOAT)j);

            glDrawArrays(GL_TRIANGLES, 0, vertexCountToDraw);
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
