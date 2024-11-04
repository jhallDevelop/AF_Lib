/*
===============================================================================
AF_CPLAYERDATA_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CPLAYERDATA_H
#define AF_CPLAYERDATA_H
#include "AF_Component.h"
#ifdef __cplusplus
extern "C" {    
#endif
/*
====================
AF_CSprite
====================
*/
// size is 64 bytes
typedef struct  {
	//BOOL has;
	//BOOL enabled;
	PACKED_CHAR enabled;	    // 1 byte
	BOOL isCarrying;
    int carryingEntity;
    float health;
    BOOL isAlive;
	
} AF_CPlayerData;


/*
====================
AF_AF_CPlayerData_ZERO
Empty constructor for the AF_CSprite component
====================
*/
static inline AF_CPlayerData AF_CPlayerData_ZERO(void){
	AF_CPlayerData returnComponent = {
		//.has = false,
		.enabled = FALSE,
		.isCarrying = FALSE,
        .carryingEntity = 0,
        .health = 0,
        .isAlive = FALSE
		};
	return returnComponent;
}

/*
====================
AF_AF_CPlayerData_ADD
ADD component and set default values
====================
*/
static inline AF_CPlayerData AF_CPlayerData_ADD(void){
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);
	AF_CPlayerData returnComponent = {
		//.has = true,
		.enabled = component,
		.isCarrying = FALSE,
        .carryingEntity = 0,
        .health = 100,
        .isAlive = TRUE
	};
	return returnComponent;
}

#ifdef __cplusplus
}
#endif

#endif