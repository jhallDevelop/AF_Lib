/*
===============================================================================
AF_CCOLLIDER_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CCOLLIDER_H
#define AF_CCOLLIDER_H
#include <stddef.h>
#include "AF_Lib_Define.h"
#include "AF_Rect.h"
#include "AF_Collision.h"
#include "ECS/Components/AF_Component.h"
#ifdef __cplusplus
extern "C" {    
#endif

/*
====================
AF_CCollider Struct used for physics
====================
*/
typedef struct {
    PACKED_CHAR enabled;
    AF_Rect bounds;
    AF_Collision collision;
    BOOL showDebug;
} AF_CCollider;

// dummy callback so our initialisation compiles when we call AF_CCollider_Add
static void AF_Collision_DummyCallback(AF_Collision* _collisionPtr){
	if(_collisionPtr){}
}
/*
====================
AF_CCollider_ZERO
Add the CCollider component
enable the component and set some values to default values.
====================
*/
static inline AF_CCollider AF_CCollider_ZERO(void){
	PACKED_CHAR component = FALSE;
	component = AF_Component_SetHas(component, FALSE);
	component = AF_Component_SetEnabled(component, FALSE);
	AF_CCollider rigidbody = {
		//.has = TRUE,
		.enabled = FALSE,//component,
		.bounds = {
			0,	// x
			0,	// y
			0,	// width
			0	// height
		},
		//.collision = {FALSE, NULL, NULL},
		.showDebug = FALSE
	};
        rigidbody.collision.collided  = FALSE;
	rigidbody.collision.entity1 = NULL;
	rigidbody.collision.entity2 = NULL;
	rigidbody.collision.callback = AF_Collision_DummyCallback;//(void (*)(AF_Collision))0;
	return rigidbody;
}


/*
====================
AF_CCollider_ADD
Add the CCollider component
enable the component and set some values to default values.
====================
*/
static inline AF_CCollider AF_CCollider_ADD(void){
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);
	AF_CCollider rigidbody = {
		//.has = TRUE,
		.enabled = TRUE,//component,
		.bounds = {
			0,	// x
			0,	// y
			0,	// width
			0	// height
		},
		//.collision = {FALSE, NULL, NULL},
		.showDebug = FALSE
	};
        rigidbody.collision.collided  = FALSE;
	rigidbody.collision.entity1 = NULL;
	rigidbody.collision.entity2 = NULL;
	rigidbody.collision.callback = AF_Collision_DummyCallback;//(void (*)(AF_Collision))0;
	return rigidbody;
}



#ifdef __cplusplus
}
#endif

#endif //AF_CCOLLIDER_H

