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


typedef struct AF_LightingData{
	// TODO pack this
    BOOL enabled;
    BOOL shadowsEnabled;
    uint32_t maxLights; 
    uint8_t ambientColor[3];
} AF_LightingData;

static inline AF_LightingData AF_AF_LightingData_ZERO(void){
	AF_LightingData returnLightingData = {
		.enabled = TRUE,
        .shadowsEnabled = TRUE,
        .maxLights = 4,
		.ambientColor = { 255, 255, 255}	// NULL 
	};

	return returnLightingData;
}

#ifdef __cplusplus
}
#endif

#endif  // AF_LIGHTINGDATA_H
