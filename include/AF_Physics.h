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
#include "ECS/Entities/AF_Entity.h"
#include "ECS/Components/AF_CCollider.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_CTransform2D.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_CollisionVolume.h"
#include "AF_Ray.h"
#include "AF_Vec3.h"
#include "AF_Util.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_PHYSICS_INIT
Definition for Physics init
====================
*/
void AF_Physics_Init();

/*
====================
AF_PHYSICS_UPDATE
Definition for Physics update
====================
*/
void AF_Physics_Update(AF_ECS* _ecs);

/*
====================
AF_PHYSICS_SHUTDOWN
Definition for Physics shutdown 
====================
*/
void AF_Physics_Shutdown();



/*
====================
AF_PHYSICS_SPHERE_RAYINTERSECTION
Calculate ray intersection hit test
====================
*/
static inline BOOL AF_Physics_Sphere_RayIntersection(const Ray* _ray, const AF_CTransform3D* _transform, const AF_CCollider* _collider, AF_Collision* _collision){
	Vec3 spherePos = _transform->pos;
	//Box* sphereCollisionVolume = (Sphere_CollisionVolume*)_collider->boundingVolume;
	AF_FLOAT sphereRadius = _collider->boundingVolume.x;
	Vec3 direction = Vec3_MINUS(spherePos, _ray->position);
	//Then project the sphere’s origin onto our ray direction vector

	AF_FLOAT sphereProj = Vec3_DOT(direction, _ray->direction);
	
	// Is point behind the ray?
	if(sphereProj < 0.0f) {
		return false; //point is behind the ray!
	}	
	
	//Get closest point on ray line to sphere
	Vec3 point = Vec3_MULT_SCALAR(Vec3_ADD(_ray->position, _ray->direction), sphereProj);
	
	AF_FLOAT sphereDist = Vec3_MAGNITUDE(Vec3_MINUS(point, spherePos));

	if (sphereDist > sphereRadius) { 
		return false;
	}
	AF_FLOAT offset = AF_Math_Sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

 
	_collision->rayDistance = sphereProj - (offset);
	_collision->collisionPoint = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, _collision->rayDistance));
		
} 

/*
====================
AF_PHYSICS_Box_RAYINTERSECTION
Calculate ray intersection hit test against a box
====================
*/
static inline BOOL AF_Physics_Box_RayIntersection(const Ray* _ray, const Vec3 _boxPos, const Vec3 _boxSize, AF_Collision* _collision){
	Vec3 boxMin = Vec3_MINUS(_boxPos, _boxSize);
	Vec3 boxMax = Vec3_ADD(_boxPos, _boxSize);
	BOOL returnResult = TRUE;
	AF_FLOAT rayPos[3] = {_ray->position.x, _ray->position.y, _ray->position.z};
	AF_FLOAT rayDir[3] = {_ray->direction.x, _ray->direction.y, _ray->direction.z};
	
	AF_FLOAT tVals[3] = {-1, -1, -1};
	// convert to float array so we can use a forloop on the values
	AF_FLOAT boxMinArray[3] = {boxMin.x, boxMin.y, boxMin.z};
	AF_FLOAT boxMaxArray[3] = {boxMax.x, boxMax.y, boxMax.z};
	

	// Get the x, y, z values if the collision is infront or behind the box edge
	for(int i = 0; i < 3; ++i){
		if(rayDir[i] > 0){
			tVals[i] = (boxMinArray[i] - rayPos[i]) / rayDir[i];
		}else if(rayDir[i] < 0){
			tVals[i] = (boxMaxArray[i] - rayPos[i]) / rayDir[i];
		}
	}

	// Figure out if the x, y, or z is the largest value
	AF_FLOAT bestT = AF_GetMaxElement(tVals, 3);
	if(bestT < 0.0f){
		debugf("AF_Physics_Box_RayIntersection: no backwards ray\n");
		returnResult = FALSE; // no backwards rays 
		return returnResult;
	}

	Vec3 intersection = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, bestT));
	float intersectionFloatArray[3] = {intersection.x, intersection.y, intersection.z};
	const AF_FLOAT epsilon = 0.0001f;
	for(int i = 0; i < 3; ++i){
		if(	intersectionFloatArray[i] + epsilon < boxMinArray[i] ||
			intersectionFloatArray[i] - epsilon > boxMaxArray[i]) {
				debugf("AF_Physics_Box_RayIntersection: best intersection doesn't touch box \n");
				returnResult = FALSE; // best intersection doesn't touch the box
				return returnResult;
			}
	}
	_collision->collisionPoint = intersection;
	_collision->rayDistance = bestT;
	returnResult = TRUE; 
	debugf("AF_Physics_Box_RayIntersection: result %i \n", returnResult);
	return returnResult;
} 

/*
====================
AF_PHYSICS_AABB_RAYINTERSECTION
Calculate ray intersection hit test against an Axis Aligned Bounding Box
====================
*/
static inline BOOL AF_Physics_AABB_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){
	Vec3 boxPos = _collider->pos;
	Vec3* _size = &_collider->boundingVolume;
	Vec3 boxHalfSize = Vec3_DIV_SCALAR(*_size, 2);
	return AF_Physics_Box_RayIntersection(_ray, boxPos, boxHalfSize, _collision);
} 

