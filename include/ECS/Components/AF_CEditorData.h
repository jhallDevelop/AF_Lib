/*
===============================================================================
AF_CEditorData_H definitions

Definition for the editor data component struct
and helper functions
===============================================================================
*/
#ifndef AF_CEDITORDATA_H
#define AF_CEDITORDATA_H
#include "AF_Component.h"
#include "AF_Lib_Define.h"
#include <stdbool.h>

#define MAX_EDITORDATA_NAME_CHAR_LENGTH 256

#ifdef __cplusplus
extern "C" {    
#endif
/*
====================
AF_CEDITOR_DATA
====================
*/
// size is 64 bytes
typedef struct AF_CEditorData{
	PACKED_CHAR enabled;
    char name[MAX_EDITORDATA_NAME_CHAR_LENGTH];	    
	bool isSelected;		    
} AF_CEditorData;

/*
====================
AF_CSprite_ZERO
Empty constructor for the AF_CSprite component
====================
*/
static inline AF_CEditorData AF_CEditorData_ZERO(void){
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
static inline AF_CEditorData AF_CEditorData_ADD(void){
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



#ifdef __cplusplus
}
#endif  

#endif  // AF_CEDITORData_H
