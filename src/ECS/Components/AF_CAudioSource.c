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
	.enabled = FALSE,
    .clip = {0,0,0},
    .channel = 255,
    .loop = FALSE,
    .isPlaying = FALSE,
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
    component = AF_Component_SetHas(component, TRUE);
    component = AF_Component_SetEnabled(component, TRUE);

    AF_CAudioSource returnMesh = {
	.enabled = component,
    .clip = {0,0,0},
    .channel = 255,
    .loop = FALSE,
    .isPlaying = FALSE,
    .clipData = NULL
    };
    return returnMesh;
}

