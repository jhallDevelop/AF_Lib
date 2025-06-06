/*
===============================================================================
AF_CAI_BEHAVIOUR_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CAI_BEHAVIOUR_H
#define AF_CAI_BEHAVIOUR_H
#include "AF_AI_Action.h"
#ifdef __cplusplus
extern "C" {    
#endif


#define AF_AI_ACTION_ARRAY_SIZE 8
/*
====================
AF_CAI_BEHAVIOUR
Struct to hold a behaviour array
====================
*///
// Size is 64 bytes
typedef struct {			// 64 or 128 bytes
	PACKED_CHAR enabled;	// 8 bytes
	AF_AI_Action actionsArray[AF_AI_ACTION_ARRAY_SIZE];	// store up to 8 actions to perform
	uint8_t nextAvailableActionSlot;
} AF_CAI_Behaviour;

AF_CAI_Behaviour AF_CAI_Behaviour_ZERO(void);
AF_CAI_Behaviour AF_CAI_Behaviour_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_CAI_BEHAVIOUR_H

