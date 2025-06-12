/*
===============================================================================
AF_OpenGL_Shader Implementation

This implementation is for OpenGL
===============================================================================
*/
#include "AF_Shader.h"
#include <string.h>
//#include <GL/glew.h>
#include "AF_File.h"
//#define GL_SILENCE_DEPRECATION

// set to 0 if ignoring shader errors or 1 if logging them
#define SHADER_ERROR_LOG 0

// Forward Declare
uint32_t AF_Shader_GetUniformLocation(uint32_t ID, const char* name);

/*
====================
AF_Shader_CheckCompileErrors
Check glsl errors against opengl
====================
*/
uint32_t AF_Shader_CheckCompileErrors(uint32_t shader, const char* type)
{
	AF_Log_Warning("AF_Shader_CheckCompileErrors: DX11 not implemented yet \n");
    return 0;
    /*
    // Check for shader compile errors
    GLint success = SHADER_FAILED_TO_LOAD;
    GLchar infoLog[1024];
    if (AF_STRING_IS_EMPTY(type)) {
        AF_Log_Error("AF_Shader: CheckCompileErrors given an empty type\n");
        return (uint32_t)success;
    }
    if (strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            AF_Log_Error("AF_Shader: SHADER_COMPILATION_ERROR of type: %s %s \n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            AF_Log_Error("AF_Shader: PROGRAM_LINKING_ERROR of type: %s %s \n", type, infoLog);
        }
    }
    return (uint32_t)success;
    */
}

uint32_t AF_Shader_FreeCharBuffer(char* _memoryBuffer) {
    if (_memoryBuffer != NULL) {
        free(_memoryBuffer);
        _memoryBuffer = NULL;
        return 0; // Success
    }
    AF_Log_Error("AF_Shader_FreeMemory: Memory buffer is NULL, nothing to free\n");
    return 1; // Failure
}

/*
====================
AF_Shader_GL_Load
Load a glsl shader from the fragement and vertex shader path
REturn the shader ID or return -1 if failed
====================
*/
uint32_t AF_Shader_Load(const char* _vertexShaderPath, const char* _fragmentShaderPath) {

    uint32_t returnShaderID = SHADER_FAILED_TO_LOAD;
	AF_Log_Warning("AF_Shader_Load: DX11 not implemented yet \n");
    return returnShaderID;

    /*
    // Check if shader paths are empty
    if (AF_STRING_IS_EMPTY(_vertexShaderPath) || AF_STRING_IS_EMPTY(_fragmentShaderPath)) {
        AF_Log_Error("AF_Shader: vertex or fragment shader path is empty\n");
        return returnShaderID;
    }

    // Check if shader is already loaded from the assets
    uint32_t vertShaderFileSize = AF_File_GetFileSize(_vertexShaderPath);
    if (vertShaderFileSize == 0) {
        AF_Log_Error("AF_Shader: vertex shader file size is 0, file not found or empty\n");
        return returnShaderID;
    }

    uint32_t fragShaderFileSize = AF_File_GetFileSize(_fragmentShaderPath);
    if (fragShaderFileSize == 0) {
        AF_Log_Error("AF_Shader: fragment shader file size is 0, file not found or empty\n");
        return returnShaderID;
    }

    // Allocate memory for the shader source code   
    char* vertexShaderSource = malloc(vertShaderFileSize + 1);
    if (vertexShaderSource == NULL) {
        AF_Log_Error("AF_Shader: Failed to allocate memory for vertex shader source\n");
        AF_Shader_FreeCharBuffer(vertexShaderSource);
        return returnShaderID;
    }
    char* framgentShaderSource = malloc(fragShaderFileSize + 1);
    if (framgentShaderSource == NULL) {
        AF_Log_Error("AF_Shader: Failed to allocate memory for fragment shader source\n");
        AF_Shader_FreeCharBuffer(framgentShaderSource);
        return returnShaderID;
    }

    // Read the shader source code from the files
    af_bool_t vertShaderLoadSuccess = AF_File_ReadFile(vertexShaderSource, vertShaderFileSize, _vertexShaderPath, "r");
    if (vertShaderLoadSuccess == AF_FALSE) {
        AF_Log_Error("AF_Shader: Failed to load vertex shader source from path: %s\n", _vertexShaderPath);
        AF_Shader_FreeCharBuffer(vertexShaderSource);
        AF_Shader_FreeCharBuffer(framgentShaderSource);
        return returnShaderID;
    }

    af_bool_t fragShaderLoadSuccess = AF_File_ReadFile(framgentShaderSource, fragShaderFileSize, _fragmentShaderPath, "r");
    if (fragShaderLoadSuccess == AF_FALSE) {
        AF_Log_Error("AF_Shader: Failed to load fragment shader source from path: %s\n", _fragmentShaderPath);
        AF_Shader_FreeCharBuffer(vertexShaderSource);
        AF_Shader_FreeCharBuffer(framgentShaderSource);
        return returnShaderID;
    }

    // Check for empty shader code
    if (AF_STRING_IS_EMPTY(vertexShaderSource) || AF_STRING_IS_EMPTY(framgentShaderSource)) {
        AF_Log_Error("AF_Shader: vertex or fragment shader source is empty \n");
        AF_Shader_FreeCharBuffer(vertexShaderSource);
        AF_Shader_FreeCharBuffer(framgentShaderSource);
        return returnShaderID;
    }

    // 2. compile shaders
    uint32_t vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* _vertexShaderSourceGL[] = { vertexShaderSource };
    const GLchar* _fragmentShaderSourceGL[] = { framgentShaderSource };

    glShaderSource(vertex, 1, _vertexShaderSourceGL, NULL);

    glCompileShader(vertex);
    AF_Shader_CheckCompileErrors((uint32_t)vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, _fragmentShaderSourceGL, NULL);
    glCompileShader(fragment);
    AF_Shader_CheckCompileErrors((uint32_t)fragment, "FRAGMENT");

    // shader Program
    returnShaderID = glCreateProgram();
    glAttachShader(returnShaderID, vertex);
    glAttachShader(returnShaderID, fragment);
    glLinkProgram(returnShaderID);
    AF_Shader_CheckCompileErrors((uint32_t)returnShaderID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (returnShaderID == SHADER_FAILED_TO_LOAD || returnShaderID == 0) {
        AF_Log_Error("AF_Shader: Loading shader failed\n");
        AF_Shader_FreeCharBuffer(vertexShaderSource);
        AF_Shader_FreeCharBuffer(framgentShaderSource);
        return returnShaderID;
    }

    // Free the allocated shader source code from the file as it lives on the graphics card now
    AF_Shader_FreeCharBuffer(vertexShaderSource);
    AF_Shader_FreeCharBuffer(framgentShaderSource);
    return returnShaderID;
    */
}



