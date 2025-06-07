#include "ECS/Components/AF_CAudioSource.h"
#include "ECS/Components/AF_Component.h"
#include <stddef.h> // For NULL

/*
====================
AF_CAudioSource_ZERO
Function used to create an empty text component
====================
*/
AF_CAudioSource AF_CAudioSource_ZERO(void){
    AF_CAudioSource returnMesh = {
	.enabled = AF_FALSE,
    .clip = {0,0,0},
    .channel = 255,
    .loop = AF_FALSE,
    .isPlaying = AF_FALSE,
    .clipData = NULL
    };
    return returnMesh;
}

/*
====================
AF_CAudioSource_ADD
Function used to Add the component
====================
*/
AF_CAudioSource AF_CAudioSource_ADD(void){
    PACKED_CHAR component = 0;
    component = AF_Component_SetHas(component, AF_TRUE);
    component = AF_Component_SetEnabled(component, AF_TRUE);

    AF_CAudioSource returnMesh = {
	.enabled = component,
    .clip = {0,0,0},
    .channel = 255,
    .loop = AF_FALSE,
    .isPlaying = AF_FALSE,
    .clipData = NULL
    };
    return returnMesh;
}

