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
	// TODO: move strings out to a separate struct or use a string library
    char name[AF_MAX_PATH_CHAR_SIZE];	    
	bool isSelected;		    
} AF_CEditorData;

AF_CEditorData AF_CEditorData_ZERO(void);
AF_CEditorData AF_CEditorData_ADD(void);

#ifdef __cplusplus
}
#endif  

#endif  // AF_CEDITORData_H
