/*
===============================================================================
AF_PHYSICS_H

Implementation for Physics helper functions such as
General collision check on all entities with filtering
AABB collision detection
===============================================================================
*/
#ifndef AF_PHYSICS_H
#define AF_PHYSICS_H
#include "AF_Lib_Define.h"
#include "ECS/Components/AF_CCollider.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_CTransform2D.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_PHYSICS_COLLISION_AABB
Simple aabb rect 
Return true if collission occured
====================
*/
static inline BOOL AF_Physics_AABB(AF_Rect* _rect1, AF_Rect* _rect2){
	//https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection#:~:text=AABB%20%2D%20AABB%20collisions,the%20x%20and%20y%20axis.
	BOOL returnValue = FALSE;
	// Collision x-axis?
	BOOL collisionX = _rect1->w >= _rect2->x &&
		_rect2->w >= _rect1->x;

	// Collision y-axis?
	BOOL collisionY = _rect1->h >= _rect2->y &&
		_rect2->h >= _rect1->y;

	if (collisionX && collisionY) {
		returnValue = TRUE;
	}
	// collsion only if on both axes
	return returnValue;
}
#ifndef PLATFORM_GB
/*
====================
AF_PHYSICS_3DCOLLISION_CHECK
Simple aabb rect 
Return true if collission occured
====================
*/
static inline void AF_Physics_3DCollision_Check(AF_Entity* _entities, uint32_t _entityCount){
	for(uint32_t i = 0; i < _entityCount; i++){
		// Entity 1
		AF_Entity* entity1 = &_entities[i];
		// NULL check
		if(entity1 == NULL){
			continue;
		}
		if(entity1->flags == FALSE){
			continue;
		}

		AF_CCollider* collider1 = entity1->collider;
		BOOL hasCollider1 = AF_Component_GetHas(collider1->enabled);
		BOOL enabledCollider1 = AF_Component_GetEnabled(collider1->enabled);

		if((hasCollider1 == FALSE) || (enabledCollider1 == FALSE)){
			continue;
		}
		
		// TODO: fix this so its not just 2d
		AF_CTransform3D* entity1Transform = entity1->transform; 
		// update the bounds of entity1 collider
		
		//float screenRatio = 1290 / 1080;

		AF_Rect collider1Bounds = {
		    entity1Transform->pos.x,// - (entity1Transform->scale.x * 0.5f * screenRatio),
		    entity1Transform->pos.y,// - (entity1Transform->scale.y * 0.5f),
		    (entity1Transform->scale.x / 2) + entity1Transform->pos.x,// * screenRatio,
		    (entity1Transform->scale.y / 2) + entity1Transform->pos.y
		};
		collider1->bounds = collider1Bounds;
		// Entity 2
		for(uint32_t y = 0; y < _entityCount; y++){

			AF_Entity* entity2 = &_entities[y];
			// NULL check
			if(entity2 == NULL){
				continue;
			}
			if(entity2->flags == FALSE){
				continue;
			}

			AF_CCollider* collider2 = entity2->collider;
			BOOL hasCollider2 = AF_Component_GetHas(collider1->enabled);
			BOOL enabledCollider2 = AF_Component_GetEnabled(collider2->enabled);

			if((hasCollider2 == FALSE) || (enabledCollider2 == FALSE)){
				continue;
			}

			// Don't check the collision against the same entity as itself.
			if(entity1->id_tag == entity2->id_tag){
				continue;
			}
			AF_CTransform3D* entity2Transform = entity2->transform;
		        // Update entity 2 bounds 
		/*	
			AF_Rect collider2Bounds = {
			    entity2Transform->pos.x - (entity2Transform->scale.x / 2.0f),
			    entity2Transform->pos.y - (entity2Transform->scale.y / 2.0f),
			    entity2Transform->scale.x,
			    entity2Transform->scale.y
			};*/
			AF_Rect collider2Bounds = {
				entity2Transform->pos.x, // x (left)
				entity2Transform->pos.y,  // y (bottom)
				(entity2Transform->scale.x) * 0/ 2 + entity2Transform->pos.x ,// * screenRatio,                                        // width
				(entity2Transform->scale.y) * 0/ 2 + entity2Transform->pos.y
			};

			// update the bounds
			collider2->bounds = collider2Bounds;


			BOOL collisionResult = AF_Physics_AABB(&collider1->bounds, &collider2->bounds);
			// If a collision occured, update both colliders with the collision information.
			AF_Collision collision1 = {collisionResult, entity1, entity2, collider1->collision.callback}; 
			AF_Collision collision2 = {collisionResult, entity2, entity1, collider2->collision.callback};
			collider1->collision = collision1; 
			collider2->collision = collision2;

			// run the function pointer if collision occured
			if(collisionResult == TRUE){
				if(collision1.callback != NULL){
					collision1.callback(collision1);
				}
				if(collision2.callback != NULL){
					collision2.callback(collision2);
				}
			}
		}
	}
}
#endif

