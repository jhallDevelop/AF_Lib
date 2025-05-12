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
#include "AF_Vec3.h"
#include "AF_Mat4.h"
#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION
#include "AF_Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "AF_Assets.h"

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

/*
====================
AF_Renderer_Awake
Init OpenGL
====================
*/
uint32_t AF_Renderer_Awake(void){
    uint32_t success = 1;
    AF_Log("AF_Renderer_Awake\n");
    //Initialize GLEW
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    AF_Renderer_CheckError( "Error initializing GLEW! \n");

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
	glCullFace(GL_BACK);  // Cull the back faces (this is the default)
	glFrontFace(GL_CW);  // Counter-clockwise winding order (default)
	glEnable(GL_CULL_FACE); // Enable culling
	
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
void AF_Renderer_Start(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight){
	AF_Log("AF_Renderer_Start\n");
	if(_renderingData == NULL || _screenWidth == NULL || _screenHeight == NULL){}
	

	// ==== Setup Depth Map and Texture ====
	uint16_t depthTextureWidth = AF_RENDERINGDATA_SHADOW_WIDTH;
	uint16_t depthTextureHeight = AF_RENDERINGDATA_SHADOW_HEIGHT;
	AF_Renderer_Start_DepthFrameBuffers(_renderingData, &depthTextureWidth, &depthTextureHeight);

	// ==== Setup Screen FBO (for main scene render to ImGui viewport) ====
    if (_screenWidth != NULL && _screenHeight != NULL && *_screenWidth > 0 && *_screenHeight > 0) {
        AF_Renderer_Start_ScreenFrameBuffers(_renderingData, _screenWidth, _screenHeight);
    } else {
        AF_Log_Error("AF_Renderer_Start: Screen dimensions not valid for initial screen FBO setup. Attempting with default or expect Editor_Viewport_Render to create.\n"); 
    }

	/*

    // ==== Setup Screen Quad VAO/VBO (used by RenderScreenFBOQuad) ====
    // This is already called inside AF_Renderer_Start_ScreenFrameBuffers.
    // If AF_Renderer_Start_ScreenFrameBuffers might not run (e.g., due to screenWidth being 0),
    AF_Renderer_CreateScreenFBOQuadMeshBuffer(_renderingData); //should be called here separately or ensured.

    AF_Log("AF_Renderer_Start: Initial FBOs and resources setup attempted.\n");
    AF_Log("  depthFBO_ID: %u, depthMapTextureID: %u\n", _renderingData->depthFBO_ID, _renderingData->depthMapTextureID);
    AF_Log("  depthDebugFBO_ID: %u, depthDebugTextureID: %u\n", _renderingData->depthDebugFBO_ID, _renderingData->depthDebugTextureID);
    AF_Log("  screenFBO_ID: %u, screenFBO_TextureID: %u\n", _renderingData->screenFBO_ID, _renderingData->screenFBO_TextureID);
	*/
	AF_Log_Error("AF_Renderer_Start: Disabled depth and debug FBO creation\n");
}

/*
====================
AF_Renderer_Start_ScreenFrameBuffers
Start function which frame buffers are initialised
====================
*/
void AF_Renderer_Start_ScreenFrameBuffers(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight){
	// ============ SCREEN FRAME BUFFER OBJECT =========== 
    uint32_t* fbo = &_renderingData->screenFBO_ID;
    uint32_t* rbo = &_renderingData->screenRBO_ID;
    uint32_t* textureID = &_renderingData->screenFBO_TextureID;
    uint16_t* viewportWidth = _screenWidth;
    uint16_t* viewportHeight = _screenHeight;
    // Create the frame buffer, containing many steps (fbo, rbo, texture id ext)
    AF_Renderer_CreateFramebuffer(fbo, rbo, textureID, viewportWidth, viewportHeight, GL_RGB, GL_COLOR_ATTACHMENT0, GL_TRUE, GL_TRUE, GL_LINEAR, GL_LINEAR);

	// Create the screen shader to use
	char screenVertShaderFullPath[MAX_PATH_CHAR_SIZE];
	char screenFragShaderFullPath[MAX_PATH_CHAR_SIZE];
	snprintf(screenVertShaderFullPath, MAX_PATH_CHAR_SIZE, "assets/shaders/%s", SCREEN_VERT_SHADER_PATH);
	snprintf(screenFragShaderFullPath, MAX_PATH_CHAR_SIZE, "assets/shaders/%s", SCREEN_FRAG_SHADER_PATH);
	_renderingData->screenFBO_ShaderID = AF_Shader_Load(screenVertShaderFullPath, screenFragShaderFullPath);
	// Set the screen Frame buffer texture
	glUseProgram(_renderingData->screenFBO_ShaderID);
	AF_Shader_SetInt(_renderingData->screenFBO_ShaderID, "screenTexture", 0);
    glUseProgram(0);
}

/*
====================
AF_Renderer_Start_DepthFrameBuffers
Start function which depth buffers are initialised
====================
*/
void AF_Renderer_Start_DepthFrameBuffers(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight){
	// ============ SCREEN FRAME BUFFER OBJECT =========== 
    uint32_t* fbo = &_renderingData->depthFBO_ID;
    uint32_t* rbo = &_renderingData->depthRBO_ID;
    uint32_t* textureID = &_renderingData->depthMapTextureID;
    uint16_t* viewportWidth = _screenWidth;
    uint16_t* viewportHeight = _screenHeight;
    // Create the frame buffer, containing many steps (fbo, rbo, texture id ext)
    //AF_Renderer_CreateFramebuffer(fbo, rbo, textureID, viewportWidth, viewportHeight, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_NONE, GL_NONE, GL_NEAREST, GL_NEAREST);
	//AF_Renderer_CreateFramebuffer(fbo, rbo, textureID, viewportWidth, viewportHeight, GL_RGB, GL_COLOR_ATTACHMENT0, GL_TRUE, GL_TRUE, GL_LINEAR, GL_LINEAR);
	AF_Renderer_CreateFramebuffer(
		fbo,
		rbo, // Or &_renderingData->depthRBO_ID if you insist on an RBO AND a depth texture (unusual for simple depth map)
		textureID,
		viewportWidth,
		viewportHeight,
		GL_DEPTH_COMPONENT24,      // Internal format for the depth texture
		GL_DEPTH_ATTACHMENT,       // Attach it as a depth buffer
		GL_NONE,                   // No color buffer to draw to
		GL_NONE,                   // No color buffer to read from
		GL_NEAREST,                // Min filter for depth (often nearest)
		GL_NEAREST                 // Mag filter for depth (often nearest)
	);
	// Create a frame
	// Create Depth Frame Buffer and textured
	// Create the depth shader to use
	
	char depthVertShaderFullPath[MAX_PATH_CHAR_SIZE];
	char depthFragShaderFullPath[MAX_PATH_CHAR_SIZE];
	snprintf(depthVertShaderFullPath, MAX_PATH_CHAR_SIZE, "assets/shaders/%s", DEPTH_VERT_SHADER_PATH);
	snprintf(depthFragShaderFullPath, MAX_PATH_CHAR_SIZE, "assets/shaders/%s", DEPTH_FRAG_SHADER_PATH);
	_renderingData->depthRenderShaderID = AF_Shader_Load(depthVertShaderFullPath, depthFragShaderFullPath);
	
	
	// Create the depth debug shader to use
	char depthDebugVertShaderFullPath[MAX_PATH_CHAR_SIZE];
	char depthDebugFragShaderFullPath[MAX_PATH_CHAR_SIZE];
	snprintf(depthDebugVertShaderFullPath, MAX_PATH_CHAR_SIZE, "assets/shaders/%s", DEPTH_DEBUG_VERT_SHADER_PATH);
	snprintf(depthDebugFragShaderFullPath, MAX_PATH_CHAR_SIZE, "assets/shaders/%s", DEPTH_DEBUG_FRAG_SHADER_PATH);
	_renderingData->depthDebugShaderID = AF_Shader_Load(depthDebugVertShaderFullPath, depthDebugFragShaderFullPath);
	 // shader configuration
    // Set the depth texture to use when using the debugging shader
    glUseProgram(_renderingData->depthDebugShaderID);
	AF_Shader_SetInt(_renderingData->depthDebugShaderID, "depthMap", 0);//_renderingData->depthMapTextureID);
    glUseProgram(0);

	// Create depth debug frame buffer to render to texture 
	uint16_t debugTexWidth = AF_RENDERINGDATA_SHADOW_WIDTH;  // Or specific size for debug texture
	uint16_t debugTexHeight = AF_RENDERINGDATA_SHADOW_HEIGHT; // Or specific size for debug texture

	AF_Renderer_CreateFramebuffer(
		&_renderingData->depthDebugFBO_ID,
		&_renderingData->depthDebugRBO_ID, // You likely don't need a separate RBO for depth/stencil on this FBO
			  // if you're just drawing a full-screen quad with depth test disabled.
			  // If AF_Renderer_CreateFramebuffer requires an RBO pointer, pass &_renderingData->depthDebugRBO_ID
			  // but ensure createDepth/createStencil params are FALSE for this call if not needed.
		&_renderingData->depthDebugTextureID, // This is the texture ImGui will display
		&debugTexWidth,
		&debugTexHeight,
		GL_RGB,                    // It MUST be a color texture (e.g., GL_RGB, GL_RGBA)
		GL_COLOR_ATTACHMENT0,      // Attach as color
		GL_FALSE,                  // No depth buffer needed for this debug FBO itself
		GL_FALSE,                  // No stencil buffer needed for this debug FBO itself
		GL_LINEAR,                 // Min filter
		GL_LINEAR                  // Mag filter
	);
	// TODO: use this for shadow map shader
	/*
	glUseProgram(_renderingData->shadowShaderID);
	AF_Shader_SetInt(_renderingData->shadowShaderID, "diffuseTexture", 0);
	AF_Shader_SetInt(_renderingData->shadowShaderID, "shadowMap", 1);
	glUseProgram(0);
	*/
	
}

void AF_Renderer_StartRendering(Vec4 _backgroundColor)
{
	// Clear Screen and buffers
	glClearColor(_backgroundColor.x, _backgroundColor.y,_backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}
/*
====================
AF_Renderer_Render
Simple render command to decide how to progress other rendering steps
====================
*/
void AF_Renderer_Render(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_Entity* _cameraEntity){

	// START RENDERING
	AF_Renderer_CheckError( "AF_Renderer_Render: Error at start of Rendering OpenGL setting color and clearing screen! \n");

	// Update lighting data
	AF_Renderer_UpdateLighting(_ecs, _lightingData);


	// Switch Between Renderer
	switch(_renderingData->rendererType)
	{
		// FORWARD RENDERING
		case AF_RENDERER_FORWARD:
			AF_Renderer_StartForwardRendering(_ecs, _renderingData, _lightingData, _cameraEntity);
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
void AF_Renderer_StartForwardRendering(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_Entity* _cameraEntity){
    AF_CCamera* camera = _cameraEntity->camera;
    uint16_t mainCameraWidth = (uint16_t)camera->windowWidth;   // Width for screenFBO_TextureID
    uint16_t mainCameraHeight = (uint16_t)camera->windowHeight; // Height for screenFBO_TextureID

    // Ensure critical FBOs are valid (simple check, robust check is glCheckFramebufferStatus in creation)
    if (_renderingData->depthFBO_ID == 0 || _renderingData->screenFBO_ID == 0 || _renderingData->depthDebugFBO_ID == 0) {
        //AF_Log_Error("AF_Renderer_StartForwardRendering: One or more critical FBOs are not initialized!\n");
        //return;
    }

    if ((mainCameraWidth > 0 && mainCameraHeight > 0 )){//&& (viewPort->isResized == TRUE)) || viewPort->isRefreshed == FALSE) {
        // Delete the existing framebuffer, texture, and renderbuffer if they exist
        AF_Renderer_CreateFramebuffer(&_renderingData->screenFBO_ID, &_renderingData->screenRBO_ID, &_renderingData->screenFBO_TextureID, &mainCameraWidth, &mainCameraHeight, GL_RGB, GL_COLOR_ATTACHMENT0, GL_TRUE, GL_TRUE, GL_LINEAR, GL_LINEAR);

        //update the glviewport size and position
        //glViewport(viewPort->viewPortPosX, viewPort->viewPortPosY, viewPort->viewPortFramebufferWidth, viewPort->viewPortFramebufferHeight);
		//glViewport(0, 0, mainCameraWidth, mainCameraHeight);
        //viewPort->isResized = TRUE;
    }

    // 1. ==== DEPTH PASS (Populates _renderingData->depthMapTextureID) ====
    // This pass renders scene geometry from the light's perspective to a depth texture.
	/*
    glEnable(GL_DEPTH_TEST); // Depth testing must be enabled for the depth pass
    glDepthMask(GL_TRUE);    // Enable writing to the depth buffer

    AF_Renderer_BindFrameBuffer(_renderingData->depthFBO_ID);
    glViewport(0, 0, AF_RENDERINGDATA_SHADOW_WIDTH, AF_RENDERINGDATA_SHADOW_HEIGHT);
    //glClear(GL_DEPTH_BUFFER_BIT); // Only clear depth for a depth-only FBO

    // AF_Renderer_StartDepthPass configures the light's view/projection,
    // sets the depth shader, and calls AF_Render_DrawMeshElements (or similar)
    // to draw relevant objects.
    AF_Renderer_StartDepthPass(_renderingData, _lightingData, _ecs, camera); // Pass main camera for now, StartDepthPass should derive light's camera

    AF_Renderer_UnBindFrameBuffer(); // Unbind, back to default framebuffer (0)
*/

    // 2. ==== MAIN COLOR PASS (Populates _renderingData->screenFBO_TextureID) ====
    // This pass renders the scene normally from the main camera's perspective.
    // It uses the _renderingData->depthMapTextureID for shadow calculations.
    // Output goes to _renderingData->screenFBO_ID (which has _renderingData->screenFBO_TextureID attached).
    // This FBO is typically managed (created/resized) by your Editor_Viewport_Render.

	// tODO change back to screen fbo
    AF_Renderer_BindFrameBuffer(_renderingData->screenFBO_ID);
    glViewport(0, 0, mainCameraWidth, mainCameraHeight); // Viewport for the main scene render

    // Set clear color for the main scene (e.g., camera's background color)
    // Assuming camera->backgroundColor is a Vec4 or similar
    //glClearColor(camera->backgroundColor.x, camera->backgroundColor.y, camera->backgroundColor.z, camera->backgroundColor.w);
    glClearColor(camera->backgroundColor.x, camera->backgroundColor.y, camera->backgroundColor.z, 1.0f); // Example clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth
	
	//glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Example clear color
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Example clear color
	//glClearColor(__apda, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST); // Ensure depth testing is on
    glDepthMask(GL_TRUE);    // Ensure depth writing is on

    // AF_Renderer_DrawMeshes renders all visible entities using their respective materials,
    // shaders, lighting, and applies shadows using depthMapTextureID and lightSpaceMatrix.
    AF_Renderer_DrawMeshes(
        &camera->viewMatrix,
        &camera->projectionMatrix,
        _ecs,
        &_cameraEntity->transform->pos, // Camera position for lighting calculations
        _lightingData
        // Note: AF_Renderer_DrawMeshes needs access to _renderingData->depthMapTextureID
        // and the lightSpaceMatrix (calculated in AF_Renderer_StartDepthPass)
        // to implement shadows. You might need to pass _renderingData or these specific items.
    );
    // After this, _renderingData->screenFBO_TextureID contains the final rendered scene.
    AF_Renderer_UnBindFrameBuffer(); // Unbind, back to default framebuffer (0)


    // 3. ==== VISUALIZE DEPTH TO TEXTURE (Populates _renderingData->depthDebugTextureID) ====
    // This pass takes the raw _renderingData->depthMapTextureID, uses _renderingData->depthDebugShaderID
    // to convert depth to a visual format (e.g., grayscale), and renders this to _renderingData->depthDebugTextureID.
/*
    AF_Renderer_BindFrameBuffer(_renderingData->depthDebugFBO_ID);
    // Viewport for the debug texture (typically same dimensions as the shadow map)
    glViewport(0, 0, AF_RENDERINGDATA_SHADOW_WIDTH, AF_RENDERINGDATA_SHADOW_HEIGHT);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear debug texture to black
    //glClear(GL_COLOR_BUFFER_BIT);         // Only clear color; depth test is disabled in RenderScreenFBOQuad

    // AF_Renderer_RenderScreenFBOQuad should internally:
    // - Use _renderingData->depthDebugShaderID
    // - Bind _renderingData->depthMapTextureID as input texture
    // - Draw a full-screen quad
    AF_Renderer_RenderScreenFBOQuad(_renderingData);

    AF_Renderer_UnBindFrameBuffer(); // Unbind, back to default framebuffer (0)
*/

    // All off-screen rendering is complete.
    // The main application loop will now handle ImGui rendering,
    // which will use _renderingData->screenFBO_TextureID (for the viewport)
    // and _renderingData->depthDebugTextureID (for the depth map debug window).
}


/*
====================
AF_Renderer_EndForwardRendering
Simple render command to cleanup forward rendering steps
====================
*/
void AF_Renderer_EndForwardRendering(void){

	// ==== DEPTH PASS ====

	// ==== COLOR PASS ====

	// ==== LIGHTING PASS ====

}


void AF_Renderer_RenderDepthMeshes(AF_ECS* _ecs){
	if(_ecs == NULL){}
}



// ============================ TEXTURES ================================ 

void AF_Renderer_SetTexture(const uint32_t _shaderID, const char* _shaderVarName, uint32_t _textureID){
    glUseProgram(_shaderID); // Bind the shader program
    glUniform1i(glGetUniformLocation(_shaderID, _shaderVarName), _textureID); // Tell the shader to set the "Diffuse_Texture" variable to use texture id 0
    glUseProgram(0);
}

/*
====================
AF_Renderer_ReLoadTexture
Reload textures
====================
*/
AF_Texture AF_Renderer_ReLoadTexture(AF_Assets* _assets, const char* _texturePath) {
    AF_Texture returnTexture = {0, AF_TEXTURE_TYPE_NONE, ""}; // Initialize to invalid

    if (!_texturePath || _texturePath[0] == '\0') {
        AF_Log_Error("AF_Renderer_ReLoadTexture: Null or empty texture path provided.\n");
        return returnTexture;
    }
    snprintf(returnTexture.path, MAX_PATH_CHAR_SIZE, "%s", _texturePath);

    // Potentially check cache first if you don't want to *always* reload from disk
    AF_Texture cachedTexture = AF_Assets_GetTexture(_assets, _texturePath);
    if (cachedTexture.type != AF_TEXTURE_TYPE_NONE) {
		AF_Log("AF_Renderer_ReLoadTexture: Loading Cached texture id: %i from assets for path: %s\n", returnTexture.id, _texturePath);
    //     // Optional: Could check glIsTexture(cachedTexture.id) here if paranoid
         return cachedTexture;
    }

	AF_Log("AF_Renderer_ReLoadTexture: Cached texture not found. Loading texture for first time: %s\n", _texturePath);
    returnTexture.id = AF_Renderer_LoadTexture(_texturePath);

    if (returnTexture.id == 0) { // Now this check is meaningful
        AF_Log_Error("AF_Renderer_ReLoadTexture: Call to AF_Renderer_LoadTexture failed for path: %s\n", _texturePath);
        // returnTexture.type is already AF_TEXTURE_TYPE_NONE
        return returnTexture;
    }

    returnTexture.type = AF_TEXTURE_TYPE_DIFFUSE; // Or determine more robustly
	AF_Log("AF_Renderer_ReLoadTexture: Cached texture id: %i stored in assets: %s\n",returnTexture.id,  _texturePath);
    AF_Assets_AddTexture(_assets, returnTexture); // Add/update in asset manager

    return returnTexture;
}


/*
====================
AF_Renderer_SetFlipImage
Set the flip image for stb_image.h
====================
*/
// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
void AF_Renderer_SetFlipImage(BOOL _flipImage)	{
	bool isFlipped = false;
	if(_flipImage == FALSE){
		isFlipped = false;
	}else{
		isFlipped = true;
	}
	
    stbi_set_flip_vertically_on_load(isFlipped);
}


/*
====================
AF_Renderer_LoadTexture
Load textures
====================
*/
unsigned int AF_Renderer_LoadTexture(char const * path) {
    if (!path || path[0] == '\0') {
        AF_Log_Error("AF_Renderer_LoadTexture: Null or empty texture path provided.\n");
        return 0; // Return 0 for invalid path
    }

    unsigned int textureID = 0; // Initialize to 0
    int width, height, nrComponents;
	//AF_Renderer_SetFlipImage(true);
    //stbi_set_flip_vertically_on_load(true); // Often needed for OpenGL, make it consistent or configurable
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
		glBindTexture(GL_TEXTURE_2D,0); // free the old bind texture if deleted
        glGenTextures(1, &textureID); // Generate ID only if data is loaded
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLenum internalFormat = GL_RGB;
        GLenum dataFormat = GL_RGB;
        if (nrComponents == 1) {
            internalFormat = GL_RED; dataFormat = GL_RED;
        } else if (nrComponents == 3) {
            internalFormat = GL_RGB8; dataFormat = GL_RGB; // Use sized internal format
        } else if (nrComponents == 4) {
            internalFormat = GL_RGBA8; dataFormat = GL_RGBA; // Use sized internal format
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0); // Good practice: unbind after configuring
        stbi_image_free(data);
        return textureID;
    } else {
        AF_Log_Error("AF_Renderer_LoadTexture: Texture failed to load at path: \"%s\" (stbi_load error: %s)\n", path, stbi_failure_reason());
        // No textureID was generated and bound with data, so return 0
        return 0;
    }
}


// ============================  DRAW ================================ 

/*
====================
AF_Render_DrawMeshElements
Simple DrawElements call on all ECS mesh components
====================
*/
void AF_Render_DrawMeshElements(AF_ECS* _ecs, Mat4* _lightProjection, Vec3* _viewPos, uint32_t _shaderID){
	if(_shaderID == 0){}
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_Entity* entity = &_ecs->entities[i];
		BOOL hasEntity = AF_Component_GetHas(entity->flags);
		if(hasEntity == FALSE){
			continue;
		}

		AF_CMesh* mesh = &_ecs->meshes[i];
		BOOL meshHas = AF_Component_GetHas(mesh->enabled);
		BOOL hasEnabled = AF_Component_GetEnabled(mesh->enabled);
		// Skip if there is no rendering component
		if(meshHas == FALSE){// || hasEnabled == FALSE){
			continue;
		}

		// ---- Setup shader ----
		//uint32_t shader = _shaderID;//mesh->shader.shaderID;
		glUseProgram(_shaderID); 
		for(uint32_t x = 0; x < mesh->meshCount; x++){
			
			glBindVertexArray(mesh->meshes[x].vao);//_meshList->vao);
			AF_Renderer_CheckError( "AF_Render_DrawMeshElements: Error bind vao Rendering OpenGL! \n");

			// If you want to explicitly bind the VBO (usually not necessary if VBOs are part of the VAO):
			glBindBuffer(GL_ARRAY_BUFFER, mesh->meshes[x].vbo);
			AF_Renderer_CheckError("AF_Render_DrawMeshElements: Error binding VBO for drawing!");


			//---------------Send command to Graphics API to Draw Triangles------------
			
			// NOTE: GL_TRUE Indicates that the matrix you are passing to OpenGL is in row-major order 

			// TODO: make this configurable from the editor component
			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK); // Default
			//glFrontFace(GL_CCW); // Or GL_CW
			//glFrontFace(GL_CW);
			
			AF_Shader_SetMat4(_shaderID,"model", *_lightProjection);
			//AF_Log("==== ------------------ ====\n");

			// Texture 
			//int textureUniformLocation = glGetUniformLocation(shaderID, "image");

			// send the camera data to the shader
			// Prep drawing
			unsigned int indexCount = mesh->meshes[x].indexCount;
			if(indexCount == 0){
				AF_Log_Warning("AF_Render_DrawMeshElements: indexCount is 0. Can't draw elements\n");
				return;
			}


			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
				
			AF_Renderer_CheckError( "AF_Render_DrawMeshElements drawElements Rendering OpenGL! \n");
			glUseProgram(0); 
			glBindVertexArray(0);
			AF_Renderer_CheckError( "AF_Render_DrawMeshElements: bindvertexarray(0) Rendering OpenGL! \n");
		}

	}
}
	

/*
====================
AF_Renderer_DrawMeshes
Loop through the entities and draw the meshes that have components attached
====================
*/
void AF_Renderer_DrawMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData){
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_Entity* entity = &_ecs->entities[i];
		if(!AF_Component_GetHas(entity->flags)){
			continue;
		}

		AF_CMesh* mesh = &_ecs->meshes[i];
		// Skip if there is no rendering component
		if(!AF_Component_GetHas(mesh->enabled)){// || hasEnabled == FALSE){
			continue;
		}
		AF_CTransform3D* modelTransform = &_ecs->transforms[i];

		// Make a copy as we will apply some special transformation. e.g. rotation is stored in degrees and needs to be converted to radians
		Vec3 rotationToRadians = {AF_Math_Radians(modelTransform->rot.x),AF_Math_Radians(modelTransform->rot.y), AF_Math_Radians(modelTransform->rot.z)};
		// Update the model matrix
		Mat4 modelMatColumn = Mat4_ToModelMat4(_ecs->transforms[i].pos, rotationToRadians, _ecs->transforms[i].scale);
		
		AF_Renderer_DrawMesh(&modelMatColumn, _viewMat, _projMat, mesh, _ecs, _cameraPos, _lightingData);
	}
}

