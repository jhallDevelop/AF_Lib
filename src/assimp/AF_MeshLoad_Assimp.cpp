#include "AF_MeshLoad.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AF_Renderer.h"
#include "AF_Util.h"

// Forward declare assimp specific definitions
BOOL AF_MeshLoad_Assimp(AF_Assets& _assets, AF_CMesh& _meshComponent, const char* path);
//void AF_MeshLoad_Component(AF_CMesh* _meshComponent, AF_Assets* _assets);
void AF_MeshLoad_Assimp_ProcessMesh(AF_Assets& _assets, const char* _meshPath, AF_MeshData& _meshData, aiMesh& mesh, const aiScene& scene);
void AF_MeshLoad_Assimp_ProcessNode(AF_Assets& _assets, AF_CMesh& _meshComponent, uint32_t& _meshIndex, aiNode& _node, const aiScene& _scene);
AF_Texture* AF_MeshLoad_Assimp_LoadMaterialTextures(AF_Assets& _assets, const char* _meshPath, aiMaterial& _assimpMat, aiTextureType _assimpType);
uint32_t AF_MeshLoad_Shader_LoadFromAssets(AF_Assets& _assetsLoaded, const char* _vertPath, const char* _fragPath);

/*
================
AF_MeshLoad_Load
Generic implementation that will call assimp specific function
================
*/
BOOL AF_MeshLoad_Load(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* _modelPath){
    if(AF_Util_FileExists(_modelPath) == FALSE){
        AF_Log_Error("AF_MeshLoad_Load: ERROR: File doesn't exist %s\n", _modelPath);
        return FALSE;
    }
    
    // clear the mesh data first incase there is left over junk
    //*_meshComponent = AF_CMesh_ZERO();

    // Call specific assimp varient of mesh loading
    BOOL isLoaded = AF_MeshLoad_Assimp(*_assets, *_meshComponent, _modelPath);
    if(isLoaded == FALSE){
        AF_Log_Error("AF_MeshLoad_Load: ERROR: Failed to load model from path %s\n", _modelPath);
        return FALSE;
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
    //_meshComponent = Editor_Model_LoadAssimp(_assets, meshPath);

    // copy back the shader paths
    snprintf(_meshComponent->shader.vertPath,sizeof(_meshComponent->shader.vertPath),"%s", vertCopy);
    snprintf(_meshComponent->shader.fragPath,sizeof(_meshComponent->shader.fragPath),"%s", fragCopy);

    _meshComponent->shader.shaderID = AF_MeshLoad_Shader_LoadFromAssets(*_assets, _meshComponent->shader.vertPath, _meshComponent->shader.fragPath);
    
    for(uint32_t i = 0; i < _meshComponent->meshCount; ++i){
        AF_MeshData* meshData = &_meshComponent->meshes[i];
        
        // Add material reference back
        _meshComponent->meshes[i].material.shaderID = _meshComponent->shader.shaderID;
        AF_Renderer_CreateMeshBuffer(&_meshComponent->meshes[i]);
    }
    // _meshComponent now has the relevent data
    // load the model into the gpu

    return TRUE;
}

/*
================
AF_MeshLoadAssimp
Unpack the model into an assimp format then process mesh nodes

================
*/
BOOL AF_MeshLoad_Assimp(AF_Assets& _assets, AF_CMesh& _meshComponent, const char* _modelPath){
    AF_Log("Editor_Model_LoadAssimp: Loading model from path: %s\n", _modelPath);
    if(std::strcmp(_modelPath, "") == 0){
        AF_Log_Error("Editor_Model_LoadAssimp: No model path provided\n");
        return FALSE;
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
        return FALSE;
    }

    // Figure out how many sub modules there are and allocate memory for them
    uint32_t numMeshes = scene->mNumMeshes;
    if(numMeshes == 0){
        AF_Log_Error("Editor_Model: processNode: No meshes found in model\n");
        return FALSE;
    }
    // allocate memory for the mesh component
    _meshComponent.meshes = (AF_MeshData*)malloc(sizeof(AF_MeshData) * numMeshes);
    
    if(_meshComponent.meshes == NULL){
        AF_Log_Error("Editor_Model: processNode: FAILED to create mesh component\n");
        return FALSE;
    }
    // initialise the mesh data
    for(uint32_t i = 0; i < numMeshes; i++){
        _meshComponent.meshes[i] = AF_MeshData_ZERO();
    }
    _meshComponent.meshCount = numMeshes;
    // retrieve the directory path of the filepath
    // process ASSIMP's root node recursively
    // keep track of the mesh index
    uint32_t meshIndex = 0;
    AF_MeshLoad_Assimp_ProcessNode(_assets, _meshComponent, meshIndex, *scene->mRootNode, *scene);
    return TRUE;
}


/*
================
AF_MeshLoad_Component

================
*/
/*
// TODO: this might be a duplicate function. 
void AF_MeshLoad_Component(AF_CMesh* _meshComponent, AF_Assets* _assets){
    
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
    
}*/

/*
================
AF_MeshLoad_Assimp_ProcessMesh
Process an assimp mesh

================
*/
void AF_MeshLoad_Assimp_ProcessMesh(AF_Assets& _assets, const char* _meshPath, AF_MeshData& _meshData, aiMesh& _mesh, const aiScene& _scene){
    // data to fill
    AF_Vertex* vertices = (AF_Vertex*)malloc(sizeof(AF_Vertex) * _mesh.mNumVertices);
    if(vertices == nullptr){
        AF_Log_Warning("Editor_Model: processMesh: FAILED to create vertices buffer\n");
        return;
    }

    //vector<unsigned int> indices;
    // count the number of indices, which is accessed per face
    uint32_t numVerticies =  _mesh.mNumVertices;
    uint32_t numIndicies = 0;
    for(unsigned int i = 0; i < _mesh.mNumFaces; i++)
    {
        aiFace face = _mesh.mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++){
            numIndicies++;
        }
    }
    uint32_t* indices = (uint32_t*)malloc(sizeof(uint32_t) * numIndicies);
    if(indices == nullptr){
        AF_Log_Warning("Editor_Model: processMesh: FAILED to create indices buffer\n");
        return;
    }

    //vector<Texture> textures;
    // Create space for 4 textures
    /*
    uint32_t* textures = (uint32_t*)malloc(sizeof(uint32_t) * 4);
    if(textures == nullptr){
        AF_Log_Warning("Editor_Model: processMesh: FAILED to create textures buffer\n");
        return;
    }*/

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < _mesh.mNumVertices; i++)
    {
        
        //Vertex vertex;
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        AF_Vertex vertex = AF_Vertex_ZERO();
        Vec3 vector = {0.0f, 0.0f, 0.0f};
        // positions
        vector.x = _mesh.mVertices[i].x;
        vector.y = _mesh.mVertices[i].y;
        vector.z = _mesh.mVertices[i].z;
        //vertex.Position = vector;
        vertex.position = vector;
        // normals
        if (_mesh.HasNormals())
        {
            vector.x = _mesh.mNormals[i].x;
            vector.y = _mesh.mNormals[i].y;
            vector.z = _mesh.mNormals[i].z;
            //vertex.Normal = vector;
            vertex.normal = vector;
        }
        // texture coordinates
        if(_mesh.mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            //glm::vec2 vec;
            Vec2 vec = {0.0f, 0.0f};
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = _mesh.mTextureCoords[0][i].x; 
            vec.y = _mesh.mTextureCoords[0][i].y;
            //vertex.TexCoords = vec;
            vertex.texCoord = vec;
            // tangent
            vector.x = _mesh.mTangents[i].x;
            vector.y = _mesh.mTangents[i].y;
            vector.z = _mesh.mTangents[i].z;
            //vertex.Tangent = vector;
            vertex.tangent = vector;
            // bitangent
            vector.x = _mesh.mBitangents[i].x;
            vector.y = _mesh.mBitangents[i].y;
            vector.z = _mesh.mBitangents[i].z;
            //vertex.Bitangent = vector;
            vertex.bitangent = vector;
        }
        else{
            vertex.texCoord = {0.0f, 0.0f};
        }
        vertices[i] = vertex;
        //vertices.push_back(vertex);
    }

    // update the mesh data vertices
    _meshData.vertexCount = numVerticies;
    _meshData.vertices = vertices;

    // TODO: move this to a function
    // global index count for all the mesh faces
    uint32_t indexCounter = 0;
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int k = 0; k < _mesh.mNumFaces; k++)
    {
        aiFace face = _mesh.mFaces[k];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int l = 0; l < face.mNumIndices; l++)
            indices[indexCounter++] = face.mIndices[l];
    }

    // update the mesh data indices
    _meshData.indexCount = numIndicies;
    _meshData.indices = indices;

    // process materials
    aiMaterial* assimpMaterial = _scene.mMaterials[_mesh.mMaterialIndex];    
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    
    //AF_Log("Editor_Model: loading DIFFUSE textures \n");
    
    
    _meshData.material.diffuseTexture = AF_MeshLoad_Assimp_LoadMaterialTextures(_assets, _meshPath, *assimpMaterial, aiTextureType_DIFFUSE);   

    // 2. specular maps
    //AF_Log("Editor_Model: loading Specular textures \n");
    _meshData.material.specularTexture =AF_MeshLoad_Assimp_LoadMaterialTextures(_assets, _meshPath, *assimpMaterial, aiTextureType_SPECULAR);  

    // 3. normal maps   
    //AF_Log("Editor_Model: loading Normal textures \n");
    _meshData.material.normalTexture =AF_MeshLoad_Assimp_LoadMaterialTextures(_assets, _meshPath, *assimpMaterial, aiTextureType_HEIGHT);  

    // 4. ambient maps
    // TODO: implement this
}

