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


#ifdef __cplusplus
extern "C" {    
#endif


/*
====================
AF_Collision Struct used for registering collisions
====================
*/
typedef struct AF_Collision {
	BOOL collided;
	void* entity1;
	void* entity2;
	void (*callback)(struct AF_Collision*);
} AF_Collision;


#ifdef __cplusplus
}
#endif

#endif //AF_COLLISION_H