/*
====================
AF_PHYSICS_OBB_RAYINTERSECTION
Calculate ray intersection hit test against a Object Orientated Box
====================
*/
static inline BOOL AF_Physics_OBB_RayIntersection(const Ray* _ray, const AF_CTransform3D* _worldTransform, const Vec3* _size, AF_Collision* _collision){

	Vec4 orientation = {_worldTransform->rot.x, _worldTransform->rot.y, _worldTransform->rot.z, 1};
	Vec3 postion = _worldTransform->pos;
	//TODO: implement correctly. p16 https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/1raycasting/Physics%20-%20Raycasting.pdf#page=5.08
	/*
	Matrix3 transform = Matrix3 ( orientation );
	Matrix3 invTransform = Matrix3 ( orientation . Conjugate ());

	Vector3 localRayPos = r . GetPosition () - position ;

	Ray tempRay ( invTransform * localRayPos , invTransform * r . GetDirection ());

	bool collided = Ray Bo xI nt er se ct io n ( tempRay , Vector3 () ,
	volume . Ge tHalfD imensi ons () , collision );

	if ( collided ) {
		collision . collidedAt = transform * collision . collidedAt + position ;
	}
		return collided ;
	*/
	return false;
}



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

/*
====================
AF_PHYSICS_Point_Inside_Rect
Calculate ray intersection hit test against a Object Orientated Box
====================
*/
static inline BOOL AF_Physics_Point_Inside_Rect(Vec2 _point, AF_Rect _rect){
	BOOL returnValue = FALSE;
	if (_point.x >= _rect.x && _point.x <= (_rect.x + _rect.w) &&
        _point.y >= _rect.y && _point.y <= (_rect.y + _rect.h)) {
        returnValue = TRUE;
    }
	return returnValue;
}
/*
====================
AF_PHYSICS_Plane_RAYINTERSECTION
Calculate ray intersection hit test against a Object Orientated Box
====================
*/
static inline BOOL AF_Physics_Plane_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){

	Vec3 planePos = _collider->pos;
	Vec3* _size = &_collider->boundingVolume;
	// assume a horizontal plane at planePos.y
	AF_FLOAT t = (planePos.y - _ray->position.y) / _ray->direction.y;

	BOOL returnValue = TRUE;

	if(t < 0){
		returnValue = FALSE;  // No intersection (plane is behind the ray or parallel)
		debugf("AF_Physics_Box_RayIntersection: no intersection, plane is behind the ray \n");
		return returnValue;
	}

	Vec3 intersection = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, t));
	

	// Check if the intersection is within the desired plane bounds (if the plane is finite)
	if(_size->y == 0 && _size->x == 0 && _size->z == 0){
		// bounds are all 0 so plane is counted as infinite.
		debugf("AF_Physics_Plane_RayIntersection: intersection on infintie plane at x: %f y: %f z: %f \n", intersection.x, intersection.y, intersection.z);
		returnValue = TRUE;
		_collision->collisionPoint = intersection;
		_collision->rayDistance = t;
		return returnValue;
	}

	// Check if within size/bounds
	Vec2 intersectionPoint = {intersection.x, intersection.z};
	AF_Rect planeRect = {-_size->x/2, -_size->z/2, _size->x, _size->z};
	if(AF_Physics_Point_Inside_Rect(intersectionPoint, planeRect) == FALSE){
		debugf("AF_Physics_Box_RayIntersection: no intersection, ray is out of bounds \nintX: %f intY: %f rectX:%f rectY:%f rectz:%f  \n", intersectionPoint.x, intersectionPoint.y, _size->x, _size->y, _size->z);
		returnValue = FALSE;
		return returnValue;
	}
	
	debugf("AF_Physics_Plane_RayIntersection: intersection within bounds x: %f y: %f z: %f \n", intersection.x, intersection.y, intersection.z);
	returnValue = TRUE;
	
	// For example, if the plane is bounded within an XZ range, you can check here

	_collision->collisionPoint = intersection;
	_collision->rayDistance = t;

	
	return returnValue;
}

/*
====================
AF_PHYSICS_RAYINTERSECTION
Calculate ray intersection hit test
====================
*/
static inline BOOL AF_Physics_RayIntersection(const Ray* _ray, AF_Entity* _entity, AF_Collision* _collision){
	const AF_CTransform3D* transform = _entity->transform;
	const AF_CCollider* collider = _entity->collider;
	enum CollisionVolumeType type = collider->type;

	switch(type){
		case Plane:
			return AF_Physics_Plane_RayIntersection(_ray, _entity->collider, _collision);
		break;
		case AABB:
			return AF_Physics_AABB_RayIntersection(_ray, _entity->collider, _collision);
		break;
		
		case OBB:
			debugf("AF_Physics_RayIntersection: OBB ray interaction not implemented\n");
			return false;
		break;

		case Sphere:
			return false;//AF_Physics_Sphere_RayIntersection(_ray, _entity->transform, _entity->collider, _collision);
		break;

		case Mesh:
			debugf("AF_Physics_RayIntersection: Mesh ray interaction not implemented\n");
			return false;
		break;

		case Compound:
			debugf("AF_Physics_RayIntersection: Compound ray interaction not implemented\n");
			return false;
		break;

		case Invalid:
			debugf("AF_Physics_RayIntersection: Invalid collider type\n");
			return false;
		break;
	}
	
	return false;
}


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


/*
====================
AF_PHYSICS_3DCOLLISION_CHECK
Simple aabb rect 
Return true if collission occured
====================
*/
/*
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
					collision1.callback(&collision1);
				}
				if(collision2.callback != NULL){
					collision2.callback(&collision2);
				}
			}
		}
	}
}
*/

#ifdef __cplusplus
}
#endif

#endif //AF_PHYSICS_H
