/*
===============================================================================
AF_CAI_ACTION_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_AI_ACTION_H
#define AF_AI_ACTION_H
#include "AF_Lib_Define.h"
#include <stddef.h> // For NULL

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AI_ACTION_Type {
	AI_ACTION_DEFAULT = 0,
	AI_ACTION_IDLE = 1,
	AI_ACTION_GOTO = 2,
	AI_ACTION_ATTACK = 3
} AI_ACTION_Type;


// Typedef for readability (optional but common in C)
typedef struct AF_AI_Action {
    af_bool_t enabled;
    AI_ACTION_Type actionType;
    void* sourceEntity;    // Avoids circular dependency with AF_Entity
    void* targetEntity;
    void (*callback)(struct AF_AI_Action*); // Correct forward reference
} AF_AI_Action;

static inline AF_AI_Action AF_AI_Action_Zero(void){
	AF_AI_Action zeroAction;
	zeroAction.enabled = AF_FALSE;
	zeroAction.actionType = AI_ACTION_DEFAULT;
	zeroAction.sourceEntity = NULL;
	zeroAction.targetEntity = NULL;
	zeroAction.callback = NULL;
	
	return zeroAction;
}
#ifdef __cplusplus
}
#endif


#endif //AF_AI_ACTION_H

