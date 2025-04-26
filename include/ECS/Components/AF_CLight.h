/*
===============================================================================
AF_CLight_H

Header only Mesh loading
functions to load meshes, creating memory on the heap based on the size of the mesh

===============================================================================
*/
#ifndef AF_CLIGHT_H
#define AF_CLIGHT_H
#include "AF_Lib_Define.h"
#include "ECS/Components/AF_Component.h"
#include "AF_Vec3.h"

#ifdef __cplusplus
extern "C" {
#endif  
#define AF_CLIGHT_TYPE_COUNT 3

enum AF_Light_Type_e{
	AF_LIGHT_TYPE_POINT = 0, 
	AF_LIGHT_TYPE_SPOT = 1,
	AF_LIGHT_TYPE_AMBIENT = 2
};


// Mesh Struct
typedef struct AF_CLight {
    PACKED_CHAR enabled;
    enum AF_Light_Type_e lightType;
    Vec3 direction;
    Vec3 ambientCol;
    Vec3 diffuseCol;
    Vec3 specularCol;
    AF_FLOAT constant;
    AF_FLOAT linear;
    AF_FLOAT quadratic;
    AF_FLOAT cutOff;
    AF_FLOAT outerCutoff;
} AF_CLight;

/*
====================
AF_CLight_ZERO
Function used to create an empty component
====================
*/
static inline AF_CLight AF_CLight_ZERO(void){
    AF_CLight returnObject = {
	//.has = FALSE,
	.enabled = FALSE
    };
    return returnObject;
}

/*
====================
AF_CLight_ADD
Function used to Add the component
====================
*/
static inline AF_CLight AF_CLight_ADD(void){
    PACKED_CHAR component = TRUE;
    component = AF_Component_SetHas(component, TRUE);
    component = AF_Component_SetEnabled(component, TRUE);
    Vec3 vec3Zero = {0.0f, 0.0f, 0.0f};
    AF_CLight returnObject = {
	//.has = TRUE,
	.enabled = component,
    .lightType = AF_LIGHT_TYPE_POINT,
    .direction = vec3Zero,
    .ambientCol = vec3Zero,
    .diffuseCol = vec3Zero,
    .specularCol = vec3Zero,
    .constant = 0.0f, 
    .linear = 0.0f,
    .quadratic = 0.0f,
    .cutOff = 0.0f,
    .outerCutoff = 0.0f
	};
    return returnObject;
}



#ifdef __cplusplus
}
#endif  

#endif  // AF_CLIGHT_H
