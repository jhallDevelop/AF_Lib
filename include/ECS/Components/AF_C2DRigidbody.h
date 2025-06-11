/*
===============================================================================
AF_C2DRIGIDBODY_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_C2DRIGIDBODY_H
#define AF_C2DRIGIDBODY_H
#include "AF_Lib_Define.h"
#include "AF_CCollider.h"
#include "AF_Math/AF_Vec2.h"
#ifdef __cplusplus
extern "C" {    
#endif

/*
====================
AF_C2DRigidbody 
Component used to contain data for physics
====================
*/
// size is 64 bytes
typedef struct {
    //af_bool_t has;
    af_bool_t enabled;
    af_bool_t isKinematic;
    uint32_t  gravity;
    Vec2 velocity;
} AF_C2DRigidbody;

AF_C2DRigidbody AF_C2DRigidbody_ZERO(void);
AF_C2DRigidbody AF_C2DRigidbody_ADD(void);

#ifdef __cplusplus
}
#endif

#endif //AF_C2DRIGIDBODY_H