/*
====================
AF_Renderer_DrawMesh
Loop through the meshes in a component and draw using opengl
====================
*/
void AF_Renderer_DrawMesh(Mat4* _modelMat, Mat4* _viewMat, Mat4* _projMat, AF_CMesh* _mesh, AF_ECS* _ecs, Vec3* _cameraPos, AF_LightingData* _lightingData){
	// draw meshes
	if(_modelMat == NULL || _viewMat == NULL || _projMat == NULL || _mesh == NULL)
	{
		AF_Log_Error("AF_Renderer_DrawMesh: Passed Null reference \n");
		return;
	}
	
	// don't render if we are not enabled or the component isn't supposed to render
	if(!AF_Component_GetHasEnabled(_mesh->enabled)){
		return;
	}
	// TODO: this is very expensive. batch these up or just per model, use one shader/material
	// ---- Setup shader ----
	uint32_t shader = _mesh->shader.shaderID;
	glUseProgram(shader); 
	for(uint32_t i = 0; i < _mesh->meshCount; i++){
		// TODO: Render based on shader type 
		// Does the shader use Textures?
		if(_mesh->textured == TRUE){
			// ---- Diffuse Texture ----
			if((_mesh->meshes[i].material.diffuseTexture.type != AF_TEXTURE_TYPE_NONE)){
				uint32_t diffuseTextureBinding = 0;
				glActiveTexture(GL_TEXTURE0 + diffuseTextureBinding); // active proper texture unit before binding
				glUniform1i(glGetUniformLocation(shader, "material.diffuse"), diffuseTextureBinding);

				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, _mesh->meshes[i].material.diffuseTexture.id);
			}

			/*
			// ---- Normal Texture ----
			if((_mesh->meshes[i].material.normalTexture.type != AF_TEXTURE_TYPE_NONE)){
				uint32_t normalTextureBinding = 1;
				glActiveTexture(GL_TEXTURE0 + normalTextureBinding); // active proper texture unit before binding
				glUniform1i(glGetUniformLocation(shader, "normal"), normalTextureBinding);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, _mesh->meshes[i].material.normalTexture.id);
			}

			// ---- Specular Texture ----
			if((_mesh->meshes[i].material.specularTexture.type != AF_TEXTURE_TYPE_NONE)){
				uint32_t specularTextureBinding = 2;
				glActiveTexture(GL_TEXTURE0 + specularTextureBinding); // active proper texture unit before binding
				glUniform1i(glGetUniformLocation(shader, "specular"), specularTextureBinding);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, _mesh->meshes[i].material.specularTexture.id);
			}*/
		}


		// Does the shader use lighting?
        //if(_mesh->recieveLights == TRUE){
		// TODO: confirm if the camera position is stored in column or row major order of the viewMat
		glUniform3f(glGetUniformLocation(shader, "viewPos"), _cameraPos->x, _cameraPos->y, _cameraPos->z); 
		// ideally shininess is set to 32.0f
		/*
		if((_mesh->meshes[i].material.diffuseTexture != NULL) && (_mesh->meshes[i].material.diffuseTexture->type != AF_TEXTURE_TYPE_NONE)){
			uint32_t diffuseTextureBinding = 0;
			glActiveTexture(GL_TEXTURE0 + diffuseTextureBinding); // active proper texture unit before binding
			glUniform1i(glGetUniformLocation(shader, "material.diffuse"), diffuseTextureBinding);

			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, _mesh->meshes[i].material.diffuseTexture->id);
		}*/

		// Get the next available lights and send data to shader up to MAX_LIGHT_NUM, likley 4
		if(_mesh->recieveLights == TRUE){
			AF_Renderer_RenderForwardPointLights(shader, _ecs, _lightingData);
		}
		

		glBindVertexArray(_mesh->meshes[i].vao);//_meshList->vao);
		AF_Renderer_CheckError( "Error bind vao Rendering OpenGL! \n");

		// If you want to explicitly bind the VBO (usually not necessary if VBOs are part of the VAO):
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->meshes[i].vbo);
		AF_Renderer_CheckError("Error binding VBO for drawing!");


		//---------------Send command to Graphics API to Draw Triangles------------
		
		// NOTE: GL_TRUE Indicates that the matrix you are passing to OpenGL is in row-major order 

		// TODO: make this configurable from the editor component
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK); // Default
		//glFrontFace(GL_CCW); // Or GL_CW
		//glFrontFace(GL_CW);
		//AF_Log("==== Projection Matrix ====\n");
		//AF_Util_Mat4_Log(*_projMat);
		int projLocation = glGetUniformLocation(shader, "projection");
		glUniformMatrix4fv(projLocation, 1, GL_TRUE, (float*)&_projMat->rows);

		// View
		//AF_Log("==== View Matrix ====\n");
		//AF_Util_Mat4_Log(*_viewMat);
		int viewLocation = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_TRUE, (float*)&_viewMat->rows);

		// Model
		//AF_Log("==== Model Matrix ====\n");
		//AF_Util_Mat4_Log(*_modelMat);
		int modelLocation = glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(modelLocation, 1, GL_TRUE, (float*)&_modelMat->rows);

		//AF_Log("==== ------------------ ====\n");

		// Texture 
		//int textureUniformLocation = glGetUniformLocation(shaderID, "image");

		// send the camera data to the shader
		// Prep drawing
		unsigned int indexCount = _mesh->meshes[i].indexCount;
		if(indexCount == 0){
			AF_Log_Warning("AF_Renderer_DrawMesh: indexCount is 0. Can't draw elements\n");
			return;
		}

		// TODO: sort transparent objects before rendering
		//https://learnopengl.com/Advanced-OpenGL/Blending

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
			
		AF_Renderer_CheckError( "AF_Renderer_DrawMesh_Error drawElements Rendering OpenGL! \n");

		glBindVertexArray(0);
		AF_Renderer_CheckError( "Error bindvertexarray(0) Rendering OpenGL! \n");
		
	}
	// Unbind shader
    glUseProgram(0);

    // Unbind textures explicitly from the units they were bound to
    // Assuming these were the maximum units you might have used within the loop.
    // If _mesh->textured was false, these calls are harmless.
    if(_mesh->textured == TRUE) { // Only unbind if textures were potentially bound
        // Check each texture type again, similar to how you bound them
        // This is a bit repetitive; ideally, you'd track which units were used.
        // For now, let's assume you used up to 3 units if textured.

        // Diffuse Texture was on unit 0
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Normal Texture was on unit 1
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Specular Texture was on unit 2
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // It's good practice to reset the active texture unit to a default,
    // though well-behaved subsequent code (like ImGui's backend) should set its own.
    glActiveTexture(GL_TEXTURE0);
}

