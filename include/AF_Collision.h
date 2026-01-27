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

#define AF_MAX_COLLISION_CONTACTS 8

/*
====================
AF_Collision Struct used for registering collisions
====================
*/
typedef struct AF_Collision {
	af_bool_t collided;
	uint32_t entity1ID;
	uint32_t entity2ID;
	Vec3 normal;
	uint32_t contactPointCount;
	Vec3 contactPoints[AF_MAX_COLLISION_CONTACTS];
	AF_FLOAT contactPenetrations[AF_MAX_COLLISION_CONTACTS];
	Vec3 collisionPoint;
	float rayDistance;
	
	float penetration;
	void (*callback)(struct AF_Collision*);
	void* ecsPtr; // Pointer to the application data, useful for callbacks
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
	
	collision.contactPointCount = 0;
	for(int i = 0; i < AF_MAX_COLLISION_CONTACTS; ++i){
		collision.contactPoints[i] = Vec3_ZERO();
		collision.contactPenetrations[i] = 0.0f;
	}
	collision.collisionPoint = Vec3_ZERO();
	collision.normal = Vec3_ZERO();
	collision.rayDistance = 0.0f;
	
	collision.penetration = 0.0f;

	collision.callback = NULL;
	collision.ecsPtr = NULL;
	return collision;
}

/*
====================
AF_Collision special initialization used for registering collisions but skipps reseting the callback
====================
*/
static inline void AF_Collision_Reset(AF_Collision* _collision){
	if(_collision == NULL){
		return;
	}
	_collision->collided = AF_FALSE;
	_collision->entity1ID = 0;
	_collision->entity2ID = 0;
	
	
	_collision->contactPointCount = 0;
	for(int i = 0; i < AF_MAX_COLLISION_CONTACTS; ++i){
		_collision->contactPoints[i] = Vec3_ZERO();
		_collision->contactPenetrations[i] = 0.0f;
	}
	_collision->normal = Vec3_ZERO();
	_collision->collisionPoint = Vec3_ZERO();
	_collision->rayDistance = 0.0f;
	
	_collision->penetration = 0.0f;
	//_collision->callback = NULL;
	_collision->ecsPtr = NULL;
}


#ifdef __cplusplus
}
#endif

#endif //AF_COLLISION_H
