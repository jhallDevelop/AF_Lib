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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
AF_Log_Mat4
Take a AF_Mat 4 and log it to the console.
====================
*/
void AF_Log_Mat4(Mat4 _mat4){
	AF_Log("		Row 1: %f %f %f %f\n\
		Row 2: %f %f %f %f\n\
		Row 3: %f %f %f %f\n\
		Row 4: %f %f %f %f\n\n",
		_mat4.rows[0].x, 
		_mat4.rows[0].y,
		_mat4.rows[0].z,
		_mat4.rows[0].w,

		_mat4.rows[1].x,
_mat4.rows[1].y,
		_mat4.rows[1].z,
		_mat4.rows[1].w,

		_mat4.rows[2].x,
		_mat4.rows[2].y,
		_mat4.rows[2].z,
		_mat4.rows[2].w,

		_mat4.rows[3].x,
		_mat4.rows[3].y,
		_mat4.rows[3].z,
		_mat4.rows[3].w);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int AF_Renderer_LoadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(TRUE);  

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
	// Use these filters for normal textures
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Use these filters for pixel art sprites.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(data);
    }
    else
    {
	AF_Log_Error("Texture failed to load at path %s\n",path);
        stbi_image_free(data);
    }

    return textureID;
}

void AF_Renderer_SetTexture(const uint32_t _shaderID, const char* _shaderVarName, uint32_t _textureID){
    glUseProgram(_shaderID); // Bind the shader program
    glUniform1i(glGetUniformLocation(_shaderID, _shaderVarName), _textureID); // Tell the shader to set the "Diffuse_Texture" variable to use texture id 0
    glUseProgram(0);
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
    
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    AF_CheckGLError( "Error initializing OpenGL! \n");
    //AF_CheckGLError("SLDGameRenderer::Initialise:: finishing up init: ");
    return success;
} 

/*
====================
AF_Renderer_Start
Start function which occurs after everything is loaded in.
====================
*/
void AF_Renderer_Start(AF_ECS* _ecs){
	AF_Log("AF_Renderer_Start\n");
	AF_Renderer_InitMeshBuffers(&_ecs->entities[0], _ecs->entitiesCount);
}

/*
====================
AF_Renderer_InitMeshBuffers
Init the mesh buffers for OpenGL
====================
*/
void AF_Renderer_InitMeshBuffers(AF_Entity* _entities, uint32_t _entityCount){ 

    if (_entityCount == 0) {
    AF_Log_Error("No meshes to draw!\n");
    return;
    }

    for(uint32_t i = 0; i < _entityCount; i++){
	    AF_CMesh* mesh = _entities[i].mesh;

	    
	    BOOL hasMesh = AF_Component_GetHas(mesh->enabled);
	    // Skip setting up if we don't have a mesh component
	    if(hasMesh == FALSE){
		continue;
	    }

		AF_CheckGLError( "Mesh has no indices!\n");

		// for each mesh
		for(uint32_t j = 0; j < mesh->meshCount; j++){
			AF_MeshData* meshData = &mesh->meshes[j];
			if (!meshData->vertices || !meshData->indices) {
			AF_Log_Error("Invalid vertex or index data!\n");
			return;
			}
			
			//int vertexBufferSize = _entityCount * (mesh->vertexCount * sizeof(AF_Vertex));
			int vertexBufferSize = meshData->vertexCount * sizeof(AF_Vertex);
			//AF_Log("Init GL Buffers for vertex buffer size of: %i\n",vertexBufferSize);
			AF_CheckGLError( "OpenGL error occurred during gVAO, gVBO, gEBO buffer creation.\n");
				
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
			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, meshData->vertices, GL_STATIC_DRAW);
			AF_CheckGLError( "OpenGL error occurred during glBufferData for the verts.\n");
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Bind the IBO and set the buffer data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indexCount * sizeof(unsigned int), &meshData->indices[0], GL_STATIC_DRAW);
			AF_CheckGLError( "OpenGL error occurred during glBufferData for the indexes.\n");

			// Stride is 8 floats wide, 3*pos, 3*normal, 2*tex
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)0);
			glEnableVertexAttribArray(0);
			
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AF_Vertex), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			AF_CheckGLError( "OpenGL error occurred during assignment of vertexAttribs.\n");

			// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0); 

			// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
			glBindVertexArray(0); 

			meshData->vao = gVAO;
			meshData->vbo = gVBO;
			meshData->ibo = gEBO;
			// Bind the IBO and set the buffer data
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
			//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, _meshList->meshes->indices, GL_STATIC_DRAW);
			AF_CheckGLError( "Error InitMesh Buffers for OpenGL! \n");
		}
    }
}

