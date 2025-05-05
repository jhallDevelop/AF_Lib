/*
===============================================================================
AF_OpenGL_Renderer Implementation

Implementation of the AF_LIB rendering functions
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


// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
// TODO: delete this
/*
float vertices2[] = {
    0.5f,  0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};

AF_Vertex vertices[4] = {
    { {0.5f,  0.5f, 0.0f}, {0.5f,  0.5f, 0.0f}, {0.5f,  0.5f} },
    { {0.5f, -0.5f, 0.0f}, {0.5f,  0.5f, 0.0f}, {0.5f,  0.5f} },
    { {-0.5f, -0.5f, 0.0f}, {0.5f,  0.5f, 0.0f}, {0.5f,  0.5f} },
    { {-0.5f,  0.5f, 0.0f}, {0.5f,  0.5f, 0.0f}, {0.5f,  0.5f} },
	
};*/

unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
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
AF_CheckGLError
Helper function for checking for GL errors
====================
*/
void AF_CheckGLError(const char* _message){    
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


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int AF_Renderer_LoadTexture(char const * path)
{
	
    unsigned int textureID;

	
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    //stbi_set_flip_vertically_on_load(TRUE);  
	
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1){
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4){
            format = GL_RGBA;
        }
            
		
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
    }
    else
    {
	AF_Log_Error("AF_Renderer_LoadTexture: Texture failed to load at path %s\n",path);
    }

	// free the loaded image
	stbi_image_free(data);
	data = NULL;
    return textureID;
}

void AF_Renderer_SetTexture(const uint32_t _shaderID, const char* _shaderVarName, uint32_t _textureID){
    glUseProgram(_shaderID); // Bind the shader program
    glUniform1i(glGetUniformLocation(_shaderID, _shaderVarName), _textureID); // Tell the shader to set the "Diffuse_Texture" variable to use texture id 0
    glUseProgram(0);
}

AF_Texture AF_Renderer_ReLoadTexture(AF_Assets* _assets, const char* _texturePath){
	AF_Texture returnTexture = AF_Assets_GetTexture(_assets, _texturePath);
	if(returnTexture.type == AF_TEXTURE_TYPE_NONE){
		AF_Log("RenderMeshSection: Loading new texture\n");
		// create a new texture struct stored in assets

		// update the path of this new texture
		snprintf(returnTexture.path, MAX_PATH_CHAR_SIZE, "%s", _texturePath);

		// Update the texture id
		returnTexture.id = AF_Renderer_LoadTexture(_texturePath);
		// update the typed
		returnTexture.type = AF_TEXTURE_TYPE_DIFFUSE;
		AF_Assets_AddTexture(_assets, returnTexture);
	}
	// swap the old texture ptr that this mesh links to with the new texture.
	return returnTexture;
}