/*
====================
AF_Renderer_CreateScreenFBOQuadMeshBuffer
Render the quad to the screen and swap the frame buffers over.
====================
*/
void AF_Renderer_RenderScreenFBOQuad(AF_RenderingData* _renderingData){
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Ensure drawing to default screen
    glDisable(GL_DEPTH_TEST);

    glUseProgram(_renderingData->depthDebugShaderID);

    // Uniforms for linearization (optional, shader dependent)
    float near_plane = 1.0f, far_plane = 7.5f; // These should match the projection used for the depth pass
    AF_Shader_SetFloat(_renderingData->depthDebugShaderID, "near_plane", near_plane);
    AF_Shader_SetFloat(_renderingData->depthDebugShaderID, "far_plane", far_plane);

    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, _renderingData->depthMapTextureID); // Bind your actual depth map texture

    if (_renderingData->screenQUAD_VAO == 0) { // Lazy init, good
        AF_Renderer_CreateScreenFBOQuadMeshBuffer(_renderingData);
    }
    glBindVertexArray(_renderingData->screenQUAD_VAO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glUseProgram(0);
    // Consider re-enabling depth test if needed by subsequent rendering
    // glEnable(GL_DEPTH_TEST);
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

	    BOOL hasMesh = AF_Component_GetHas(_mesh->enabled);
	    // Skip setting up if we don't have a mesh component
	    if(hasMesh == FALSE){
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
		
	GLuint gVAO, gVBO, gEBO;
	glGenVertexArrays(1, &gVAO);
	glGenBuffers(1, &gVBO);
	glGenBuffers(1, &gEBO);
	AF_Renderer_CheckError( "OpenGL error occurred during gVAO, gVBO, gEBO buffer creation.\n");

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s)
	glBindVertexArray(gVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	AF_Renderer_CheckError( "OpenGL error occurred during binding of the gVAO, gVBO.\n");

	// our buffer needs to be 8 floats (3*pos, 3*normal, 2*tex)
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, _meshData->vertices, GL_STATIC_DRAW);
	AF_Renderer_CheckError( "OpenGL error occurred during glBufferData for the verts.\n");
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the IBO and set the buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _meshData->indexCount * sizeof(uint32_t), &_meshData->indices[0], GL_STATIC_DRAW);
	AF_Renderer_CheckError( "OpenGL error occurred during glBufferData for the indexes.\n");

	// Stride is 8 floats wide, 3*pos, 3*normal, 2*tex
	// Vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Vertex tangent attributes
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Vertex bi tangent attributes
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Vertex texture coords
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(4);

	AF_Renderer_CheckError( "OpenGL error occurred during assignment of vertexAttribs.\n");

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0); 

	_meshData->vao = gVAO;
	_meshData->vbo = gVBO;
	_meshData->ibo = gEBO;
	// Bind the IBO and set the buffer data
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, _meshList->meshes->indices, GL_STATIC_DRAW);

	// Now that the mesh is loaded, we can delete the memory created for the verts and indices
	free(_meshData->vertices);
	_meshData->vertices = NULL;
	free(_meshData->indices);
	_meshData->indices = NULL;
	AF_Renderer_CheckError( "Error InitMesh Buffers for OpenGL! \n");
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

