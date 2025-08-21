/*
===============================================================================
AF_LIGHTINGDATA_H defninitions

Implementation of lighting data struct 
===============================================================================
*/

#ifndef AF_LIGHTINGDATA_H
#define AF_LIGHTINGDATA_H

#include "AF_Lib_Define.h"
#include "AF_Math/AF_Mat4.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LIGHT_NUM 4

// Create a shadow data struct
typedef struct AF_ShadowData{
    af_bool_t shadowsEnabled;
    AF_FLOAT nearPlane;
	AF_FLOAT farPlane;
	float outerBounds;
    Vec3 lightPos;
    Vec3 lightTarget;
    Vec3 worldUp;
    Mat4 shadowLightSpaceMatrix;
    uint32_t shadowCameraID;
} AF_ShadowData;

// Function to create a zero-initialized shadow data struct
static inline AF_ShadowData AF_ShadowData_ZERO(void) {
    AF_ShadowData shadowData;
    shadowData.shadowsEnabled = AF_TRUE;
    shadowData.nearPlane = 1.0f;
    shadowData.farPlane = 7.5f;
    shadowData.outerBounds = 10.0f;
    
    // Initialize light position, target, and world up
    Vec3 defaultLightPos = { -2.0f, 4.0f, -1.0f };
    shadowData.lightPos = defaultLightPos;

    Vec3 defaultLightTarget = { 0.0f, 0.0f, 0.0f };
    shadowData.lightTarget = defaultLightTarget;

    Vec3 defaultWorldUp = { 0.0f, 1.0f, 0.0f };
    shadowData.worldUp = defaultWorldUp;
    shadowData.shadowLightSpaceMatrix = Mat4_ZERO();
    shadowData.shadowCameraID = 0;
    return shadowData;
}

typedef struct AF_LightingData{
	// TODO pack this
    af_bool_t enabled;
    //af_bool_t shadowsEnabled;
    uint32_t maxLights; 
    uint8_t ambientColor[3];
    // Counting up the lights
    uint16_t ambientLightEntityIndex;
    uint16_t spotLightEntityIndex;
    uint16_t pointLightIndexArray[MAX_LIGHT_NUM];
    uint8_t pointLightsFound;
    //Mat4 shadowLightSpaceMatrix;
    AF_ShadowData shadowData; // Shadow data for the renderer
} AF_LightingData;

static inline AF_LightingData AF_LightingData_ZERO(void){
    AF_LightingData returnLightingData;
    returnLightingData.enabled = AF_TRUE;
    //returnLightingData.shadowsEnabled = AF_TRUE;
    returnLightingData.maxLights = MAX_LIGHT_NUM;
    returnLightingData.ambientColor[0] = 255;
    returnLightingData.ambientColor[1] = 255;
    returnLightingData.ambientColor[2] = 255;
    returnLightingData.ambientLightEntityIndex = 0;
    returnLightingData.spotLightEntityIndex = 0;
    returnLightingData.pointLightIndexArray[0] = 0;
    returnLightingData.pointLightIndexArray[1] = 0;
    returnLightingData.pointLightIndexArray[2] = 0;
    returnLightingData.pointLightIndexArray[3] = 0;
    returnLightingData.pointLightsFound = 0;
    //returnLightingData.shadowLightSpaceMatrix = Mat4_ZERO();
    returnLightingData.shadowData = AF_ShadowData_ZERO();
	return returnLightingData;
}



#ifdef __cplusplus
}
#endif

#endif  // AF_LIGHTINGDATA_H