/*
====================
AF_LIB_Awake
Init OpenGL
====================
*/
uint32_t AF_Renderer_Awake(void){
    uint32_t success = 1;
    AF_Log("AF_Renderer_Awake\n");
    //Initialize GLEW
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    AF_CheckGLError( "Error initializing GLEW! \n");

    //Use Vsync
    /*
    if( SDL_GL_SetSwapInterval( 1 ) < 0 )
    {
        AF_Log_Error( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
        success = 0;
    }*/

    //Initialize OpenGL
    
    //Initialize clear color
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
    
    /**/

    //set the glViewport and the perspective
    //glViewport(_window->windowXPos, _window->windowYPos, _window->windowWidth, _window->windowHeight);

        // configure global opengl state
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
	
    AF_CheckGLError( "Error initializing OpenGL! \n");
    //AF_CheckGLError("SLDGameRenderer::Initialise:: finishing up init: ");
    return success;
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
void AF_Renderer_Render(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_Entity* _cameraEntity, uint32_t _frameBufferWidth, uint32_t _frameBufferHeight){

	// START RENDERING
	
	AF_CheckGLError( "AF_Renderer_Render: Error at start of Rendering OpenGL setting color and clearing screen! \n");

	// Update lighting data
	AF_Renderer_UpdateLighting(_ecs, _lightingData);


	// Switch Between Renderer
	switch(_renderingData->rendererType)
	{
		// FORWARD RENDERING
		case AF_RENDERER_FORWARD:
			AF_Renderer_StartForwardRendering(_ecs, _renderingData, _lightingData, _cameraEntity, _frameBufferWidth, _frameBufferHeight);
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

/*
====================
AF_Renderer_StartForwardRendering
Simple render command to perform forward rendering steps
====================
*/
void AF_Renderer_StartForwardRendering(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_Entity* _cameraEntity, uint32_t _frameBufferWidth, uint32_t _frameBufferHeight){

	// ==== DEPTH PASS ====
	// Draw Depth Pass
	AF_Renderer_StartDepthPass(_renderingData, _lightingData, _ecs, _cameraEntity->camera);
	

	// Draw objects to depth 
	// TODO: send the render pass type into draw meshes, as well as a shared shader
	// e.g. if its a depth pass, then send a flag so other render steps are ignored, also send the depth shader
	//AF_Renderer_DrawMeshes(&_cameraEntity->camera->viewMatrix, &_cameraEntity->camera->projectionMatrix, _ecs, &_cameraEntity->transform->pos, _lightingData);
	// End Depth pass
	//AF_Renderer_EndDepthPass(_frameBufferWidth, _frameBufferHeight);

	// Draw Color Pass
	
	// ==== COLOR PASS ====
	AF_Renderer_DrawMeshes(&_cameraEntity->camera->viewMatrix, &_cameraEntity->camera->projectionMatrix, _ecs, &_cameraEntity->transform->pos, _lightingData);
	

	// ==== LIGHTING PASS ====

}

void AF_Renderer_RenderDepthMeshes(AF_ECS* _ecs){
	
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



/*
====================
AF_Renderer_Start
Start function which occurs after everything is loaded in.
====================
*/
void AF_Renderer_Start( AF_RenderingData* _renderingData){
	AF_Log("AF_Renderer_Start\n");
	if(_renderingData == NULL){}
	//AF_Renderer_InitMeshBuffers(&_ecs->entities[0], _ecs->entitiesCount);

	// ==== Setup Depth Map and Texture ====
	// Create Depth Frame Buffer and texture
	_renderingData->depthFrameBufferID = AF_Renderer_CreateDepthMapFBO();
	_renderingData->depthMapTextureID = AF_Renderer_CreateDepthMapTexture();
	// Bind the frame buffer
	AF_Renderer_BindDepthFrameBuffer(_renderingData->depthFrameBufferID, _renderingData->depthMapTextureID);

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
	AF_Shader_SetInt(_renderingData->depthDebugShaderID, "depthMap", _renderingData->depthMapTextureID);
    glUseProgram(0);
	
	
}

// ================= HELPER FUNCTIONS =====================

/*
====================
AF_Renderer_CreateDepthMapFBO
Create Depth map frame buffer object
return framebuffer index uint32_t
====================
*/
uint32_t AF_Renderer_CreateDepthMapFBO(void){
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	return depthMapFBO;
}

/*
====================
AF_Renderer_CreateDepthMapTexture
Create Depth map texture and return the texture id
====================
*/
uint32_t AF_Renderer_CreateDepthMapTexture(void){
	
	unsigned int depthMapTextureID;
	glGenTextures(1, &depthMapTextureID);
	glBindTexture(GL_TEXTURE_2D, depthMapTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
		AF_RENDERINGDATA_SHADOW_WIDTH, AF_RENDERINGDATA_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

	return depthMapTextureID;
}

/*
====================
AF_Renderer_BindDepthFrameBuffer
Bind the depth FBO and Texture to the framebuffer command on the gpu
====================
*/
void AF_Renderer_BindDepthFrameBuffer(uint32_t _depthMapFBOID, uint32_t _depthMapTextureID){
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBOID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMapTextureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  
	
}

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

		AF_CheckGLError( "Mesh has no indices!\n");

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
	AF_CheckGLError( "OpenGL error occurred just before gVAO, gVBO, gEBO buffer creation.\n");
		
	GLuint gVAO, gVBO, gEBO;
	glGenVertexArrays(1, &gVAO);
	glGenBuffers(1, &gVBO);
	glGenBuffers(1, &gEBO);
	AF_CheckGLError( "OpenGL error occurred during gVAO, gVBO, gEBO buffer creation.\n");

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s)
	glBindVertexArray(gVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	AF_CheckGLError( "OpenGL error occurred during binding of the gVAO, gVBO.\n");

	// our buffer needs to be 8 floats (3*pos, 3*normal, 2*tex)
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, _meshData->vertices, GL_STATIC_DRAW);
	AF_CheckGLError( "OpenGL error occurred during glBufferData for the verts.\n");
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the IBO and set the buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _meshData->indexCount * sizeof(uint32_t), &_meshData->indices[0], GL_STATIC_DRAW);
	AF_CheckGLError( "OpenGL error occurred during glBufferData for the indexes.\n");

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

	AF_CheckGLError( "OpenGL error occurred during assignment of vertexAttribs.\n");

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
	AF_CheckGLError( "Error InitMesh Buffers for OpenGL! \n");
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
	Vec3 viewPos =  {0,2,0};	// 0
	lightView = Mat4_Lookat(targetPos, viewPos, upDirection); //glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = Mat4_MULT_M4(lightProjection, lightView);

	// render scene from light's point of view
	glUseProgram(_renderingData->depthRenderShaderID); 
	AF_Shader_SetMat4(_renderingData->depthRenderShaderID, "lightSpaceMatrix", lightSpaceMatrix);
	
	
	glViewport(0, 0, AF_RENDERINGDATA_SHADOW_WIDTH, AF_RENDERINGDATA_SHADOW_HEIGHT);	
	//glBindFramebuffer(GL_FRAMEBUFFER, _renderingData->depthFrameBufferID);//depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		AF_Render_DrawMeshElements(_ecs, &lightProjection, &viewPos, _renderingData->depthRenderShaderID);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	// Reset Viewport
	glViewport(0, 0, _camera->windowWidth, _camera->windowHeight);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

/*
====================
AF_Render_DrawMeshElements
Simple DrawElements call on all ECS mesh components
====================
*/
void AF_Render_DrawMeshElements(AF_ECS* _ecs, Mat4* _lightProjection, Vec3* _viewPos, uint32_t _shaderID){
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
		uint32_t shader = mesh->shader.shaderID;
		glUseProgram(shader); 
		for(uint32_t x = 0; x < mesh->meshCount; x++){
			
			glUniform3f(glGetUniformLocation(shader, "viewPos"), _viewPos->x, _viewPos->y, _viewPos->z);//_cameraPos->x, _cameraPos->y, _cameraPos->z);//_viewMat->rows[3].x, _viewMat->rows[3].y, _viewMat->rows[3].z); 
			

			glBindVertexArray(mesh->meshes[x].vao);//_meshList->vao);
			AF_CheckGLError( "Error bind vao Rendering OpenGL! \n");

			// If you want to explicitly bind the VBO (usually not necessary if VBOs are part of the VAO):
			glBindBuffer(GL_ARRAY_BUFFER, mesh->meshes[x].vbo);
			AF_CheckGLError("Error binding VBO for drawing!");


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
			glUniformMatrix4fv(projLocation, 1, GL_TRUE, (float*)&_lightProjection->rows);

			// View
			//AF_Log("==== View Matrix ====\n");
			//AF_Util_Mat4_Log(*_viewMat);
			int viewLocation = glGetUniformLocation(shader, "view");
			glUniformMatrix4fv(viewLocation, 1, GL_TRUE, (float*)&_lightProjection->rows);

			// Model
			//AF_Log("==== Model Matrix ====\n");
			//AF_Util_Mat4_Log(*_modelMat);
			int modelLocation = glGetUniformLocation(shader, "model");
			glUniformMatrix4fv(modelLocation, 1, GL_TRUE, (float*)&_lightProjection->rows);

			//AF_Log("==== ------------------ ====\n");

			// Texture 
			//int textureUniformLocation = glGetUniformLocation(shaderID, "image");

			// send the camera data to the shader
			// Prep drawing
			unsigned int indexCount = mesh->meshes[x].indexCount;
			if(indexCount == 0){
				AF_Log_Warning("AF_Renderer_DrawMesh: indexCount is 0. Can't draw elements\n");
				return;
			}

			// TODO: sort transparent objects before rendering
			//https://learnopengl.com/Advanced-OpenGL/Blending

			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
				
			AF_CheckGLError( "AF_Renderer_DrawMesh_Error drawElements Rendering OpenGL! \n");

			glBindVertexArray(0);
			AF_CheckGLError( "Error bindvertexarray(0) Rendering OpenGL! \n");
		}

	}
}

/*
====================
AF_Renderer_EndDepthPass
Do the final cleanup for rendering a depth pass in opengl
====================
*/
void AF_Renderer_EndDepthPass(uint32_t _screenWidth, uint32_t _screenHeight){
	// reset viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _screenWidth, _screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/**/
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
	
	BOOL hasMesh = AF_Component_GetHas(_mesh->enabled);
	BOOL isEnabled = AF_Component_GetEnabled(_mesh->enabled);
	// don't render if we are not enabled or the component isn't supposed to render
	if(hasMesh == FALSE || isEnabled == FALSE){
		return;;
	}
	// TODO: this is very expensive. batch these up
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
			}
		}


		// Does the shader use lighting?
        //if(_mesh->recieveLights == TRUE){
		// TODO: confirm if the camera position is stored in column or row major order of the viewMat
		glUniform3f(glGetUniformLocation(shader, "viewPos"), _cameraPos->x, _cameraPos->y, _cameraPos->z);//_viewMat->rows[3].x, _viewMat->rows[3].y, _viewMat->rows[3].z); 
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
		AF_CheckGLError( "Error bind vao Rendering OpenGL! \n");

		// If you want to explicitly bind the VBO (usually not necessary if VBOs are part of the VAO):
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->meshes[i].vbo);
		AF_CheckGLError("Error binding VBO for drawing!");


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
			
		AF_CheckGLError( "AF_Renderer_DrawMesh_Error drawElements Rendering OpenGL! \n");

		glBindVertexArray(0);
		AF_CheckGLError( "Error bindvertexarray(0) Rendering OpenGL! \n");
		
	}
	//glUseProgram(0);
	// Unbind textures
	//unbind diffuse
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

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
		//AF_Log("Point Light: %i (entityIndex: %i)\n", i, _lightingData->pointLightIndexArray[i]);
		uint16_t pointLightEntityIndex = _lightingData->pointLightIndexArray[i];
		AF_CLight* light = &_ecs->lights[pointLightEntityIndex];
		char posUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(posUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].position", i);
		Vec3* lightPosition = &_ecs->entities[pointLightEntityIndex].transform->pos;
		glUniform3f(glGetUniformLocation(_shader, posUniformName), lightPosition->x, lightPosition->y, lightPosition->z);//-0.2f, 1.0f, -0.3f);//_lightPos->x, _lightPos->y, _lightPos->z);//-0.2f, 1.0f, -0.3f);
		
		// Ambient
		char ambientUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(ambientUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].ambient", i);
		glUniform3f(glGetUniformLocation(_shader, ambientUniformName), light->ambientCol.x, light->ambientCol.y, light->ambientCol.z);//0.05f, 0.05f, 0.05f); //_light->ambientCol.x, _light->ambientCol.y, _light->ambientCol.z);//0.05f, 0.05f, 0.05f);  
		
		// Diffuse
		char pointUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(pointUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].diffuse", i);
		glUniform3f(glGetUniformLocation(_shader, pointUniformName), light->diffuseCol.x, light->diffuseCol.y, light->diffuseCol.z);//0.4f, 0.4f, 0.4f); //_light->diffuseCol.x, _light->diffuseCol.y, _light->diffuseCol.z); //0.4f, 0.4f, 0.4f); 
		
		// Specular
		char specUniformName[MAX_PATH_CHAR_SIZE];
		snprintf(specUniformName, MAX_PATH_CHAR_SIZE, "pointLights[%i].specular", i);
		glUniform3f(glGetUniformLocation(_shader, specUniformName), light->specularCol.x, light->specularCol.y, light->specularCol.z);////0.5f, 0.5f, 0.5f); //_light->specularCol.x, _light->specularCol.y, _light->specularCol.z);//0.5f, 0.5f, 0.5f); 
		
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
AF_LIB_DestroyRenderer
Destroy the renderer
====================
*/
void AF_Renderer_DestroyRenderer(AF_ECS* _ecs){
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
		AF_Renderer_Destroy_Shader(meshComponent->shader.shaderID);

		// Destroy the mesh buffers
		AF_Renderer_DestroyMeshBuffers(meshComponent);

		// zero the component
		*meshComponent = AF_CMesh_ZERO();
    }

	// Delete textures

	// Delete Shaders
       
        //glDeleteTexture(_meshList->materials[0].textureID);
    AF_CheckGLError( "Error Destroying Renderer OpenGL! \n");
}

