/*
===============================================================================
AF_SHADER_H

Definitions for helper functions to load and use shaders 
===============================================================================
*/
#ifndef AF_SHADER_H
#define AF_SHADER_H
#include "AF_Util.h"
#include <stdint.h>
#include "AF_Rect.h"

#define MAX_SHADER_CHAR_PATH 128
#define DEFAULT_SHADER_DIR_PATH "./assets/shaders"
#define SHADER_FAILED_TO_LOAD 9999

#ifdef __cplusplus
extern "C" {    
#endif

typedef struct AF_Shader {
    uint32_t shaderID;
    char fragPath[MAX_SHADER_CHAR_PATH];
    char vertPath[MAX_SHADER_CHAR_PATH];
} AF_Shader;

/*
====================
AF_Shader_ZERO
Init shader struct
====================
*/
inline static AF_Shader AF_Shader_ZERO(void) {
    AF_Shader zeroShader;
    zeroShader.shaderID = 0;

    
    zeroShader.fragPath[0] = '\0'; // Proper null-terminated empty string
    zeroShader.vertPath[0] = '\0'; // Proper null-terminated empty string

    return zeroShader;
}

/*
====================
AF_Shader_CheckCompileErrors
Check glsl errors against opengl 
====================
*/
uint32_t AF_Shader_CheckCompileErrors(uint32_t shader, const char* type);

/*
====================
AF_Shader_GL_Load
Load a glsl shader from the fragement and vertex shader path
REturn the shader ID or return -1 if failed
====================
*/
uint32_t AF_Shader_Load(const char* _vertexShaderPath, const char* _fragmentShaderPath);

void AF_Shader_Delete(uint32_t programID);


// Helper Functions
void AF_Shader_SetBool(uint32_t ID, const char* name, bool value);
void AF_Shader_SetInt(uint32_t ID, const  char* name, int value);
void AF_Shader_SetFloat(uint32_t ID, const char* name, float value);
//void AF_Shader_SetVec2(uint32_t ID, const  char* name, const Vec2 value);
void AF_Shader_SetVec2(uint32_t ID, const  char* name, float x, float y);
//void AF_Shader_SetVec3(uint32_t ID, const  char* name, const Vec3 value);
void AF_Shader_SetVec3(uint32_t ID, const  char* name, float x, float y, float z);
//void AF_Shader_SetVec4(uint32_t ID, const char* name, const Vec4 value);
void AF_Shader_SetVec4(uint32_t ID, const  char* name, float x, float y, float z, float w);
void AF_Shader_SetMat4(uint32_t ID, const char* name, const Mat4 mat);
void AF_Shader_Use(uint32_t _id);
uint32_t AF_Shader_FreeCharBuffer(char* _buffer);

#ifdef __cplusplus
}
#endif

#endif //SHADER_H
