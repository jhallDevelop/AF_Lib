#include "ECS/Components/AF_CEditorData.h"
#include "ECS/Components/AF_Component.h"
/*
====================
AF_CSprite_ZERO
Empty constructor for the AF_CSprite component
====================
*/
AF_CEditorData AF_CEditorData_ZERO(void){
    AF_CEditorData returnComponent = {
	//.has = FALSE,
	.enabled = FALSE,
    .name = "\0",
    .isSelected = false
    };
    return returnComponent;
}

/*
====================
AF_CEditorData_ADD
Function used to Add the component
====================
*/
AF_CEditorData AF_CEditorData_ADD(void){
    PACKED_CHAR component = TRUE;
    component = AF_Component_SetHas(component, TRUE);
    component = AF_Component_SetEnabled(component, TRUE);

    AF_CEditorData returnComponent = {
	//.has = TRUE,
	.enabled = component,
    .name = "Entity: ",
    .isSelected = false
    };
    return returnComponent;
}

