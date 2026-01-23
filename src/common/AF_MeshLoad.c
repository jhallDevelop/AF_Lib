#include "AF_MeshLoad.h"
#include "AF_Renderer.h"
#include "AF_RendererBuffer.h"
//#include "AF_Util.h"
//#include "AF_File.h"

/*
====================
AF_MeshLoad_InitMesh
Initialize a mesh component by loading the model from file
====================
*/
af_bool_t AF_MeshLoad_InitMesh(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* _modelPath){
    if(_assets == NULL || _meshComponent == NULL || _modelPath == NULL){
        AF_Log_Error("AF_MeshLoad_Init: NULL pointer provided\n");
        return AF_FALSE;
    }
    
    //AF_Log("AF_MeshLoad_Load: Show Model File Browser \n");
    // delete the existing mesh data
    // save a copy of the mesh path, and shader as we still want to use that.
    // copy the mesh path
    char meshPath[AF_MAX_PATH_CHAR_SIZE];
    snprintf(meshPath, sizeof(meshPath), "%s", _meshComponent->meshPath);

    // copy the shader to re-use it. 
    // save a copy of the shaders used
    char vertCopy[128];
    char fragCopy[128];
    snprintf(vertCopy,sizeof(vertCopy),"%s", _meshComponent->shader.vertPath);
    snprintf(fragCopy,sizeof(fragCopy),"%s", _meshComponent->shader.fragPath);

    // Blat the component. removing all memory
    //AF_Log_Warning("AF_MeshLoad_Load: DISABLED destroying mesh, need to sync AF_Lib from home \n");

    //Load the new model from path

    // copy back the shader paths
    snprintf(_meshComponent->shader.vertPath,sizeof(_meshComponent->shader.vertPath),"%s", vertCopy);
    snprintf(_meshComponent->shader.fragPath,sizeof(_meshComponent->shader.fragPath),"%s", fragCopy);

    _meshComponent->shader.shaderID = AF_MeshLoad_Shader_LoadFromAssets(_assets, _meshComponent->shader.vertPath, _meshComponent->shader.fragPath);
    // Add material reference back
    _meshComponent->material.shaderID = _meshComponent->shader.shaderID;
    for(uint32_t i = 0; i < _meshComponent->meshCount; ++i){
        AF_MeshData* meshData = &_meshComponent->meshes[i];
        AF_RendererBuffer_CreateMeshBuffer(&_meshComponent->meshes[i]);
    }

    return AF_TRUE;
}


/*
====================
AF_MeshLoad_InitFontMesh
Initialize a font component by loading the font from file
====================
*/
af_bool_t AF_MeshLoad_InitTextMesh(AF_Assets* _assets, AF_CText* _fontComponent, const char* _fontPath, float _fontSize){
    if (_assets == NULL || _fontComponent == NULL || _fontPath == NULL || _fontSize <= 0) {
        AF_Log_Error("AF_MeshLoad_Init: NULL pointer provided\n");
        return AF_FALSE;
    }
    
    // The shader ID is already loaded and set by AF_Project_SyncEntities.
    // This function's only responsibility is to initialize the OpenGL buffers.
    // We can remove the redundant shader loading logic.

    // setup the font mesh data
    AF_RendererBuffer_InitTextMeshBuffers(_fontComponent);

    return AF_TRUE;
}



/*====================
AF_MeshLoad_Shader_LoadFromAssets
Check the editor assets to see if the shader is already loaded
====================*/
uint32_t AF_MeshLoad_Shader_LoadFromAssets(AF_Assets* _assetsLoaded, const char* _vertPath, const char* _fragPath)
{
    if(AF_STRING_IS_EMPTY(_vertPath) || AF_STRING_IS_EMPTY(_fragPath)) {
        AF_Log_Warning("AF_MeshLoad_Shader_LoadFromAssets: shader path is empty\n");
        return SHADER_FAILED_TO_LOAD;
    }

    uint32_t returnShaderID = SHADER_FAILED_TO_LOAD;
    // get a full texture path to use to compare
    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    
    // look at all the available textures loaded, increment the condition so we at least check the first entry
    // Ensure _assetsLoaded.shaders is valid BEFORE using it
    
    
    for (unsigned int j = 0; j < AF_ASSETS_MAX_SHADERS; j++) {
        //AF_Log("Checking: \n%s\n%s\n", _assetsLoaded.shaders[j].vertPath, _vertPath);

        // Compare the vertex shader path
        if (strcmp(_assetsLoaded->shaders[j].vertPath, _vertPath) == 0) {
            
            // Ensure the shader ID is valid before using it
            if (_assetsLoaded->shaders[j].shaderID == 0) {
                AF_Log_Warning("Warning: Shader ID at index %i is 0! Skipping.\n", j);
                continue;
            }

            //AF_Log("AF_MeshLoad_Shader_LoadFromAssets: SUCCESS: Loaded existing shader: %i with path %s\n", _assetsLoaded.shaders[j].shaderID, _vertPath);
            returnShaderID = _assetsLoaded->shaders[j].shaderID;
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

        AF_Shader* assetShaderPtr = AF_Assets_AddShader(_assetsLoaded);
        if(assetShaderPtr == NULL){
            AF_Log_Error("AF_MeshLoad_Shader_LoadFromAssets: Failed to AF_Assets_AddShader(). Returned ptr is null\n");
            return SHADER_FAILED_TO_LOAD;
        }
        // save the data to the new shader added to assets
        assetShaderPtr->shaderID = returnShaderID;
        snprintf(assetShaderPtr->vertPath, MAX_SHADER_CHAR_PATH, "%s", _vertPath);
        snprintf(assetShaderPtr->fragPath, MAX_SHADER_CHAR_PATH, "%s", _fragPath);
        
        if(assetShaderPtr == NULL){
            AF_Log_Error("AF_MeshLoad_Shader_LoadFromAssets: Failed to add shader, something went wrong\n");
            return SHADER_FAILED_TO_LOAD;
        }
        //AF_Log("AF_MeshLoad_Shader_LoadFromAssets: SUCCESS: Loaded new shader from path %s \n",_vertPath);
    }
    
    return returnShaderID;
}
