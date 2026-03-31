#include "AF_TestFramework.h"
#include "AF_Assets.h"
#include <string.h>

void Test_AddShader_Null(void){
    AF_Shader* result = AF_Assets_AddShader(NULL);
    AF_TEST_ASSERT(result == NULL, "AF_Assets_AddShader should return NULL for null assets");
}

void Test_AddShader_One(void){
    AF_Assets assets = AF_Assets_ZERO();
    AF_Shader* s = AF_Assets_AddShader(&assets);
    AF_TEST_ASSERT(s != NULL, "AF_Assets_AddShader returned NULL unexpectedly");
    AF_TEST_ASSERT(s == &assets.shaders[0], "First added shader should be slot 0");
    AF_TEST_EQUAL_INT(assets.nextAvailableShader, 1);
}

void Test_AddMesh_ReturnsRightSlot(void){
    AF_Assets assets = AF_Assets_ZERO();
    AF_MeshData m = AF_MeshData_ZERO();
    AF_MeshData* r = AF_Assets_AddMesh(&assets, m);
    AF_TEST_ASSERT(r != NULL, "AF_Assets_AddMesh should not return NULL" );
    AF_TEST_ASSERT(r == &assets.meshes[0], "AF_Assets_AddMesh must return mesh slot 0 for first mesh");
    AF_TEST_EQUAL_INT(assets.nextAvailableMesh, 1);
}

void Test_AddTexture_AddThenGet(void){
    AF_Assets assets = AF_Assets_ZERO();
    AF_Texture t = AF_Texture_ZERO();
    strncpy(t.path, "texture_path", AF_MAX_PATH_CHAR_SIZE-1);
    t.path[AF_MAX_PATH_CHAR_SIZE-1] = '\0';
    t.id = 42;

    AF_Assets_AddTexture(&assets, t);
    AF_TEST_EQUAL_INT(assets.nextAvailableTexture, 1);
    AF_TEST_ASSERT(strcmp(assets.textures[0].path, "texture_path") == 0, "Texture should be stored at index 0");

    AF_Texture got = AF_Assets_GetTexture(&assets, "texture_path");
    AF_TEST_EQUAL_INT(got.id, 42);
}

void Test_GetTexture_NotFound(void){
    AF_Texture got = AF_Assets_GetTexture(NULL, "missing");
    AF_TEST_EQUAL_INT(got.id, 0);
    AF_TEST_ASSERT(got.path[0] == '\0', "Expected empty texture when getting from NULL assets");
}

void Test_AddFont_Duplicates(void){
    AF_Assets assets = AF_Assets_ZERO();
    AF_Font f = AF_Font_Zero();
    strncpy(f.fontPath, "font.ttf", AF_MAX_PATH_CHAR_SIZE-1);
    f.fontPath[AF_MAX_PATH_CHAR_SIZE-1] = '\0';
    f.fontSize = 12;

    AF_Font* first = AF_Assets_AddFont(&assets, f);
    AF_TEST_ASSERT(first != NULL, "First add font should succeed");
    AF_Font* second = AF_Assets_AddFont(&assets, f);
    AF_TEST_ASSERT(second != NULL, "Second add font should return existing font");
    AF_TEST_ASSERT(first == second, "Duplicate font add should return same pointer");
    AF_TEST_EQUAL_INT(assets.nextAvailableFont, 1);
}

void Test_Boundaries(void){
    AF_Assets assets = AF_Assets_ZERO();

    for (uint32_t i = 0; i < AF_ASSETS_MAX_TEXTURES; i++) {
        AF_Texture t = AF_Texture_ZERO();
        snprintf(t.path, AF_MAX_PATH_CHAR_SIZE, "tex_%u", i);
        AF_Assets_AddTexture(&assets, t);
        AF_TEST_EQUAL_INT(assets.nextAvailableTexture, i + 1);
        AF_TEST_ASSERT(strcmp(assets.textures[i].path, t.path) == 0, "Texture should be in next available slot");
    }

    AF_Texture overflow = AF_Texture_ZERO();
    AF_Assets_AddTexture(&assets, overflow);
    AF_TEST_EQUAL_INT(assets.nextAvailableTexture, AF_ASSETS_MAX_TEXTURES);

    for (uint32_t i = 0; i < AF_ASSETS_MAX_MESHES; i++) {
        AF_MeshData m = AF_MeshData_ZERO();
        AF_MeshData* r = AF_Assets_AddMesh(&assets, m);
        AF_TEST_ASSERT(r != NULL, "Mesh add within boundary should succeed");
    }

    AF_MeshData mfail = AF_MeshData_ZERO();
    AF_MeshData* rm = AF_Assets_AddMesh(&assets, mfail);
    AF_TEST_ASSERT(rm == NULL, "Mesh add above boundary should return NULL");
}