/*
================
AF_MeshLoad_Assimp_ProcessNode
Process an assimp node

================
*/
void AF_MeshLoad_Assimp_ProcessNode(AF_Assets& _assets, AF_CMesh& _meshComponent, uint32_t& _meshIndex, aiNode& _node, const aiScene& _scene){
     // process each mesh located at the current node
    for(unsigned int i = 0; i < _node.mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = _scene.mMeshes[_node.mMeshes[i]];
        
        // TODO: support multiple meshes
        AF_MeshLoad_Assimp_ProcessMesh(_assets, _meshComponent.meshPath, _meshComponent.meshes[_meshIndex], *mesh, _scene);
        // update the mesh index
        _meshIndex++;
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < _node.mNumChildren; i++)
    {
        AF_MeshLoad_Assimp_ProcessNode(_assets, _meshComponent, _meshIndex, *_node.mChildren[i], _scene);
    }
}

/*
================
AF_MeshLoad_Assimp_LoadMaterialTextures
Load the material and texture from the assimp materials

================
*/
AF_Texture* AF_MeshLoad_Assimp_LoadMaterialTextures(AF_Assets& _assets, const char* _meshPath, aiMaterial& _assimpMat, aiTextureType _assimpType){
    uint32_t assimpMaterialTextureCount = _assimpMat.GetTextureCount(_assimpType);
    if(assimpMaterialTextureCount == 0){
        AF_Log_Warning("Editor_LoadMaterialTextures: No textures found for type %i\n", _assimpType);
        return NULL;
    }   

    //AF_Texture* textures = (AF_Texture*)malloc(sizeof(AF_Texture) * textureCount);
    // prep the return texture pointer
    AF_Texture* returnTexturePtr = NULL;

    
    for(unsigned int i = 0; i < assimpMaterialTextureCount; i++)
    {
        aiString str;
        _assimpMat.GetTexture(_assimpType, i, &str);

        // check string length
        if(str.length == 0){
            AF_Log_Warning("Editor_LoadMaterialTextures: assimp Texture %i path is empty\n", i);
            continue;
        }

        // get a full texture path to use to compare
        std::string modelPathString = _meshPath;
        size_t lastSlash = modelPathString.find_last_of('/');
        std::string modelDirectorStr = modelPathString.substr(0, lastSlash);
        char assimpTexturePath[256];
        std::snprintf(assimpTexturePath, sizeof(assimpTexturePath), "%s/%s", modelDirectorStr.c_str(), str.C_Str());
            

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        // look at all the available textures loaded
        //for(unsigned int j = 0; j < _assets.nextAvailableTexture; j++)
        for(unsigned int j = 0; j < AF_ASSETS_MAX_TEXTURES; j++)
        {
            if(std::strcmp(_assets.textures[j].path, assimpTexturePath) == 0)
            {
                // if a texture with the same filepath is already loaded, use this texture data
                //textures.push_back(_assetsLoaded[j]);
                //AF_Log("Editor_LoadMaterialTextures: Texture with path %s already loaded\n", assimpTexturePath);
                returnTexturePtr = &_assets.textures[j];

                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            //Assets has a fixed array of textures that can be unlocked.
            // Add/unlock a new AF_Texture struct that can be used.
            AF_Texture* assetTexturePtr = AF_Assets_AddTexture(&_assets);
            if(assetTexturePtr == NULL){
                AF_Log_Warning("Editor_LoadMaterialTextures: Failed to add texture to assets\n");
                return NULL;
            }

            // Combine the model path, with the texture path
            // Textures are to be found at the same directory as the model
            // extract the path from the model path
            // e.g. model path will be ./assets/models/<modelname>/modelname.obj
            // texture path will be ./assets/models/<modelname>/<texturename>.png
            
            std::snprintf(assetTexturePtr->path, sizeof(assetTexturePtr->path), "%s/%s", modelDirectorStr.c_str(), str.C_Str());
            
            
            assetTexturePtr->id = AF_Renderer_LoadTexture(assetTexturePtr->path);

            // Map the assimp texture type to our texture type
            if(_assimpType == aiTextureType_DIFFUSE){
                assetTexturePtr->type = AF_TextureType::AF_TEXTURE_TYPE_DIFFUSE;
                AF_Log("Editor_LoadMaterialTextures: Texture type is DIFFUSE\n");
            } else if (_assimpType == aiTextureType_SPECULAR){
                assetTexturePtr->type = AF_TextureType::AF_TEXTURE_TYPE_SPECULAR;
                AF_Log("Editor_LoadMaterialTextures: Texture type is SPECULAR\n");
            } else if (_assimpType == aiTextureType_NORMALS){
                assetTexturePtr->type = AF_TextureType::AF_TEXTURE_TYPE_NORMALS;
                AF_Log("Editor_LoadMaterialTextures: Texture type is NORMALS\n");
            } else {
                assetTexturePtr->type = AF_TextureType::AF_TEXTURE_TYPE_NONE;
                AF_Log("Editor_LoadMaterialTextures: Texture type is NONE\n");
            }
                
            
            returnTexturePtr = assetTexturePtr;
        }
    }
    if(returnTexturePtr == NULL){
        AF_Log_Warning("Editor_LoadMaterialTextures: No textures loaded, something went wrong\n");
    }

    return returnTexturePtr;
}


/*====================
AF_MeshLoad_Shader_LoadFromAssets
Check the editor assets to see if the shader is already loaded
====================*/
uint32_t AF_MeshLoad_Shader_LoadFromAssets(AF_Assets& _assetsLoaded, const char* _vertPath, const char* _fragPath)
{
    uint32_t returnShaderID = SHADER_FAILED_TO_LOAD;
    // get a full texture path to use to compare
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    
    // look at all the available textures loaded, increment the condition so we at least check the first entry
    // Ensure _assetsLoaded.shaders is valid BEFORE using it
    
    
    for (unsigned int j = 0; j < AF_ASSETS_MAX_SHADERS; j++) {
        AF_Log("Checking: \n%s\n%s\n", _assetsLoaded.shaders[j].vertPath, _vertPath);

        // Compare the vertex shader path
        if (strcmp(_assetsLoaded.shaders[j].vertPath, _vertPath) == 0) {
            
            // Ensure the shader ID is valid before using it
            if (_assetsLoaded.shaders[j].shaderID == 0) {
                AF_Log_Warning("Warning: Shader ID at index %i is 0! Skipping.\n", j);
                continue;
            }

            AF_Log("AF_MeshLoad_Shader_LoadFromAssets: SUCCESS: Loaded existing shader: %i with path %s\n", _assetsLoaded.shaders[j].shaderID, _vertPath);
            returnShaderID = _assetsLoaded.shaders[j].shaderID;
            break;
        }
    }

    if(returnShaderID == SHADER_FAILED_TO_LOAD){
         // if texture hasn't been loaded already, load it
        //Assets has a fixed array of textures that can be unlocked.
        // Add/unlock a new AF_Texture struct that can be used.
        
        //add data to the next shader inline to be filled, and get a ptr to it 
        returnShaderID = AF_Shader_Load(_vertPath, _fragPath);
        if(returnShaderID == SHADER_FAILED_TO_LOAD){
                AF_Log_Error("AF_MeshLoad_Shader_LoadFromAssets: Shader failed to load with path %s \n", _vertPath);
                return returnShaderID;
        }

        AF_Shader* assetShaderPtr = AF_Assets_AddShader(&_assetsLoaded);
        if(assetShaderPtr == NULL){
            AF_Log_Error("AF_MeshLoad_Shader_LoadFromAssets: Failed to AF_Assets_AddShader(). Returned ptr is null\n");
            return SHADER_FAILED_TO_LOAD;
        }
        // save the data to the new shader added to assets
        assetShaderPtr->shaderID = returnShaderID;
        snprintf(assetShaderPtr->vertPath, MAX_SHADER_CHAR_PATH, "%s", _vertPath);
        snprintf(assetShaderPtr->fragPath, MAX_SHADER_CHAR_PATH, "%s", _fragPath);
        
        if(assetShaderPtr == nullptr){
            AF_Log_Error("AF_MeshLoad_Shader_LoadFromAssets: Failed to add shader, something went wrong\n");
            return SHADER_FAILED_TO_LOAD;
        }
        AF_Log("AF_MeshLoad_Shader_LoadFromAssets: SUCCESS: Loaded new shader from path %s \n",_vertPath);
    }
    
    return returnShaderID;
}