/*
====================
AF_Renderer_StartRendering
Do the initial setup for rendering opengl
====================
*/
void AF_Renderer_StartRendering(float _backgroundColor[3] ){
	AF_CheckGLError( "Error at start of Rendering OpenGL! \n");
    glClearColor(_backgroundColor[0], _backgroundColor[1],_backgroundColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable transparent blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // if in 2d mode, disable depth testing
    glDisable(GL_DEPTH_TEST);
}

/*
====================
AF_Renderer_DrawMeshes
Loop through the entities and draw the meshes that have components attached
====================
*/
void AF_Renderer_DrawMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs){
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_CMesh* mesh = &_ecs->meshes[i];
		BOOL meshHas = AF_Component_GetHas(mesh->enabled);
		BOOL hasEnabled = AF_Component_GetEnabled(mesh->enabled);
		// Skip if there is no rendering component
		if(meshHas == FALSE){// || hasEnabled == FALSE){
			continue;
		}
		AF_CTransform3D* modelTransform = &_ecs->transforms[i];
		AF_Renderer_DrawMesh(&modelTransform->modelMat, _viewMat, _projMat, mesh);
	}
}

/*
====================
AF_Renderer_DrawMesh
Loop through the meshes in a component and draw using opengl
====================
*/
void AF_Renderer_DrawMesh(Mat4* _modelMat, Mat4* _viewMat, Mat4* _projMat, AF_CMesh* _mesh){
	// draw meshes
	// TODO: this is very expensive. batch these up
	for(uint32_t i = 0; i < _mesh->meshCount; i++){
		glBindVertexArray(_mesh->meshes[i].vao);//_meshList->vao);
		AF_CheckGLError( "Error bind vao Rendering OpenGL! \n");

		// If you want to explicitly bind the VBO (usually not necessary if VBOs are part of the VAO):
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->meshes[i].vbo);
		AF_CheckGLError("Error binding VBO for drawing!");


		//---------------Send command to Graphics API to Draw Triangles------------
		// Setup shader
		uint32_t shader = _mesh->meshes[i].material.shaderID;
		glUseProgram(shader); 

		//AF_Log_Mat4(*_modelMat);
		// TODO: work in row major order so we don't have the gpy have to do the extra transpose
		// NOTE: GL_TRUE Indicates that the matrix you are passing to OpenGL is in row-major order 
		// and should be transposed to column-major order (the default for OpenGL).
		// Projection
		int projLocation = glGetUniformLocation(shader, "projection");
		glUniformMatrix4fv(projLocation, 1, GL_TRUE, (float*)&_projMat->rows);

		// View
		int viewLocation = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_TRUE, (float*)&_viewMat->rows);

		// Model
		int modelLocation = glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(modelLocation, 1, GL_TRUE, (float*)&_modelMat->rows);

		// Texture 
		//int textureUniformLocation = glGetUniformLocation(shaderID, "image");

		// send the camera data to the shader
		// Prep drawing
		unsigned int indexCount = _mesh->meshes[i].indexCount;
		if(indexCount == 0){
			AF_Log_Warning("AF_Renderer_DrawMesh: indexCount is 0. Can't draw elements\n");
			return;
		}

		//AF_Log_Error("AF_Renderer_DisplayRenderer: segfault here %s: %s\n", __FILE__, __LINE__);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
			
		AF_CheckGLError( "AF_Renderer_DrawMesh_Error drawElements Rendering OpenGL! \n");

		glBindVertexArray(0);
		AF_CheckGLError( "Error bindvertexarray(0) Rendering OpenGL! \n");
	}
	// Unbind textures
		//unbind diffuse
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, 0);
}



/*
====================
AF_LIB_DestroyRenderer
Destroy the renderer
====================
*/
void AF_Renderer_DestroyRenderer(AF_ECS* _ecs){
    AF_Log("%s Destroyed\n", openglRendererFileTitle);
    for(uint32_t i  = 0; i < _ecs->entitiesCount; i++){
		AF_CMesh* meshComponent = _ecs->entities[i].mesh;
		// for each mesh
		for(uint32_t j = 0; j < meshComponent->meshCount; j++){
			// optional: de-allocate all resources once they've outlived their purpose:
			// ------------------------------------------------------------------------
			AF_MeshData* mesh = &_ecs->entities[i].mesh->meshes[j];
			glDeleteVertexArrays(1, &mesh->ibo);
			glDeleteBuffers(1, &mesh->vbo);
			glDeleteBuffers(1, &mesh->ibo);
			glDeleteProgram(mesh->material.shaderID);
			glDeleteTextures(1, &mesh->material.textureID);

			// Free the vertices and indices
			free(mesh->vertices);
			mesh->vertices = NULL;
			free(mesh->indices);
			mesh->indices = NULL;
		}
		// Free the meshes in the component correctly
		free(meshComponent->meshes);
		meshComponent->meshes = NULL;
    }
       
        //glDeleteTexture(_meshList->materials[0].textureID);
    AF_CheckGLError( "Error Destroying Renderer OpenGL! \n");
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
