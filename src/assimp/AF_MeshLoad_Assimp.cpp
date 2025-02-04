#include "AF_MeshLoad.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AF_Util.h"

// Forward declare assimp specific definitions
void AF_MeshLoad_Assimp_ProcessMesh(AF_Assets* _assets, const char* _meshPath, AF_MeshData* _meshData, aiMesh* mesh, const aiScene* scene);
void AF_MeshLoad_Assimp_ProcessNode(AF_Assets* _assets, AF_CMesh* _meshComponent, uint32_t* _meshIndex, aiNode* node, const aiScene* scene);
AF_Texture* AF_MeshLoad_Assimp_LoadMaterialTextures(AF_Assets* _assets, const char* _meshPath, aiMaterial* _assimpMat, aiTextureType _assimpType);



void AF_MeshLoad_Component(AF_CMesh* _meshComponent, AF_Assets* _assets){
    /*
    if(AF_Util_FileExists(_meshComponent->meshPath) == FALSE){
        AF_Log_Warning("Editor_Component_LoadModel: ERROR: File doesn't exist $s\n", _meshComponent->meshPath);
        return;
    }

    AF_Log("Show Model File Browser \n");
    // delete the existing mesh data
    // save a copy of the mesh path, and shader as we still want to use that.
    // copy the mesh path
    char meshPath[MAX_PATH_CHAR_SIZE];
    snprintf(meshPath, sizeof(meshPath), "%s", _meshComponent->meshPath);

    // copy the shader to re-use it. 
    // save a copy of the shaders used
    char vertCopy[128];
    char fragCopy[128];
    snprintf(vertCopy,sizeof(vertCopy),"%s", _meshComponent->shader.vertPath);
    snprintf(fragCopy,sizeof(fragCopy),"%s", _meshComponent->shader.fragPath);

    // Blat the component. removing all memory
    AF_Log_Warning("Editor_Component_LoadModel: DISABLED destroying mesh, need to sync AF_Lib from home \n");
    //AF_Renderer_DestroyMeshBuffers(&_meshComponent);

    //Load the new model from path
    _meshComponent = Editor_Model_LoadAssimp(_assets, meshPath);

    // copy back the shader paths
    snprintf(_meshComponent->shader.vertPath,sizeof(_meshComponent->shader.vertPath),"%s", vertCopy);
    snprintf(_meshComponent->shader.fragPath,sizeof(_meshComponent->shader.fragPath),"%s", fragCopy);

    _meshComponent->shader.shaderID = Editor_Shader_LoadFromAssets(_assets, _meshComponent->shader.vertPath, _meshComponent->shader.fragPath);
    
    for(uint32_t i = 0; i < _meshComponent->meshCount; ++i){
        AF_MeshData* meshData = &_meshComponent->meshes[i];
        // Add material reference back
        _meshComponent->meshes[i].material.shaderID = _meshComponent->shader.shaderID;
        AF_Renderer_CreateMeshBuffer(&_meshComponent->meshes[i]);
    }
    */
}
void AF_MeshLoad_LoadModel(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* _modelPath){
    /*
    AF_Log("Editor_Model_LoadAssimp: Loading model from path: %s\n", _modelPath);
    if(std::strcmp(_modelPath, "") == 0){
        AF_Log_Error("Editor_Model_LoadAssimp: No model path provided\n");
        return;
    }
    // Add the mesh component first
    AF_CMesh returnMesh = AF_CMesh_ADD();
    // copy in the mesh path
    snprintf(returnMesh.meshPath, sizeof(returnMesh.meshPath), "%s", _modelPath);
    returnMesh.meshType = AF_MESH_TYPE_MESH;
    // Default is isImageFlipped = TRUE
    AF_Renderer_SetFlipImage(returnMesh.isImageFlipped);

    

    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_modelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        AF_Log_Error("Editor_Model: ERROR::ASSIMP:: %s" , importer.GetErrorString());
        return;
    }

    // Figure out how many sub modules there are and allocate memory for them
    uint32_t numMeshes = scene->mNumMeshes;
    if(numMeshes == 0){
        AF_Log_Error("Editor_Model: processNode: No meshes found in model\n");
        return;
    }
    // allocate memory for the mesh component
    _meshComponent->meshes = (AF_MeshData*)malloc(sizeof(AF_MeshData) * numMeshes);
    
    if(_meshComponent->meshes == NULL){
        AF_Log_Error("Editor_Model: processNode: FAILED to create mesh component\n");
        return;
    }
    // initialise the mesh data
    for(uint32_t i = 0; i < numMeshes; i++){
        _meshComponent->meshes[i] = AF_MeshData_ZERO();
    }
    _meshComponent->meshCount = numMeshes;
    // retrieve the directory path of the filepath
    // process ASSIMP's root node recursively
    // keep track of the mesh index
    uint32_t meshIndex = 0;
    AF_MeshLoad_Assimp_ProcessNode(_assets, _meshComponent, meshIndex, *scene->mRootNode, *scene);
    */
}

void AF_MeshLoad_Assimp_ProcessMesh(AF_Assets* _assets, const char* _meshPath, AF_MeshData* _meshData, aiMesh* mesh, const aiScene* scene){

}

void AF_MeshLoad_Assimp_ProcessNode(AF_Assets* _assets, AF_CMesh* _meshComponent, uint32_t* _meshIndex, aiNode* node, const aiScene* scene){

}

AF_Texture* AF_MeshLoad_Assimp_LoadMaterialTextures(AF_Assets* _assets, const char* _meshPath, aiMaterial* _assimpMat, aiTextureType _assimpType){

}
