#ifndef AF_VIEWPORTDATA_H
#define AF_VIEWPORTDATA_H
#include <stdint.h>
#include "ECS/Components/AF_CCamera.h"
#include "AF_Material.h"



typedef struct AF_ViewportData {
    AF_Window* windowPtr;
    //uint16_t viewPortPosX;
    //uint16_t viewPortPosY;
    //uint16_t viewPortFramebufferWidth;
    //uint16_t viewPortFramebufferHeight;
    //uint32_t fbo;
    //uint32_t textureID;
    //uint32_t rbo;
    float backgroundColor[3];
    float mouseLookSensitivity;
    AF_Entity* cameraEntityPtr;
    AF_Material gridlineMaterial;
    uint32_t gridlineVAO;
    uint32_t gridlineVBO;
    af_bool_t isResized;
    af_bool_t isRefreshed;
} AF_ViewportData;

inline static AF_ViewportData AF_ViewportData_ZERO(void) {
    AF_Color color = { 0,0,0,0 };
    AF_Material returnMaterial;
    returnMaterial.shaderID = 0;
    returnMaterial.diffuseTexture = { 0, AF_TEXTURE_TYPE_DIFFUSE, "\0" };
    returnMaterial.normalTexture = { 0, AF_TEXTURE_TYPE_NORMALS, "\0" };
    returnMaterial.specularTexture = { 0, AF_TEXTURE_TYPE_SPECULAR, "\0" };
    returnMaterial.color = color;

    AF_ViewportData viewportData;
    viewportData.windowPtr = NULL;
    //.viewPortPosX = 0,
    //.viewPortPosY = 0,
    //.viewPortFramebufferWidth = 0,
    //.viewPortFramebufferHeight = 0,
    //.fbo = 0,
    //.textureID = 0,
    //.rbo = 0,
    viewportData.backgroundColor[0] = 0.0f;
    viewportData.backgroundColor[1] = 0.0f;
    viewportData.backgroundColor[0] = 0.0f;
    viewportData.mouseLookSensitivity = 0.25f;
    viewportData.cameraEntityPtr = NULL;
    viewportData.gridlineMaterial = returnMaterial;
    viewportData.gridlineVAO = 0;
    viewportData.gridlineVBO = 0;
    viewportData.isResized = AF_TRUE;
    viewportData.isRefreshed = AF_FALSE;
    return viewportData;
}

#endif