void AF_Shader_Delete(uint32_t programID) {

	AF_Log_Warning("AF_Shader_Delete: DX11 not implemented yet \n");
    /*
    if (programID != 0) { // 0 is not a valid shader program
        glUseProgram(0);  // Unbind the program if it is currently in use
        glDeleteProgram(programID);
        AF_Log("AF_Shader: Shader program with ID %u deleted\n", programID);
    }
    else {
        AF_Log_Error("AF_Shader_Delete: Invalid shader program ID (0)\n");
    }
    */
}


// utility uniform functions
// ------------------------------------------------------------------------
void AF_Shader_SetBool(uint32_t ID, const char* name, bool value)
{
	AF_Log_Warning("AF_Shader_SetBool: DX11 not implemented yet \n");
    //glUniform1i(AF_Shader_GetUniformLocation(ID, name), (int)value);
}
// ------------------------------------------------------------------------
void AF_Shader_SetInt(uint32_t ID, const  char* name, int value)
{
	AF_Log_Warning("AF_Shader_SetInt: DX11 not implemented yet \n");
    //glUniform1i(AF_Shader_GetUniformLocation(ID, name), value);
}
// ------------------------------------------------------------------------
void AF_Shader_SetFloat(uint32_t ID, const char* name, float value)
{
	AF_Log_Warning("AF_Shader_SetFloat: DX11 not implemented yet \n");
    //glUniform1f(AF_Shader_GetUniformLocation(ID, name), value);
}
// ------------------------------------------------------------------------

void AF_Shader_SetVec2(uint32_t ID, const  char* name, float x, float y)
{
	AF_Log_Warning("AF_Shader_SetVec2: DX11 not implemented yet \n");
    //glUniform2f(AF_Shader_GetUniformLocation(ID, name), x, y);
}
// ------------------------------------------------------------------------


void AF_Shader_SetVec3(uint32_t ID, const  char* name, float x, float y, float z)
{
	AF_Log_Warning("AF_Shader_SetVec3: DX11 not implemented yet \n");
    //glUniform3f(AF_Shader_GetUniformLocation(ID, name), x, y, z);
}
// ------------------------------------------------------------------------
void AF_Shader_SetVec4(uint32_t ID, const  char* name, float x, float y, float z, float w)
{
	AF_Log_Warning("AF_Shader_SetVec4: DX11 not implemented yet \n");
    //glUniform4f(AF_Shader_GetUniformLocation(ID, name), x, y, z, w);
}

// ------------------------------------------------------------------------
void AF_Shader_SetMat4(uint32_t ID, const char* name, const Mat4 mat)
{
	AF_Log_Warning("AF_Shader_SetMat4: DX11 not implemented yet \n");
    //glUniformMatrix4fv(AF_Shader_GetUniformLocation(ID, name), 1, GL_FALSE, &mat.rows->x);
}

// Get uniform Location
uint32_t AF_Shader_GetUniformLocation(uint32_t ID, const char* name) {
	AF_Log_Warning("AF_Shader_GetUniformLocation: DX11 not implemented yet \n");
	return -1; // Return -1 if not implemented yet
    /*
    GLint returnValue = glGetUniformLocation(ID, name);
    if (returnValue == -1 && SHADER_ERROR_LOG == 1) {
        AF_Log_Error("AF_Shader_GetUniformLocation: Can't find uniform location %s in shader %i\n", name, ID);
    }
    return returnValue;
    */
}

void AF_Shader_Use(uint32_t _id) {
	AF_Log_Warning("AF_Shader_Use: DX11 not implemented yet \n");
    //glUseProgram(_id);
}

