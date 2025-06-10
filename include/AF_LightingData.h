/*
===============================================================================
AF_LIGHTINGDATA_H defninitions

Implementation of lighting data struct 
===============================================================================
*/

#ifndef AF_LIGHTINGDATA_H
#define AF_LIGHTINGDATA_H

#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LIGHT_NUM 4

typedef struct AF_LightingData{
	// TODO pack this
    af_bool_t enabled;
    af_bool_t shadowsEnabled;
    uint32_t maxLights; 
    uint8_t ambientColor[3];
    // Counting up the lights
    uint16_t ambientLightEntityIndex;
    uint16_t spotLightEntityIndex;
    uint16_t pointLightIndexArray[MAX_LIGHT_NUM];
    uint8_t pointLightsFound;
    Mat4 shadowLightSpaceMatrix;
} AF_LightingData;

static inline AF_LightingData AF_LightingData_ZERO(void){
    AF_LightingData returnLightingData;
    returnLightingData.enabled = AF_TRUE;
    returnLightingData.shadowsEnabled = AF_TRUE;
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
    returnLightingData.shadowLightSpaceMatrix = Mat4_ZERO();
	return returnLightingData;
}

#ifdef __cplusplus
}
#endif

#endif  // AF_LIGHTINGDATA_H
