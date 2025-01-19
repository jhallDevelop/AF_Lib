#ifndef AF_VIEWPORTDATA_H
#define AF_VIEWPORTDATA_H
#include <stdint.h>
#include "ECS/Components/AF_CCamera.h"
#include "AF_Material.h"



typedef struct AF_ViewportData {
    uint16_t viewPortFramebufferWidth;
    uint16_t viewPortFramebufferHeight;
    uint32_t fbo;
    uint32_t textureID;
    uint32_t rbo;
    float backgroundColor[3];
    float mouseLookSensitivity;
    AF_Entity* cameraEntityPtr;
    AF_Material gridlineMaterial;
    uint32_t gridlineVAO;
    uint32_t gridlineVBO;
    BOOL isResized;
} AF_ViewportData;

inline static AF_ViewportData AF_ViewportData_ZERO(void){
    AF_Color color = {0,0,0,0};
	AF_Material returnMaterial = {
		.shaderID = 0,
		.textureID = 0, 
		.color = color
	};
    
    AF_ViewportData viewportData = {
    .viewPortFramebufferWidth = 0,
    .viewPortFramebufferHeight = 0,
    .fbo = 0,
    .textureID = 0,
    .rbo = 0,
    .backgroundColor = {0.0f, 0.0f, 0.0f},
    .mouseLookSensitivity = 0.25f,
    .cameraEntityPtr = NULL,
    .gridlineMaterial = returnMaterial,
    .gridlineVAO = 0,
    .gridlineVBO = 0,
    .isResized = TRUE
    };

    return viewportData;
}

#endif