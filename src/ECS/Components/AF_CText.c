#include "ECS/Components/AF_CText.h"
#include "ECS/Components/AF_Component.h"
#include <stddef.h> // For NULL

/*
AF_CText_ZERO
Function used to create an empty text component
*/
AF_CText AF_CText_ZERO(void){
    AF_CText returnMesh = {
    .enabled = AF_FALSE,
    .isDirty = AF_FALSE,
    .isShowing = AF_FALSE,
    .fontID = 0,
    .fontPath = "\0",
    .text = "\0",
    .screenPos = {0,0},
    .textBounds = {0,0},
    .textColor = {0,0,0,1},
    .textData = NULL
    };
    return returnMesh;
}

/*
====================
AF_CText_ADD
Function used to Add the component
====================
*/
AF_CText AF_CText_ADD(void){
    PACKED_CHAR component = 0;
    component = AF_Component_SetHas(component, AF_TRUE);
    component = AF_Component_SetEnabled(component, AF_TRUE);

    AF_CText returnMesh = {
	component,
    AF_TRUE,
    AF_TRUE,
    0,
    "\0",
    "\0",
    {0,0},
    {0,0},
    {0,0,0,1},
    NULL
    };
    return returnMesh;
}

