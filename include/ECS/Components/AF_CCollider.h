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
#include "AF_CollisionVolume.h"
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
    enum CollisionVolumeType type;
    Vec3 boundingVolume;
	Vec3 pos;
    //AF_Rect bounds;
    AF_Collision collision; //TODO: why do we need this?
    BOOL showDebug;
	Vec3 broadphaseAABB;
} AF_CCollider;

void AF_Collision_DummyCallback(AF_Collision* _collisionPtr);
AF_CCollider AF_CCollider_ZERO(void);
AF_CCollider AF_CCollider_ADD(void);
AF_CCollider AF_CCollider_Sphere_ADD(void);
AF_CCollider AF_CCollider_Box_ADD(void);
AF_CCollider AF_CCollider_Plane_ADD(void);
AF_CCollider AF_CCollider_ADD_TYPE(enum CollisionVolumeType _volumeType);

#ifdef __cplusplus
}
#endif

#endif //AF_CCOLLIDER_H

