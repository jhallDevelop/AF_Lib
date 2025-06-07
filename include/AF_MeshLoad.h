/*
===============================================================================
AF_MESHLOAD_H

Definition of Mesh Load functions that will be implemented using different mesh loading libraries like assimp
===============================================================================
*/
#ifndef AF_MESHLOAD_H
#define AF_MESHLOAD_H

#include "ECS/Components/AF_CMesh.h"
#include "AF_Assets.h"

// Publically accessible function to trigger further mesh loading implemented
af_bool_t AF_MeshLoad_Load(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* path);
uint32_t AF_MeshLoad_Shader_LoadFromAssets(AF_Assets& _assetsLoaded, const char* _vertPath, const char* _fragPath);

#endif
