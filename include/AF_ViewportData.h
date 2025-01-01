#ifndef AF_VIEWPORTDATA_H
#define AF_VIEWPORTDATA_H
#include <stdint.h>
#include "ECS/Components/AF_CCamera.h"
#include "AF_Material.h"



typedef struct AF_ViewportData {
    uint16_t viewPortFramebufferWidth;
    uint16_t viewPortFramebufferHeight;
    uint32_t framebufferID;
    uint32_t textureID;
    uint32_t rbo;
    float backgroundColor[4];
    AF_CCamera camera;
    AF_Material gridlineMaterial;
    uint32_t gridlineVAO;
    uint32_t gridlineVBO;
} AF_ViewportData;

#endif