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
	AF_LightingData returnLightingData = {
		.enabled = AF_TRUE,
        .shadowsEnabled = AF_TRUE,
        .maxLights = MAX_LIGHT_NUM,
		.ambientColor = { 255, 255, 255},	
        .ambientLightEntityIndex = 0,
        .spotLightEntityIndex = 0,
        .pointLightIndexArray = {0,0,0,0},
        .pointLightsFound = 0,
        .shadowLightSpaceMatrix = Mat4_ZERO()
	};

	return returnLightingData;
}

#ifdef __cplusplus
}
#endif

#endif  // AF_LIGHTINGDATA_H