void AF_Renderer_Destroy_Shader(uint32_t _shaderID){
	glDeleteProgram(_shaderID);
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
		glDeleteTextures(1, &_material->diffuseTexture.id);
	}

	// Specular
	if(_material->specularTexture.type != AF_TEXTURE_TYPE_NONE){
		glDeleteTextures(1, &_material->specularTexture.id);
	}

	// Normal
	if(_material->normalTexture.type != AF_TEXTURE_TYPE_NONE){
		glDeleteTextures(1, &_material->normalTexture.id);
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
			// optional: de-allocate all resources once they've outlived their purpose:
			// ------------------------------------------------------------------------
			AF_MeshData* mesh = &_mesh->meshes[j];
			if(mesh == NULL){
				AF_Log_Warning("AF_Renderer_DestroyMeshBuffers: skipping destroy of mesh %i\n", j);
				continue;
			}

		
			glDeleteVertexArrays(1, &mesh->ibo);
			glDeleteBuffers(1, &mesh->vbo);
			glDeleteBuffers(1, &mesh->ibo);
			//glDeleteProgram(mesh->material.shaderID);

			
			// Free the vertices and indices
			//free(mesh->vertices);
			//mesh->vertices = NULL;
			//free(mesh->indices);
			//mesh->indices = NULL;
		}
		// Free the meshes in the component correctly
		//free(_mesh->meshes);
		//_mesh->meshes = NULL;
}


