#include "AF_Assets.h"

/*
====================
AF_Assets_AddShader
Implementation for adding a shader to the assets
returns a pointer to the shader added in the assets/shader array
====================
*/
AF_Shader* AF_Assets_AddShader(AF_Assets* _assets){
    if(_assets == NULL){
        AF_Log_Warning("AF_Assets_AddShader: passed null assets\n");
        return NULL;
    }
    //AF_Log("next available shader: %i MAX: %i \n",_assets->nextAvailableShader, AF_ASSETS_MAX_SHADERS);
    if(_assets->nextAvailableShader < AF_ASSETS_MAX_SHADERS){
        _assets->nextAvailableShader++;
        return &_assets->shaders[_assets->nextAvailableShader - 1];
    }else{
        AF_Log_Warning("AF_Assets_AddShader: Max shaders reached\n");
        return NULL;
    }
}

/*
====================
AF_Assets_AddTexture
Implementation for adding a texture to the assets
returns a pointer to the texture added in the assets/texture array
====================
*/
void AF_Assets_AddTexture(AF_Assets* _assets, AF_Texture _texture){
    if(_assets->nextAvailableTexture < AF_ASSETS_MAX_TEXTURES){
        _assets->nextAvailableTexture++;
        _assets->textures[_assets->nextAvailableTexture] = _texture;
    }else{
        AF_Log_Warning("AF_Assets_AddTexture: Max textures reached\n");
    }
}

/*
====================
AF_Assets_GetTexture
Implementation for getting a texture to the assets
returns a pointer to the texture added in the assets/texture array
====================
*/
AF_Texture AF_Assets_GetTexture(AF_Assets* _assets, const char* _texturePath){
    if(_assets == NULL){
        AF_Log_Error("AF_Assets_AddTexture: passed null assets\n");
    }
    
    AF_Texture returnTexture = AF_Texture_ZERO();
    for(unsigned int j = 0; j < AF_ASSETS_MAX_TEXTURES; j++)
    {
        if(strncmp(_assets->textures[j].path, _texturePath, AF_MAX_PATH_CHAR_SIZE) == 0)
        {
            //AF_Log("AF_Assets_GetTexture: Found existing texture %s | %s\n", _texturePath, _assets->textures[j].path);
            // if a texture with the same filepath is already loaded, use this texture data
            //AF_Log("AF_Assets_GetTexture: Found texture %s in assets: path: %s ID: %i\n", _texturePath, _assets->textures[j].path, returnTexturePtr->id);
            returnTexture = _assets->textures[j];
            // TODO: set correct type of texture
            //returnTexture.type = AF_TEXTURE_TYPE_DIFFUSE;
            break;
        }
    }

    
    return returnTexture;
}

/*
====================
AF_Assets_AddMesh
Implementation for adding a mesh to the assets
returns a pointer to the mesh data added in the assets/meshdata array
====================
*/
AF_MeshData* AF_Assets_AddMesh(AF_Assets* _assets, AF_MeshData _mesh){
    if(_assets == NULL){
        AF_Log_Warning("AF_Assets_AddMesh: passed null assets\n");
        return NULL;
    }
    if(_assets->nextAvailableMesh < AF_ASSETS_MAX_MESHES){
        _assets->meshes[_assets->nextAvailableMesh] = _mesh;
        _assets->nextAvailableMesh++;
        return &_assets->meshes[_assets->nextAvailableTexture - 1];
    }else{
        AF_Log_Warning("AF_Assets_AddMesh: Max meshes reached\n");
        return NULL;
    }
}

/*
=========================
AF_Assets_AddFont
Implementation for adding a font to the assets
returns a pointer to the font added in the assets/font array
=========================
*/
AF_Font* AF_Assets_AddFont(AF_Assets *_assets, AF_Font _font)
{
    if (_assets == NULL)
    {
        AF_Log_Warning("AF_Assets_AddFont: passed null assets\n");
        return NULL;
    }

    // check if there is already this font loaded
    for (uint32_t i = 0; i < _assets->nextAvailableFont; i++)
    {
        if (strncmp(_assets->fonts[i].fontPath, _font.fontPath, AF_MAX_PATH_CHAR_SIZE) == 0 && _assets->fonts[i].fontSize == _font.fontSize)
        {
            // font already loaded, return existing font
            return &_assets->fonts[i];
        }
    }

    // add new font
    if (_assets->nextAvailableFont < AF_ASSETS_MAX_FONTS)
    {
        _assets->fonts[_assets->nextAvailableFont] = _font;
        _assets->nextAvailableFont++;
        return &_assets->fonts[_assets->nextAvailableFont - 1];
    }
    else
    {
        AF_Log_Warning("AF_Assets_AddFont: Max fonts reached\n");
        return NULL;
    }
}
