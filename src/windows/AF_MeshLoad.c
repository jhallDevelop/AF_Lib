#include "AF_MeshLoad.h"

// Publically accessible function to trigger further mesh loading implemented
af_bool_t AF_MeshLoad_Load(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* path) {
	AF_Log_Warning("AF_MeshLoad_Load: is not implemented yet. Please implement it in AF_MeshLoad.c");
	if(_assets == NULL || _meshComponent == NULL || path == NULL) {
		AF_Log_Warning("AF_MeshLoad_Load: Invalid parameters passed.");
		return AF_FALSE;
	}
	return AF_FALSE;
}

uint32_t AF_MeshLoad_Shader_LoadFromAssets(AF_Assets* _assetsLoaded, const char* _vertPath, const char* _fragPath) {
	AF_Log_Warning("AF_MeshLoad_Shader_LoadFromAssets: is not implemented yet. Please implement it in AF_MeshLoad.c");
	if(_assetsLoaded == NULL || _vertPath == NULL || _fragPath == NULL){

	}
	return AF_FALSE;
}