///////////////////////////// 2D RENDERING //////////////////////////////
/*
		// if we have a sprite, then render it.
		AF_CSprite* sprite = entity->sprite;
		BOOL hasSprite = AF_Component_GetHas(sprite->enabled);
		BOOL enabledSprite = AF_Component_GetEnabled(sprite->enabled);

		GLfloat spriteSize[2] = {
				sprite->spriteSize.x, /// sprite->spriteSheetSize.x, 
				sprite->spriteSize.y// / sprite->spriteSheetSize.y
				};

		if(hasSprite == TRUE && enabledSprite == TRUE){
			// TODO: pack all this info about the sprite into a single data struct 
			// so talking to the shader is cheaper
			// Tell the shader about the sprite size
			int spriteSizeLocation = glGetUniformLocation(shaderID, "spriteSize");
			// Tell the shader about the sprite position
			if (spriteSizeLocation != -1) {
				glUniform2fv(spriteSizeLocation, 1, spriteSize);
			}

			// Tell the shader about the sprite sheet size
				int spriteSheetSizeLocation = glGetUniformLocation(shaderID, "spriteSheetSize");
			if(spriteSheetSizeLocation != -1){
				GLfloat spriteSheetSize[2] = {
					sprite->spriteSheetSize.x,
					sprite->spriteSheetSize.y
				};
				glUniform2fv(spriteSheetSizeLocation, 1, spriteSheetSize);
			}
			

			// if we have a Animation, then render it.
			AF_CAnimation* animation = entity->animation;
			BOOL hasAnimation = AF_Component_GetHas(animation->enabled);
			BOOL enabledAnimation = AF_Component_GetEnabled(animation->enabled);
			// setup a vec2 to update the sprite position		
			GLfloat spritePos[2] = {
				sprite->spritePos.x,
				sprite->spritePos.y
				};
			
			// if we have animation component, then update the spritePosition 
			// based on the current frame.
			if(hasAnimation == TRUE && enabledAnimation == TRUE){
			// Update the sprite position
				spritePos[0] = spritePos[0] + (spriteSize[0] * animation->currentFrame);
			}
			// Tell the shader about the updated sprite position
				int spritePosLocation = glGetUniformLocation(shaderID, "spritePos");
				if(spritePosLocation != -1){
					glUniform2fv(spritePosLocation, 1, spritePos);
			}
		
			// Set the texture for the shader
			glUniform1i(textureUniformLocation,0);// _meshList->materials[0].textureID); 
			glActiveTexture(GL_TEXTURE0);
			// TODO implement binding the actual texture
			// unsigned int diffuseTexture = _mesh.material.diffuseTexture;
			glBindTexture(GL_TEXTURE_2D, mesh->material.textureID);
			AF_CheckGLError("Error blBindTexture diffuse ");
		}

		// if we have a sprite, but it isn't enabled then don't contine
		if(hasSprite == TRUE && enabledSprite == FALSE){
			continue;
		}
		*/