/*
====================
AF_Renderer_CreateFramebuffer
Create FBO, RBO, and Texture to use in frame buffer rendering for color
====================
*/
void AF_Renderer_CreateFramebuffer(uint32_t *_fbo, uint32_t *_rbo, uint32_t *_textureID, uint16_t *_textureWidth, uint16_t *_textureHeight, uint32_t _internalFormat, uint32_t _textureAttatchmentType, uint32_t _drawBufferType, uint32_t _readBufferType, uint32_t _minFilter, uint32_t _magFilter)
{
	// Delete the existing framebuffer, texture, and renderbuffer if they exist
    AF_Renderer_DeleteFBO(_fbo);
    AF_Renderer_DeleteRBO(_rbo);
	AF_Renderer_DeleteTexture(_textureID);

    
    // Generate the framebuffer id
    *_fbo = AF_Renderer_CreateFBO();
    AF_Renderer_BindFrameBuffer(*_fbo);
	
    // Generate texture to render to
	*_textureID = AF_Renderer_CreateFBOTexture(*_textureWidth, *_textureHeight, _internalFormat, GL_UNSIGNED_BYTE, _minFilter, _magFilter);

	AF_Renderer_BindFrameBufferToTexture(*_fbo, *_textureID, _textureAttatchmentType);

    // Generate renderbuffer for depth and stencil
    *_rbo = AF_Renderer_CreateRBO();
    AF_Renderer_BindRenderBuffer(*_rbo, *_textureWidth, *_textureHeight);
    AF_Renderer_CheckFrameBufferStatus("AF_Renderer_CreateFramebuffer\n");
	
	// TODO: take in as args as some buffers e.g. depth buffer need this
	if((GLint)_drawBufferType == GL_NONE){
		glDrawBuffer(GL_NONE);
    	glReadBuffer(GL_NONE);
	}
	
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
uint32_t AF_Renderer_CreateFBOTexture(uint32_t _textureWidth, uint32_t _textureHeight, uint32_t _internalFormat, uint32_t _pixelDataType, uint32_t _minFilter, uint32_t _magFilter){ // Added params

    unsigned int fboTextureID = 0;
	glBindTexture(GL_TEXTURE_2D,0);
    glGenTextures(1, &fboTextureID);
    glBindTexture(GL_TEXTURE_2D, fboTextureID);

    // Determine format based on internalFormat (simplified example)
    GLenum format = GL_RGB; // Default
    if ((GLenum)_internalFormat == GL_DEPTH_COMPONENT) {
        format = GL_DEPTH_COMPONENT;
    } else if ((GLenum)_internalFormat == GL_RGBA || (GLenum)_internalFormat == GL_RGBA16F || (GLenum)_internalFormat == GL_RGBA32F) {
         format = GL_RGBA;
    } // Add more cases if needed
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, // Use the specific internal format
        _textureWidth, _textureHeight, 0,
        format, // Use the determined format
        (GLenum)_pixelDataType, // Use the specified data type
        NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)_minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)_magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Add wrap parameters if needed (often GL_CLAMP_TO_EDGE for FBO textures)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
