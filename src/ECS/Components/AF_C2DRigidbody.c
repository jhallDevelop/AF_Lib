#include "ECS/Components/AF_C2DRigidbody.h"
#include "ECS/Components/AF_Component.h"
/*
====================
AF_C2DRigidbody_ZERO
Empty constructor for the CRigidbody component
====================
*/
AF_C2DRigidbody AF_C2DRigidbody_ZERO(void){
	AF_C2DRigidbody rigidbody = {
		//.has = false,
		.enabled = FALSE,
		.isKinematic = FALSE,
		.velocity = {0, 0},
		.gravity = 0
	};
	return rigidbody;
}
/*
====================
AF_C2DRigidbody_ADD
Add the CRigidbody component
enable the component and set some values to default values.
====================
*/
AF_C2DRigidbody AF_C2DRigidbody_ADD(void){
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);

	AF_C2DRigidbody rigidbody = {
		//.has = true,
		.enabled = component,
		.isKinematic = FALSE,			// isKinematic means to be controlled by script rather than the velocity
		.velocity = {0, 0},		// zero velocity 
		.gravity = 0				// gravity off by default
	};
	return rigidbody;
}
