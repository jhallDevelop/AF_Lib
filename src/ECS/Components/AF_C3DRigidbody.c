#include "ECS/Components/AF_C3DRigidbody.h"
#include "ECS/Components/AF_Component.h"
/*
====================
AF_C3DRigidbody_ZERO
Empty constructor for the CRigidbody component
====================
*/
AF_C3DRigidbody AF_C3DRigidbody_ZERO(void){
	AF_C3DRigidbody rigidbody = {
		//.has = false,
		.enabled = FALSE,
		.isKinematic = FALSE,
		.gravity = TRUE,
		.velocity = {0, 0, 0},
		.anglularVelocity = {0,0,0},
		.inverseMass = 1,
		.force = {0,0,0},
		.torque = {0,0,0},
		.inertiaTensor = {0,0,0}
	};
	return rigidbody;
}
/*
====================
AF_C3DRigidbody_ADD
Add the CRigidbody component
enable the component and set some values to default values.
====================
*/
AF_C3DRigidbody AF_C3DRigidbody_ADD(void){ //
	PACKED_CHAR component = TRUE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);

	AF_C3DRigidbody rigidbody = {
		//.has = true,
		.enabled = component,
		.isKinematic = FALSE,			// isKinematic means to be controlled by script rather than the velocity
		.gravity = FALSE,				// gravity off by default
		.velocity = {0, 0, 0},		// zero velocity 
		.anglularVelocity = {0,0,0},
		.inverseMass = 1,
		.force = {0,0,0},
		.torque = {0,0,0},
		.inertiaTensor = {0,0,0}
	};
	return rigidbody;
}
