/*
===============================================================================
AF_C3DRIGIDBODY_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_C3DRIGIDBODY_H
#define AF_C3DRIGIDBODY_H
#include "AF_Lib_Define.h"
#include "AF_CCollider.h"
#include "AF_Vec3.h"
#ifdef __cplusplus
extern "C" {    
#endif

/*
====================
AF_C3DRigidbody 
Component used to contain data for physics
====================
*/
// size is 64 bytes
typedef struct {
    //BOOL has;
    BOOL enabled;
    BOOL isKinematic;
    BOOL  gravity;
    Vec3 velocity;
	Vec3 anglularVelocity;
	float inverseMass;
	Vec3 force;
	Vec3 torque;
	Vec3 inertiaTensor;

} AF_C3DRigidbody;

AF_C3DRigidbody AF_C3DRigidbody_ZERO(void);
AF_C3DRigidbody AF_C3DRigidbody_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_C3DRIGIDBODY_H

