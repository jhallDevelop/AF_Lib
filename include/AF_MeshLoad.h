/*
===============================================================================
AF_MESHLOAD_H

Definition of Mesh Load functions that will be implemented using different mesh loading libraries like assimp
===============================================================================
*/
#ifndef AF_MESHLOAD_H
#define AF_MESHLOAD_H

#include "ECS/Components/AF_CMesh.h"
#include "ECS/Components/AF_CText.h"
#include "AF_Assets.h"
#include "AF_Lib_Define.h"
#include "AF_Lib_API.h"
#ifdef __cplusplus
extern "C" {
#endif

// Publically accessible function to trigger further mesh loading implemented
AF_LIB_API af_bool_t AF_MeshLoad_Load(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* path);
AF_LIB_API af_bool_t AF_MeshLoad_InitMesh(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* _modelPath);
AF_LIB_API af_bool_t AF_MeshLoad_InitFontMesh(AF_Assets* _assets, AF_CText* _fontComponent, const char* _fontPath, float _fontSize);
AF_LIB_API uint32_t AF_MeshLoad_Shader_LoadFromAssets(AF_Assets* _assetsLoaded, const char* _vertPath, const char* _fragPath);
AF_LIB_API af_bool_t AF_MeshLoad_FromFile(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* _modelPath);

#ifdef __cplusplus
}
#endif
#endif