void AF_Renderer_StartDepthPass(AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_ECS* _ecs, AF_CCamera* _camera){
	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	
	Mat4 lightProjection;
	Mat4 lightView;
	Mat4 lightSpaceMatrix;
	float near_plane = 1.0f;
	float far_plane = 7.5f;
	//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
	AF_CCamera depthCamera = AF_CCamera_ZERO();
	depthCamera.nearPlane = near_plane;
	depthCamera.farPlane = far_plane;
	lightProjection = Mat4_Ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	//glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	Vec3 upDirection = {0.0f, 1.0f, 0.0f};
	Vec3 targetPos = _ecs->entities[_lightingData->ambientLightEntityIndex].transform->pos;// lightPos 

	Vec3 viewPos =  {0,0,0};	// 0
	lightView = Mat4_Lookat(targetPos, viewPos, upDirection); //glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = Mat4_MULT_M4(lightProjection, lightView);

	// render scene from light's point of view
	AF_Shader_Use(_renderingData->depthRenderShaderID); 
	AF_Shader_SetMat4(_renderingData->depthRenderShaderID, "lightSpaceMatrix", lightSpaceMatrix);

	// Render meshes
	AF_Render_DrawMeshElements(_ecs, &lightProjection, &viewPos, _renderingData->depthRenderShaderID);
	AF_Shader_Use(0);
	
	/*
	// render Depth map to quad for visual debugging
	// ---------------------------------------------
	glUseProgram(_renderingData->depthDebugShaderID);
	//AF_Shader_SetFloat(_renderingData->depthDebugShaderID, "near_plane", near_plane);
	//AF_Shader_SetFloat(_renderingData->depthDebugShaderID, "far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _renderingData->depthMapTextureID);
	//renderQuad();
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
*/
	/*
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	renderScene(simpleDepthShader);
	*/
}

