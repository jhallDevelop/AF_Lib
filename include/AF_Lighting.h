/*
===============================================================================
AF_Lighting Header

Lighting system for managing and updating light data
===============================================================================
*/
#ifndef AF_LIGHTING_H
#define AF_LIGHTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AF_Lib_Define.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_LightingData.h"

// =================================================================================================
// Function Declarations
// =================================================================================================

// Render forward point lights to shader uniforms
void AF_Lighting_RenderForwardPointLights(uint32_t _shader, AF_ECS* _ecs, AF_LightingData* _lightingData);

// Update lighting data by searching and storing active lights
void AF_Lighting_UpdateLighting(AF_ECS *_ecs, AF_LightingData *_lightingData);

#ifdef __cplusplus
}
#endif

#endif // AF_LIGHTING_H