#ifdef PLATFORM_GB
/*
====================
AF_PHYSICS_2DCOLLISION_CHECK
Simple aabb rect 
Return true if collission occured
====================
*/
static inline void AF_Physics_2DCollision_Check(AF_Entity* _entities, uint32_t _entityCount){
	for(uint32_t i = 0; i < _entityCount; i++){
		// Entity 1
		AF_Entity* entity1 = &_entities[i];
		// NULL check
		if(entity1 == NULL){
			continue;
		}
		if(entity1->flags == FALSE){
			continue;
		}

		AF_CCollider* collider1 = entity1->collider;
		BOOL hasCollider1 = AF_Component_GetHas(collider1->enabled);
		BOOL enabledCollider1 = AF_Component_GetEnabled(collider1->enabled);

		if((hasCollider1 == FALSE) || (enabledCollider1 == FALSE)){
			continue;
		}
		
		// TODO: fix this so its not just 2d
		AF_CTransform2D* entity1Transform = entity1->transform; 
		// update the bounds of entity1 collider
		
		//float screenRatio = 1290 / 1080;

		AF_Rect collider1Bounds = {
		    entity1Transform->pos.x,// - (entity1Transform->scale.x * 0.5f * screenRatio),
		    entity1Transform->pos.y,// - (entity1Transform->scale.y * 0.5f),
		    (entity1Transform->scale.x / 2) + entity1Transform->pos.x,// * screenRatio,
		    (entity1Transform->scale.y / 2) + entity1Transform->pos.y
		};
		collider1->bounds = collider1Bounds;
		// Entity 2
		for(uint32_t y = 0; y < _entityCount; y++){

			AF_Entity* entity2 = &_entities[y];
			// NULL check
			if(entity2 == NULL){
				continue;
			}
			if(entity2->flags == FALSE){
				continue;
			}

			AF_CCollider* collider2 = entity2->collider;
			BOOL hasCollider2 = AF_Component_GetHas(collider1->enabled);
			BOOL enabledCollider2 = AF_Component_GetEnabled(collider2->enabled);

			if((hasCollider2 == FALSE) || (enabledCollider2 == FALSE)){
				continue;
			}

			// Don't check the collision against the same entity as itself.
			if(entity1->id_tag == entity2->id_tag){
				continue;
			}
			AF_CTransform2D* entity2Transform = entity2->transform;
		        // Update entity 2 bounds 
		/*	
			AF_Rect collider2Bounds = {
			    entity2Transform->pos.x - (entity2Transform->scale.x / 2.0f),
			    entity2Transform->pos.y - (entity2Transform->scale.y / 2.0f),
			    entity2Transform->scale.x,
			    entity2Transform->scale.y
			};*/
			AF_Rect collider2Bounds = {
				entity2Transform->pos.x, // x (left)
				entity2Transform->pos.y,  // y (bottom)
				(entity2Transform->scale.x) * 0/ 2 + entity2Transform->pos.x ,// * screenRatio,                                        // width
				(entity2Transform->scale.y) * 0/ 2 + entity2Transform->pos.y
			};

			// update the bounds
			collider2->bounds = collider2Bounds;


			BOOL collisionResult = AF_Physics_AABB(&collider1->bounds, &collider2->bounds);
			// If a collision occured, update both colliders with the collision information.
			AF_Collision collision1 = {collisionResult, entity1, entity2, collider1->collision.callback}; 
			AF_Collision collision2 = {collisionResult, entity2, entity1, collider2->collision.callback};
			collider1->collision = collision1; 
			collider2->collision = collision2;

			// run the function pointer if collision occured
			if(collisionResult == TRUE){
				if(collision1.callback != NULL){
					collision1.callback((void*)collision1);
				}
				if(collision2.callback != NULL){
					collision2.callback((void*)collision2);
				}
			}
		}
	}
}

#endif

#ifdef __cplusplus
}
#endif

#endif //AF_PHYSICS_H
