#include "ECS/Components/AF_CLight.h"
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CLight_ZERO
Function used to create an empty component
====================
*/
AF_CLight AF_CLight_ZERO(void){
    AF_CLight returnObject = {
	//.has = AF_FALSE,
	.enabled = AF_FALSE
    };
    return returnObject;
}

/*
====================
AF_CLight_ADD
Function used to Add the component
====================
*/
AF_CLight AF_CLight_ADD(void){
    PACKED_CHAR component = AF_TRUE;
    component = AF_Component_SetHas(component, AF_TRUE);
    component = AF_Component_SetEnabled(component, AF_TRUE);
    Vec3 vec3Zero = {0.0f, 0.0f, 0.0f};
    AF_CLight returnObject = {
	//.has = AF_TRUE,
	.enabled = component,
    .lightType = AF_LIGHT_TYPE_POINT,
    .direction = {0.0f, -1.0f, 0.0f},
    .ambientCol = {0.5f, 0.5f, 0.5f},
    .diffuseCol = {0.4f, 0.4f, 0.4f},
    .specularCol = {0.5f, 0.5f, 0.5f},
    .constant = 1.0f, 
    .linear = 0.09f,
    .quadratic = 0.032f,
    .cutOff = 0.9763f,
    .outerCutoff = 0.9659f
	};
    return returnObject;
}