// ============================  LIGHTING ================================ 
/*
====================
AF_Renderer_RenderForwardPointLights
Update shaders with lighting data
====================
*/
void AF_Renderer_RenderForwardPointLights(uint32_t _shader, AF_ECS* _ecs, AF_LightingData* _lightingData){
	
	// if we have a found ambient light
	if(_lightingData->ambientLightEntityIndex > 0){
		AF_CLight* light = &_ecs->lights[_lightingData->ambientLightEntityIndex];
		glUniform1f(glGetUniformLocation(_shader, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(_shader, "dirLight.direction"), light->direction.x, light->direction.y, light->direction.z);
		glUniform3f(glGetUniformLocation(_shader, "dirLight.ambient"),  light->ambientCol.x, light->ambientCol.y, light->ambientCol.z); 
		glUniform3f(glGetUniformLocation(_shader, "dirLight.diffuse"),  light->diffuseCol.x, light->diffuseCol.y, light->diffuseCol.z);
		glUniform3f(glGetUniformLocation(_shader, "dirLight.specular"),  light->specularCol.x, light->specularCol.y, light->specularCol.z);
	}
	// if we have a found spot light
	
	if(_lightingData->spotLightEntityIndex > 0){
		AF_Entity* spotLightEntity = &_ecs->entities[_lightingData->spotLightEntityIndex];
		AF_CLight* spotLight = &_ecs->lights[_lightingData->spotLightEntityIndex];
		Vec3* spotLightPos = &spotLightEntity->transform->pos;
		
		glUniform3f(glGetUniformLocation(_shader, "spotLight.position"), spotLightPos->x, spotLightPos->y, spotLightPos->z);
		glUniform3f(glGetUniformLocation(_shader, "spotLight.direction"), spotLight->direction.x, spotLight->direction.y, spotLight->direction.z);
		glUniform3f(glGetUniformLocation(_shader, "spotLight.ambient"), spotLight->ambientCol.x, spotLight->ambientCol.y, spotLight->ambientCol.z);
		glUniform3f(glGetUniformLocation(_shader, "spotLight.diffuse"), spotLight->diffuseCol.x, spotLight->diffuseCol.y, spotLight->diffuseCol.z);
		glUniform3f(glGetUniformLocation(_shader, "spotLight.specular"), spotLight->specularCol.x, spotLight->specularCol.y, spotLight->specularCol.z);
		glUniform1f(glGetUniformLocation(_shader, "spotLight.constant"), spotLight->constant); 
		glUniform1f(glGetUniformLocation(_shader, "spotLight.linear"), spotLight->linear);
		glUniform1f(glGetUniformLocation(_shader, "spotLight.quadratic"), spotLight->quadratic);
		glUniform1f(glGetUniformLocation(_shader, "spotLight.cutoff"), AF_Math_Cos(AF_Math_Radians(spotLight->cutOff)));
		glUniform1f(glGetUniformLocation(_shader, "spotLight.outerCutOff"), AF_Math_Cos(AF_Math_Radians(spotLight->outerCutoff)));
	}
	
	// if we have a found point lights, for each found light
	for(uint8_t i = 0; i < _lightingData->pointLightsFound; i++){
		// Point Light
		uint16_t pointLightEntityIndex = _lightingData->pointLightIndexArray[i];
		AF_CLight* light = &_ecs->lights[pointLightEntityIndex];
		char posUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(posUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].position", i);
		Vec3* lightPosition = &_ecs->entities[pointLightEntityIndex].transform->pos;
		glUniform3f(glGetUniformLocation(_shader, posUniformName), lightPosition->x, lightPosition->y, lightPosition->z);
		
		// Ambient
		char ambientUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(ambientUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].ambient", i);
		glUniform3f(glGetUniformLocation(_shader, ambientUniformName), light->ambientCol.x, light->ambientCol.y, light->ambientCol.z);
		
		// Diffuse
		char pointUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(pointUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].diffuse", i);
		glUniform3f(glGetUniformLocation(_shader, pointUniformName), light->diffuseCol.x, light->diffuseCol.y, light->diffuseCol.z); 
		
		// Specular
		char specUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(specUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].specular", i);
		glUniform3f(glGetUniformLocation(_shader, specUniformName), light->specularCol.x, light->specularCol.y, light->specularCol.z);
		
		// Constant
		char constantUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(constantUniformName, MAX_PATH_CHAR_SIZE,"pointLights[%i].constant", i);
		GLint unformShaderName = glGetUniformLocation(_shader, constantUniformName);
		AF_Shader_SetFloat(_shader, constantUniformName, light->constant);
	
		// Linear
		char linearUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(linearUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].linear", i);
		glUniform1f(glGetUniformLocation(_shader, linearUniformName), light->linear);//0.09f); //_light->linear);//0.09f); 

		// Quadratic
		char quadraticUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(quadraticUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].quadratic", i);
		glUniform1f(glGetUniformLocation(_shader, quadraticUniformName), light->quadratic);//0.032f); //_light->quadratic);//0.032f); 
	}
	
}

/*
====================
AF_Renderer_UpdateLighting
Update lighting data by searching and storing the index's of active lights
Used in later render passes
====================
*/
void AF_Renderer_UpdateLighting(AF_ECS *_ecs, AF_LightingData *_lightingData)
{
	// clear the lighting data so we can re-count fresh
	_lightingData->ambientLightEntityIndex = 0;
	_lightingData->spotLightEntityIndex = 0;
	for(uint16_t x = 0; x < _lightingData->maxLights; x++){
		_lightingData->pointLightIndexArray[x] = 0;
	}
	_lightingData->pointLightsFound = 0;

	// search all lights in the entities
	// store the point lights, ambient, and spot light
	BOOL ambientLightFound = FALSE;
	BOOL spotLightfound = FALSE;
	BOOL allPointLightsFound = FALSE;
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		// early exit if we have found all available lights
		if(ambientLightFound == TRUE && spotLightfound == TRUE && allPointLightsFound == TRUE){
			break;
		}

		AF_Entity* entity = &_ecs->entities[i];
		// Only search enabled entities
		BOOL entityEnabled = AF_Component_GetEnabled(entity->flags);
		if(entityEnabled == FALSE){
			continue;
		}

		// search if this is a light component that is is enabled
		AF_CLight* light = &_ecs->lights[i];
		BOOL hasLightComponent = AF_Component_GetHas(light->enabled);
		BOOL hasLightComponentEnabled = AF_Component_GetEnabled(light->enabled);

		if(hasLightComponent == FALSE || hasLightComponentEnabled == FALSE){
			continue;
		}

		// What type of light
		if(light->lightType == AF_LIGHT_TYPE_AMBIENT){
			if(_lightingData->ambientLightEntityIndex > 0){
				AF_Log_Warning("AF_Renderer_UpdateLighting: Ambient Light already Set (entityIndex %i).  Can't set ambient light (entityIndex: %i) You are only allowed 1. Disable the others\n", _lightingData->ambientLightEntityIndex, i);
				continue;
			}
			_lightingData->ambientLightEntityIndex = i;
		}else if(light->lightType == AF_LIGHT_TYPE_POINT){
			if(_lightingData->pointLightsFound >= 4){
				AF_Log_Warning("AF_Renderer_UpdateLighting: Point Lights already Maxed out (entityIndex %i, %i, %i, %i). \
					Can't set point light (entityIndex: %i) You are only allowed 1. \
					Disable the others\n", \
					_lightingData->pointLightIndexArray[0], \
					_lightingData->pointLightIndexArray[1], \
					_lightingData->pointLightIndexArray[2], \
					_lightingData->pointLightIndexArray[3], i);
					continue;
			}
			_lightingData->pointLightIndexArray[_lightingData->pointLightsFound] = i;
			_lightingData->pointLightsFound++;
		}else if (light->lightType == AF_LIGHT_TYPE_SPOT){
			if(_lightingData->spotLightEntityIndex > 0){
				AF_Log_Warning("AF_Renderer_UpdateLighting: Spot Light already Set (entityIndex %i). Can't set spot light (entityIndex: %i) You are only allowed 1. Disable the others\n", _lightingData->spotLightEntityIndex, i);
				continue;
			}
			_lightingData->spotLightEntityIndex = i;
		}
	}
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
		BOOL hasMesh = AF_Component_GetHas(meshComponent->enabled);
		BOOL hasEnabled = AF_Component_GetEnabled(meshComponent->enabled);
		// Skip if there is no rendering component
		if(hasMesh == FALSE || hasEnabled == FALSE){
			continue;
		}
		// Destroy the mesh buffers
		
		// Destory the material textures
		for(uint32_t j = 0; j < meshComponent->meshCount; j++){
			AF_Renderer_Destroy_Material_Textures(&meshComponent->meshes[j].material);
		}
		
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
	glDeleteRenderbuffers(1, &_renderingData->screenRBO_ID);
	glDeleteFramebuffers(1, &_renderingData->screenFBO_ID);

	// Depth Buffers
	glDeleteFramebuffers(1, &_renderingData->depthFBO_ID);
	glDeleteFramebuffers(1, &_renderingData->depthRBO_ID);

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

		
			glDeleteVertexArrays(1, &mesh->ibo);
			glDeleteBuffers(1, &mesh->vbo);
			glDeleteBuffers(1, &mesh->ibo);
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
		AF_Log_Error("AF_Renderer_Start_ScreenFrameBuffers: ERROR::FRAMEBUFFER:: Framebuffer is not complete! Status: 0x%x (%s): %s\n", status, statusStr, _message);
	}
}


