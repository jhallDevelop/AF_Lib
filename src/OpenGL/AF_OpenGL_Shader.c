/*
===============================================================================
AF_OpenGL_Shader Implementation

This implementation is for OpenGL
===============================================================================
*/
#include "AF_Shader.h"
#include <string.h>
#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION

// Forward Declare
GLint AF_Shader_GetUniformLocation(uint32_t ID, const char* name);

/*
====================
AF_Shader_CheckCompileErrors
Check glsl errors against opengl 
====================
*/
uint32_t AF_Shader_CheckCompileErrors(uint32_t shader, const char* type)
{

    // Check for shader compile errors
    GLint success = SHADER_FAILED_TO_LOAD;
    GLchar infoLog[1024];
    if(AF_STRING_IS_EMPTY(type)){
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
}


/*
====================
AF_Shader_GL_Load
Load a glsl shader from the fragement and vertex shader path
REturn the shader ID or return -1 if failed
====================
*/
uint32_t AF_Shader_Load(const char* _vertexShaderPath, const char* _fragmentShaderPath){

    uint32_t returnShaderID = SHADER_FAILED_TO_LOAD;
    
    // Check if shader paths are empty
    if(AF_STRING_IS_EMPTY(_vertexShaderPath) || AF_STRING_IS_EMPTY(_fragmentShaderPath)){
        AF_Log_Error("AF_Shader: vertex or fragment shader path is empty\n");
        return returnShaderID;
    }

    // Check if shader is already loaded from the assets
    
    
    char* _vertexShaderSource = AF_Util_ReadFile(_vertexShaderPath);
    char* _fragmentShaderSource = AF_Util_ReadFile(_fragmentShaderPath);

    // Null check the loaded shader code
    if(_vertexShaderSource == NULL || _fragmentShaderSource == NULL){
        AF_Log_Error("AF_Shader: vertex or fragment shader source is null \n");
        return returnShaderID;
    }
    // Check for empty shader code
    if(AF_STRING_IS_EMPTY(_vertexShaderSource) || AF_STRING_IS_EMPTY(_fragmentShaderSource)){
        AF_Log_Error("AF_Shader: vertex or fragment shader source is empty \n");
        return returnShaderID;
    }
    
    // 2. compile shaders
    uint32_t vertex, fragment;
    
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    
    const GLchar* _vertexShaderSourceGL[] = {_vertexShaderSource};
    const GLchar* _fragmentShaderSourceGL[] = {_fragmentShaderSource};
    
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

    if(returnShaderID == SHADER_FAILED_TO_LOAD || returnShaderID == 0){
        AF_Log_Error("AF_Shader: Loading shader failed\n");
        returnShaderID = SHADER_FAILED_TO_LOAD;
    }
    
    // Free the allocated shader source code from the file as it lives on the graphics card now
    free(_vertexShaderSource);
    _vertexShaderSource = NULL;
    free(_fragmentShaderSource);
    _fragmentShaderSource = NULL;

    return returnShaderID;
}

void AF_Shader_Delete(uint32_t programID) {
    if (programID != 0) { // 0 is not a valid shader program
        glUseProgram(0);  // Unbind the program if it is currently in use
        glDeleteProgram(programID);
        AF_Log("AF_Shader: Shader program with ID %u deleted\n", programID);
    } else {
        AF_Log_Error("AF_Shader_Delete: Invalid shader program ID (0)\n");
    }
}


// utility uniform functions
// ------------------------------------------------------------------------
void AF_Shader_SetBool(uint32_t ID, const char* name, bool value) 
{   
    glUniform1i(AF_Shader_GetUniformLocation(ID, name), (int)value); 
}
// ------------------------------------------------------------------------
void AF_Shader_SetInt(uint32_t ID, const  char* name, int value) 
{ 
    //glUseProgram(ID);
    glUniform1i(AF_Shader_GetUniformLocation(ID, name), value); 
    //glUseProgram(0);
}
// ------------------------------------------------------------------------
void AF_Shader_SetFloat(uint32_t ID, const char* name, float value) 
{ 
    glUniform1f(AF_Shader_GetUniformLocation(ID, name), value); 
}
// ------------------------------------------------------------------------
/*
void AF_Shader_SetVec2(uint32_t ID, const  char* name, const Vec2 value) 
{ 
    glUniform2fv(AF_Shader_GetUniformLocation(ID, name), 1, &value.x); 
}*/
void AF_Shader_SetVec2(uint32_t ID, const  char* name, float x, float y) 
{ 
    glUniform2f(AF_Shader_GetUniformLocation(ID, name), x, y); 
}
// ------------------------------------------------------------------------
/*
void AF_Shader_SetVec3(uint32_t ID, const  char* name, const Vec3 value) 
{ 
    glUniform3fv(AF_Shader_GetUniformLocation(ID, name), 1, &value.x); 
}*/

void AF_Shader_SetVec3(uint32_t ID, const  char* name, float x, float y, float z) 
{ 
    glUniform3f(AF_Shader_GetUniformLocation(ID, name), x, y, z); 
}
// ------------------------------------------------------------------------
/*
void AF_Shader_SetVec4(uint32_t ID, const char* name, const Vec4 value) 
{ 
    glUniform4fv(AF_Shader_GetUniformLocation(ID, name), 1, &value.x); 
}*/


void AF_Shader_SetVec4(uint32_t ID, const  char* name, float x, float y, float z, float w) 
{ 
    glUniform4f(AF_Shader_GetUniformLocation(ID, name), x, y, z, w); 
}
/*
// ------------------------------------------------------------------------
void AF_Shader_SetMat2(uint32_t ID, const char* name, const glm::mat2 &mat) 
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}*/
/*
// ------------------------------------------------------------------------
void AF_Shader_SetMat3(uint32_t ID, const char* name, const glm::mat3 &mat) 
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}
    */
// ------------------------------------------------------------------------
void AF_Shader_SetMat4(uint32_t ID, const char* name, const Mat4 mat) 
{
    //AF_Shader_Use(ID); 
    glUniformMatrix4fv(AF_Shader_GetUniformLocation(ID, name), 1, GL_FALSE, &mat.rows->x);
    //AF_Shader_Use(0);
}

// Get uniform Location
GLint AF_Shader_GetUniformLocation(uint32_t ID, const char* name){
    GLint returnValue = glGetUniformLocation(ID, name);
    if(returnValue == -1){
        AF_Log_Error("AF_Shader_GetUniformLocation: Can't find uniform location %s in shader %i\n", name, ID);
    }
    return returnValue;
}

void AF_Shader_Use(uint32_t _id){
    glUseProgram(_id); 
}

