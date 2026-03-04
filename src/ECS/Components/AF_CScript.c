#include "ECS/Components/AF_CScript.h"
#include <stddef.h> // For NULL
#include <string.h> // For memset
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CCamera_ZERO
Initialisation constructor function
====================
*/
AF_CScript AF_CScript_ZERO(void){
    AF_CScript returnComponent;
    memset(&returnComponent, 0, sizeof(AF_CScript));
    returnComponent.enabled = AF_FALSE;
    return returnComponent;
}

/*
====================
AF_CCamera_ADD
Add the camera component
Initialise with enable and has set to true
====================
*/
AF_CScript AF_CScript_ADD(void){
    AF_CScript returnComponent;
    memset(&returnComponent, 0, sizeof(AF_CScript));
    returnComponent.enabled = AF_FALSE;
    returnComponent.enabled = AF_Component_SetHas(returnComponent.enabled, AF_TRUE);
    returnComponent.enabled = AF_Component_SetEnabled(returnComponent.enabled, AF_TRUE);
    return returnComponent;
}