/*
====================
AF_Renderer_CheckError
Helper function for checking for GL errors
====================
*/
void AF_Renderer_CheckError(const char* _message){    
    GLenum errorCode = GL_NO_ERROR;
    errorCode = glGetError();
    const char* errorMessage = "";
    if(errorMessage){}
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
	switch (errorCode)
        {
            case GL_INVALID_ENUM:                  errorMessage  = invalidEnum; break;
            case GL_INVALID_VALUE:                 errorMessage  = invalidValue; break;
            case GL_INVALID_OPERATION:             errorMessage  = invalidOperation; break;
            case GL_STACK_OVERFLOW:                errorMessage  = stackOverflow; break;
            case GL_STACK_UNDERFLOW:               errorMessage  = stackUnderflow; break;
            case GL_OUT_OF_MEMORY:                 errorMessage  = outOfMemory; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: errorMessage  = invalidFrameBufferOperation; break;
        }
    AF_Log_Error(_message,errorMessage);

    }
           //printf("\nGL Error: %i\n", error);
}


/*
====================
AF_Renderer_SetPolygonMode
Set the polygon mode used
====================
*/
void AF_Renderer_SetPolygonMode(AF_Renderer_PolygonMode_e _polygonMode){
	switch(_polygonMode){
		case AF_RENDERER_POLYGON_MODE_FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
		case AF_RENDERER_POLYGON_MODE_POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
		case AF_RENDERER_POLYGON_MODE_LINE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;

	}
}

