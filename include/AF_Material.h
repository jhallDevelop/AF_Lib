/*
===============================================================================
AF_MATERIAL_H defninitions

Implementation of logging helper functions for the game
Calls vfprintf but adds some colour to text output
===============================================================================
*/

#ifndef AF_MATERIAL_H
#define AF_MATERIAL_H

typedef struct{
	// TODO pack this
	uint32_t shaderID;
	uint32_t textureID;
} AF_Material;

static inline AF_Material AF_Material_ZERO(void){
	AF_Material returnMaterial = {
		.shaderID = 0,
		.textureID = 0
	};

	return returnMaterial;
}

#endif

