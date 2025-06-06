#include "ECS/Components/AF_CPlayerData.h"
#include "ECS/Components/AF_Component.h"
#include "AF_Vec3.h"
/*
====================
AF_AF_CPlayerData_ZERO
Empty constructor for the AF_CSprite component
====================
*/
AF_CPlayerData AF_CPlayerData_ZERO(void){
	Vec3 startPosition = {0,0,0};
	AF_CPlayerData returnComponent = {
		//.has = false,
		.enabled = FALSE,
		.isHuman = FALSE,
		.isCarrying = FALSE,
		.isCarried = FALSE,
        .carryingEntity = 0,
        .health = 0,
        .isAlive = FALSE,
		.isAttacking = FALSE,
		.isJumping = FALSE,
		.movementSpeed = 0,
		.score = 0,
		.startPosition = startPosition,
		.targetDestination = startPosition,
		.spawnTime = 0.0f,
		.faction = DEFAULT
		};
	return returnComponent;
}

/*
====================
AF_AF_CPlayerData_ADD
ADD component and set default values
====================
*/
AF_CPlayerData AF_CPlayerData_ADD(void){
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);
	Vec3 startPosition = {0,0,0};
	AF_CPlayerData returnComponent = {
		//.has = true,
		.enabled = component,
		.isHuman = TRUE,
		.isCarrying = FALSE,
		.isCarried = FALSE,
        .carryingEntity = 0,
        .health = 100,
        .isAlive = TRUE,
		.isAttacking = FALSE,
		.isJumping = FALSE,
		.movementSpeed = 0,
		.score = 0,
		.startPosition = startPosition,
		.targetDestination = startPosition,
		.spawnTime = 0.0f,
		.faction = DEFAULT
	};
	return returnComponent;
}
