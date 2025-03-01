/*
===============================================================================
AF_SCENE_H

Definitions for helper functions to load and use scenes 
===============================================================================
*/
#ifndef AF_SCENE_H
#define AF_SCENE_H

#include "AF_Renderer.h"
#include "AF_MeshLoad.h"
#include "AF_File.h"

#ifdef __cplusplus
extern "C" {    
#endif

inline static bool AF_Scene_Load(AF_ECS* _ecs, AF_Assets* _assets, const char* _filePath){
    // Open a file for binary reading
    FILE* file = AF_File_OpenFile(_filePath, "rb");
    if (file == NULL) {
        printf("Editor_FileBrowser_Render: FAILED to open file %s\n", _filePath);
        return false;
    } else {
        // Clean up the render objects first as some data is malloc
        for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
            BOOL hasMesh = AF_Component_GetHas(_ecs->entities[i].mesh->enabled);
            if(hasMesh == FALSE){
                continue;
            }
            AF_Renderer_DestroyMeshBuffers(_ecs->entities[i].mesh);
        }

        // Load ecs data from file
        AF_ECS_LoadECSFromBinary(file, _ecs);
        // Load Reload meshes
        // Todo: don't reference editor model this should be an AF function
        for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
            BOOL hasMesh = AF_Component_GetHas(_ecs->entities[i].mesh->enabled);
            if(hasMesh == FALSE){
                continue;
            }
            AF_MeshLoad_Load(_assets, _ecs->entities[i].mesh, _ecs->entities[i].mesh->meshPath);
            //Editor_Model_LoadModelComponent(*_editorAppData->ecs.entities[i].mesh, _editorAppData->assets);
        }
        return true;
    }
}

#ifdef __cplusplus
}
#endif

#endif