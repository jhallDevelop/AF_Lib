/*
===============================================================================
AF_COLLISION_H definitions

Definition for the collision struct
and helper functions
===============================================================================
*/
#ifndef AF_COLLISION_H
#define AF_COLLISION_H
#include "AF_Lib_Define.h"
#include "AF_Math/AF_Vec3.h"

#ifdef __cplusplus
extern "C" {    
#endif


/*
====================
AF_Collision Struct used for registering collisions
====================
*/
typedef struct AF_Collision {
	af_bool_t collided;
	uint32_t entity1ID;
	uint32_t entity2ID;
	void (*callback)(struct AF_Collision*);
	Vec3 collisionPoint;
	float rayDistance;
	Vec3 normal;
	float penetration;
} AF_Collision;

/*
====================
AF_Collision Initialisation used for registering collisions
====================
*/
static inline AF_Collision AF_Collision_ZERO(void){
	AF_Collision collision;
	collision.collided = AF_FALSE;
	collision.entity1ID = 0;
	collision.entity2ID = 0;
	collision.callback = NULL;
	collision.collisionPoint = Vec3_ZERO();
	collision.rayDistance = 0.0f;
	collision.normal = Vec3_ZERO();
	collision.penetration = 0.0f;
	return collision;
}


#ifdef __cplusplus
}
#endif

#endif //AF_COLLISION_H
