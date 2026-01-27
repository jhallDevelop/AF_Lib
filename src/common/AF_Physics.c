/*
===============================================================================
AF_PHYSICS Implementation 
Implementation of AF_Physics
===============================================================================
*/
#include "AF_Physics.h"
#include "AF_Log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Small constant to counteract floating point errors
#define AF_PHYSICS_EPSILON 0.00001f


float collisionColor[4] = {255,0, 0, 1};

typedef struct Mat3{
    // A 3x3 array to hold the matrix elements [row][column]
    float m[3][3];
} Mat3;

// Helper to multiply Mat4 (treating as 3x3) by Vec3
static inline Vec3 AF_Mat4_MULT_Vec3(Mat4 m, Vec3 v) {
    return (Vec3){
        m.rows[0].x * v.x + m.rows[0].y * v.y + m.rows[0].z * v.z,
        m.rows[1].x * v.x + m.rows[1].y * v.y + m.rows[1].z * v.z,
        m.rows[2].x * v.x + m.rows[2].y * v.y + m.rows[2].z * v.z
    };
}

typedef struct OBB {
	AF_FLOAT c[3]; // OBB center point
	Vec3 u[3]; // Local x-, y-, and z-axes
	AF_FLOAT e[3]; // Positive halfwidth extents of OBB along each axis
} OBB;

// Forward Declareations
// Physics system functions
af_bool_t AF_Physics_Collision_Test(AF_ECS* _ecs);
void AF_Physics_GetInterval(const AF_CTransform3D* transform, const Vec3* halfSize, const Vec3* axis, AF_FLOAT* min, AF_FLOAT* max);
af_bool_t AF_Physics_AABB_Test(AF_ECS* _ecs, uint32_t _entity1ID, uint32_t _entity2ID, AF_CTransform3D* transformA, AF_CCollider* colliderA, AF_CTransform3D* transformB, AF_CCollider* colliderB, AF_Collision* outCollision);
af_bool_t AF_Physics_OBB_Test(AF_ECS* _ecs, uint32_t _entity1ID, uint32_t _entity2ID, AF_CTransform3D* transformA, AF_CCollider* colliderA, AF_CTransform3D* transformB, AF_CCollider* colliderB, AF_Collision* outCollision);
af_bool_t AF_Physics_TerrainTest(AF_ECS* _ecs, uint32_t _entity1ID, AF_CTransform3D* _entity1Transform, AF_CCollider* _collider1, AF_CTerrain* _terrain, AF_CTransform3D* _terrainTransform);
af_bool_t AF_Physics_Point_Inside_Rect(Vec2 _point, AF_Rect _rect);
af_bool_t AF_Physics_Plane_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision);
af_bool_t AF_Physics_CollisionInfoLessThan(const AF_Collision* info1, const AF_Collision* info2);


//=====HELPER FUNCTIONS=====
Vec3 AF_Physics_CalculateBoxInverseInertiaTensor(Vec3 halfExtents, float inverseMass);
Mat4 AF_Physics_TransformInertiaTensorToWorldSpace(Vec3 localInertia, Mat4 modelMat);
void AF_Physics_ApplyAngularImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force, Mat4 _worldInertia);
void AF_Physics_ApplyLinearImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force);
AF_LIB_API void AF_Physics_IntegrateVelocity(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt);
AF_LIB_API void AF_Physics_IntegrateAccell(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt);
af_bool_t AF_Physics_Sphere_RayIntersection(const Ray* _ray, const AF_CTransform3D* _transform, const AF_CCollider* _collider, AF_Collision* _collision);
af_bool_t AF_Physics_Box_RayIntersection(const Ray* _ray, const Vec3 _boxPos, const Vec3 _boxSize, AF_Collision* _collision);
af_bool_t AF_Physics_AABB_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision);
af_bool_t AF_Physics_OBB_RayIntersection(const Ray* _ray, const AF_CTransform3D* _worldTransform, const Vec3* _size, AF_Collision* _collision);
af_bool_t AF_Physics_AABB(AF_Rect* _rect1, AF_Rect* _rect2);
af_bool_t AF_Physics_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision);
AF_LIB_API void AF_Physics_ResolveCollision(AF_ECS* _ecs, uint32_t _entityAID, uint32_t _entityBID, AF_Collision* _collision);
void AF_Physics_UpdateBroadphaseAABB(AF_CCollider* _collider);
void AF_Physics_NarrowPhase(AF_Collision* broadPhaseCollisions, size_t collisionCount, int numCollisionFrames);
af_bool_t AF_Physics_Raycast(const Ray* _ray, AF_ECS* _ecs, void* _physicsEngineHandle, AF_Collision* _collision) ;
void AF_Physics_DrawBox(AF_CCollider* collider, float* color);
AF_FLOAT AF_Physics_TransformToAxis(const AF_CTransform3D* transform, const Vec3* halfSize, Vec3 axis);
void AF_Physics_DetectBoxAndPoint(AF_CCollider* _boxCollider, const Vec3* _point, AF_Collision* outCollision);

// Math Helpers for Quaternion integration
Vec4 createQuaternionFromAngularVelocity(Vec3 angVel, float dt);
Vec4 Quat_MULT(Vec4 q1, Vec4 q2);
Mat4 QuaternionToMat4(Vec4 q);
Mat4 Mat4_ToModelMat4_Quaternion(Vec3 pos, Vec4 quat, Vec3 scale);
Vec4 AF_EulerToQuaternion(Vec3 euler);

/*
====================
AF_Physics_Init
Implementation of Init
====================
*/
void AF_Physics_Init(AF_ECS* _ecs, void** _physicsEngineHandle){
	assert(_ecs != NULL && "Physics: Physics_Init pass in a null reference\n");
	AF_Log("Physics_Init: \n");
	(void)_physicsEngineHandle;


	// Setup Broadphase physics
	/*
	AF_Physics_UpdateBroadphaseAABB
	||
	AF_Physics_BroadPhase
	||
	AF_Physics_NarrowPhase
	
	for(int i = 0; i < _ecs->entitiesCount; ++i){
		//AF_Physics_UpdateBroadphaseAABB(&_ecs->colliders[i]);
	}

	//AF_Physics_BroadPhase(_ecs);

	for(int i = 0; i < _ecs->entitiesCount; ++i){
		//AF_Physics_NarrowPhase(&_ecs->colliders[i].collision, _ecs->entitiesCount, 1);
	}*/
	

}

/*
====================
AF_Physics_Update_Bounds
Update the physics bounds
====================
*/
void AF_Physics_Update_Bounds(AF_ECS* _ecs)
{
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
	AF_CCollider* collider = &_ecs->colliders[i];
		// update the bounds position
		// update the bounding Pos
		
		collider->boundingPos = Vec3_ADD(_ecs->transforms[i].pos, collider->posOffset);
		collider->boundingRot = _ecs->transforms[i].rot;
	}
}

/*
====================
AF_Physics_Update
Implementation of update
====================
*/
void AF_Physics_Update(AF_ECS* _ecs, void* _physicsEngineHandle, const float _dt){
	assert(_ecs != NULL && "Physics: AF_Physics_Update pass in a null reference\n");
	(void)_physicsEngineHandle;
	// loop through and update all transforms based on their velocities
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_CTransform3D* transform = &_ecs->transforms[i];

		assert(transform != NULL && "Physics: AF_Physics_Update transform is null\n");
		
		AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i];

		

		if((AF_Component_GetHasEnabled(rigidbody->enabled) == AF_TRUE)) {
		
			//debgf("Physics: upate: velocity x: %f y: %f z: %f\n", rigidbody->velocity.x, rigidbody->velocity.y, rigidbody->velocity.z);
			// if the object isn't static
			if(rigidbody->inverseMass > 0 || rigidbody->isKinematic == AF_TRUE){
				AF_Physics_IntegrateAccell(transform, rigidbody, _dt);
				AF_Physics_IntegrateVelocity(transform, rigidbody, _dt);  
			}

		}

		AF_CCollider* collider = &_ecs->colliders[i];
		// update the bounds position
		// update the bounding Pos
		// Update the bounding pos
		collider->boundingPos = Vec3_ADD(transform->pos, collider->posOffset);
		collider->boundingRot = transform->rot;
		// Update the bounding rotation

		// clear all collsision except keep the callback
		AF_Collision_Reset(&collider->collision);
	} 
}

/*
====================
AF_Physics_LateUpdate
Implementation of late update
====================
*/
void AF_Physics_LateUpdate(AF_ECS* _ecs, void* _physicsEngineHandle){
	assert(_ecs != NULL && "Physics: AF_Physics_LateUpdate pass in a null reference\n");
	(void)_physicsEngineHandle;

	// Do collision tests
	AF_Physics_Collision_Test(_ecs);

	// call the collision pairs

	// Resolve collision between two objects
}

/*
====================
AF_Physics_LateRenderUpdate
Implementation of late render update
====================
*/
// TODO: figure out if this is still needed or a waste of time
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs, void* _physicsEngineHandle){
	assert(_ecs != NULL && "Physics: AF_Physics_LateRenderUpdate pass in a null reference\n");
	(void)_physicsEngineHandle;
	/*
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_CCollider* collider = &_ecs->colliders[i];
		if(collider->showDebug == AF_FALSE){
			//AF_Log("Physics: LateRenderUpate: not showing debug %i\n", i);
			continue;
		}

		if(collider->collision.collided == AF_FALSE){
			//AF_Log("Physics: LateRenderUpate: not colided\n");
			continue;
		}
		//AF_Log("Physics: LateRenderUpate: draw debug\n");
		//AF_Physics_DrawBox(collider, collisionColor);	
	}
		*/
}

/*
====================
AF_Physics_GetInterval
Helper function for SAT. Projects an OBB onto an axis and returns the min/max interval.
====================
*/
void AF_Physics_GetInterval(const AF_CTransform3D* transform, const Vec3* halfSize, const Vec3* axis, AF_FLOAT* min, AF_FLOAT* max) {
    Vec3 corners[8];
    corners[0] = (Vec3){-halfSize->x, -halfSize->y, -halfSize->z};
    corners[1] = (Vec3){-halfSize->x, -halfSize->y,  halfSize->z};
    corners[2] = (Vec3){-halfSize->x,  halfSize->y, -halfSize->z};
    corners[3] = (Vec3){-halfSize->x,  halfSize->y,  halfSize->z};
    corners[4] = (Vec3){ halfSize->x, -halfSize->y, -halfSize->z};
    corners[5] = (Vec3){ halfSize->x, -halfSize->y,  halfSize->z};
    corners[6] = (Vec3){ halfSize->x,  halfSize->y, -halfSize->z};
    corners[7] = (Vec3){ halfSize->x,  halfSize->y,  halfSize->z};

    Mat4 modelMat = transform->modelMat;
    for (int i = 0; i < 8; ++i) {
		Vec4 corner = {corners[i].x, corners[i].y, corners[i].z, 1.0f};
		Vec4 returnCorners = Mat4_MULT_V4(modelMat, corner);

        corners[i].x = returnCorners.x;
		corners[i].y = returnCorners.y;
		corners[i].z = returnCorners.z;
    }

    *min = *max = Vec3_DOT(corners[0], *axis);

    for (int i = 1; i < 8; ++i) {
        float projection = Vec3_DOT(corners[i], *axis);
        if (projection < *min) *min = projection;
        if (projection > *max) *max = projection;
    }
}

/*
====================
AF_PHYSICS_AABB_Test
Calculate ray intersection hit test
====================
*/
af_bool_t AF_Physics_Collision_Test(AF_ECS* _ecs){
	// TODO:
	// implement cheaper nested for loop
	/*
	https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/4collisiondetection/Physics%20-%20Collision%20Detection.pdf
	for int x = 0; x < lastObject ; ++ x {
		for int y = x +1; y < lastObject ; ++ y ) {
			if ( IsColliding (x , y )) {
				ResolveCollision (x , y )
			}
		}
	}
	*/
	af_bool_t returnValue = AF_FALSE;
	// Get the terrain if it exists
	AF_CTerrain* terrain = NULL;
	uint32_t terrainEntityID = 0;
	for(uint32_t t = 0; t < AF_ECS_TOTAL_ENTITIES; ++t){
		AF_CTerrain* terrainCheck = &_ecs->terrains[t];
		if(AF_Component_GetHasEnabled(terrainCheck->enabled) == AF_TRUE){
			terrain = terrainCheck;
			terrainEntityID = t;
			break;
		}
	}
	

	// Loop through all entities with colliders
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){

		if(AF_Component_GetHasEnabled(_ecs->colliders[i].enabled) == AF_FALSE){
			continue;
		}
		AF_Entity* entity1 = &_ecs->entities[i];
		uint32_t entity1ID = i;//AF_ECS_GetID(entity1->id_tag);
		AF_CCollider* collider1 = &_ecs->colliders[entity1ID];
		AF_CTransform3D* entity1Transform = &_ecs->transforms[entity1ID];


		
		// Check against terrain
		if(terrain != NULL){
			af_bool_t terrainCollision = AF_Physics_TerrainTest(_ecs, entity1ID, entity1Transform, collider1, terrain, &_ecs->transforms[terrainEntityID]);
			if(terrainCollision == AF_TRUE){
				//AF_Log("Physics: Collision detected between entity %i and terrain\n", entity1ID);
				returnValue = AF_TRUE;

				// Resolve the collision with the terrain
				AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[entity1ID];
				if(rigidbody->isKinematic == AF_FALSE){
					AF_Physics_ResolveCollision(_ecs, entity1ID, terrainEntityID, &collider1->collision);
					// FIX: Re-sync collider position immediately after modification
					collider1->boundingPos = Vec3_ADD(entity1Transform->pos, collider1->posOffset);
				}
			}
		}
		
		
		// Test all other entities which have colliders
		for(uint32_t x = i + 1; x < _ecs->entitiesCount; ++x){
			// only test enabled colliders
			if(AF_Component_GetHasEnabled(_ecs->colliders[x].enabled) == AF_FALSE){
				continue;
			}

			// check self
			if(i == x){
				continue;
			}

			

			// Check against other colliders
			AF_Entity* entity2 = &_ecs->entities[x];
			uint32_t entity2ID = x;//AF_ECS_GetID(entity2->id_tag);
			AF_CCollider* collider2 = &_ecs->colliders[entity2ID];
			AF_CTransform3D* entity2Transform = &_ecs->transforms[entity2ID];
		
			// only check colliders that can collide i.e. have a bounding volume
			if(collider1->boundingVolume.x == 0 && collider1->boundingVolume.y == 0 && collider1->boundingVolume.z == 0){
				continue;
			}

			if(collider2->boundingVolume.x == 0 && collider2->boundingVolume.y == 0 && collider2->boundingVolume.z == 0){
				continue;
			}

			AF_Collision collisionResult;
            memset(&collisionResult, 0, sizeof(AF_Collision));

			


			// OBB vs OBB test
            af_bool_t collisionSuccess = AF_Physics_OBB_Test(_ecs, entity1ID, entity2ID, entity1Transform, collider1, entity2Transform, collider2, &collisionResult);
            if(collisionSuccess == AF_FALSE){
                continue;
            }

            // --- FIX: Add the collision handling logic here ---

            // 1. Preserve the callback pointers before overwriting
            void (*callback1)(AF_Collision*) = collider1->collision.callback;
            void (*callback2)(AF_Collision*) = collider2->collision.callback;

            // 2. Copy the collision result into each collider's data structure.
            collider1->collision = collisionResult;
            collider2->collision = collisionResult;
            
            // 3. Restore the callback pointers
            collider1->collision.callback = callback1;
            collider2->collision.callback = callback2;
            
            // Invert the normal for the second collider.
            collider2->collision.normal = Vec3_MULT_SCALAR(collisionResult.normal, -1.0f);

            // 4. Invoke the callbacks if they exist.
            if(collider1->collision.callback != NULL){
                collider1->collision.ecsPtr = _ecs;
                collider1->collision.callback(&collider1->collision);
            }
            if(collider2->collision.callback != NULL){
                collider2->collision.ecsPtr = _ecs;
                collider2->collision.callback(&collider2->collision);
            }

            // 5. Resolve the collision for non-kinematic objects.
            AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i];
            if(rigidbody->isKinematic == AF_FALSE){
                // Pass the correct collision data to the resolver.
                AF_Physics_ResolveCollision(_ecs, entity1ID, entity2ID, &collider1->collision);
				// FIX: Re-sync collider position immediately after modification
				collider1->boundingPos = Vec3_ADD(entity1Transform->pos, collider1->posOffset);
				collider2->boundingPos = Vec3_ADD(entity2Transform->pos, collider2->posOffset);
            }
		}
	}
		
	return returnValue;
}

/*
====================
AF_Physics_AABB_Test
Performs an AABB vs AABB collision test 
Returns AF_TRUE if a collision occurs.
====================
*/
af_bool_t AF_Physics_AABB_Test(AF_ECS* _ecs, uint32_t _entity1ID, uint32_t _entity2ID, AF_CTransform3D* transformA, AF_CCollider* colliderA, AF_CTransform3D* transformB, AF_CCollider* colliderB, AF_Collision* outCollision) {
    if(transformA == NULL || transformB == NULL || outCollision == NULL) return AF_FALSE;
	af_bool_t returnValue = AF_FALSE;
	Vec3* posA = &colliderA->boundingPos;
	Vec3* posB = &colliderB->boundingPos;


	Vec3 delta = Vec3_MINUS(*posA, *posB);
	//ec3 totalSize = Vec3_ADD(halfSizeA, halfSizeB);
	// Orient the bounding volumes)
	Vec3 boundingVolumeA = colliderA->boundingVolume;//Vec3_MULT(colliderA->boundingVolume, colliderA->boundingRot);
	Vec3 boundingVolumeB = colliderB->boundingVolume;//Vec3_MULT(colliderB->boundingVolume, colliderB->boundingRot);;
	Vec3 totalSize = Vec3_ADD(boundingVolumeA, boundingVolumeB);
	//Vec3 totalSize = Vec3_ADD(scaledHalfSizeA, scaledHalfSizeB);

	if(
		fabsf(delta.x) < totalSize.x  &&
		fabsf(delta.y) < totalSize.y && 
		fabsf(delta.z) < totalSize.z){

			// TODO: determine from what direction the collision occurs
			//ResolveCollision(_ecs, i, x);
			returnValue = AF_TRUE;
			// Resolve collision
			//AF_PHYSICS_CUBE_COLLISION_FACES
			// Get the min and max of each cube
			// Correct way to find min/max corners=
			Vec3 minA = Vec3_MINUS(*posA, boundingVolumeA);
			Vec3 maxA = Vec3_ADD(*posA, boundingVolumeA);

			Vec3 minB = Vec3_MINUS(*posB, boundingVolumeB);
			Vec3 maxB = Vec3_ADD(*posB, boundingVolumeB);

			
			float distances [FACES_COUNT];
			
			distances[0] = maxB.x - minA.x; // distance of box ’b ’ to ’ left ’ of ’a ’.
			distances[1] = maxA.x - minB.x; // distance of box ’b ’ to ’ right ’ of ’a ’.
			distances[2] = maxB.y - minA.y; // distance of box ’b ’ to ’ bottom ’ of ’a ’.
			distances[3] = maxA.y - minB.y; // distance of box ’b ’ to ’ top ’ of ’a ’.
			distances[4] = maxB.z - minA.z; // distance of box ’b ’ to ’ far ’ of ’a ’.
			distances[5] = maxA.z - minB.z;  // distance of box ’b ’ to ’ near ’ of ’a ’.
			
			float penetration = AF_FLOAT_MAX;
			Vec3 bestAxis = {0,0,0};	// default value
			for(int j = 0; j < FACES_COUNT; ++j){
				if(distances[j] < penetration){
					penetration = distances[j];
					bestAxis = AF_PHYSICS_CUBE_COLLISION_FACES[j]; 
				}
			}
			
			// Find the point on box A closest to box B's center, and vice-versa
			Vec3 closestPointA = Vec3_CLAMP(*posB, minA, maxA);
			Vec3 closestPointB = Vec3_CLAMP(*posA, minB, maxB);

			// The contact point is the average of these two closest points
			Vec3 contactPoint = Vec3_MULT_SCALAR(Vec3_ADD(closestPointA, closestPointB), 0.5f);

			// copy the new struct values to each collider
			// Collision 1
			AF_Collision* collision1 = &colliderA->collision;
			AF_Collision* collision2 = &colliderB->collision;
			collision1->collided = AF_TRUE;
			collision1->entity1ID = _entity1ID;
			collision1->entity2ID = _entity2ID;
			collision1->penetration = penetration;
			collision1->normal = bestAxis;
			collision1->collisionPoint = contactPoint;

			collision2->collided = AF_TRUE;
			collision2->entity1ID = _entity1ID;
			collision2->entity2ID = _entity2ID;
			collision2->penetration = penetration;
			collision2->normal = Vec3_MULT_SCALAR(bestAxis, -1);
			collision2->collisionPoint = contactPoint;

			//AF_Log("AF_Physics_AABB_Test: collision detected between entity id_tags: %i and %i\n", i, x);
			if(collision1->callback != NULL){
				collision1->ecsPtr = _ecs;
				collision1->callback(collision1);
			}else{
				AF_Log_Warning("AF_Physics_AABB_Test: collision detected but no callback set on entity id_tag: %i\n", _entity1ID);
			}

			if(collision2->callback != NULL){
				collision2->ecsPtr = _ecs;
				collision2->callback(collision2);
			}else{
				AF_Log_Warning("AF_Physics_AABB_Test: collision detected but no callback set on entity id_tag: %i\n", _entity2ID);
			}

			
			returnValue = AF_TRUE;
			// Apply collision resolution
			//AF_CTransform3D* transform1 = &_ecs->transforms[i];
			//AF_CTransform3D* transform2 = &_ecs->transforms[x];

			//AF_C3DRigidbody* rigidbody1 = &_ecs->rigidbodies[i];
			//AF_C3DRigidbody* rigidbody2 = &_ecs->rigidbodies[x];
			// don't apply force for kinematic objects
			
	}
	return returnValue;
}

// --- FIX: Helper function for OBB test cross products ---
static af_bool_t TestCrossAxis(Vec3 axis1, Vec3 axis2, float ra, float rb, float expr, float* minPenetration, Vec3* bestAxis) {
    float penetration = (ra + rb) - fabsf(expr);
    if (penetration < 0) {
        return AF_FALSE; // Found a separating axis
    }
    if (penetration < *minPenetration) {
        *minPenetration = penetration;
        Vec3 cross = Vec3_CROSS(axis1, axis2);
        // Only update the best axis if the cross product is not a zero vector
        if (Vec3_MAGNITUDE_SQ(cross) > AF_PHYSICS_EPSILON * AF_PHYSICS_EPSILON) {
            *bestAxis = Vec3_NORMALIZE(cross);
        }
    }
    return AF_TRUE; // No separation found on this axis
}

/*
====================
AF_Physics_OBB_Test
Performs an OBB vs OBB collision test using the Separating Axis Theorem (SAT).
Returns AF_TRUE if a collision occurs.
// Implementation adapted from "Real-Time Collision Detection" by Christer Ericson.
https://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
====================
*/

af_bool_t AF_Physics_OBB_Test(AF_ECS* _ecs, uint32_t _entity1ID, uint32_t _entity2ID, AF_CTransform3D* transformA, AF_CCollider* colliderA, AF_CTransform3D* transformB, AF_CCollider* colliderB, AF_Collision* outCollision) {
    if(_ecs == NULL || transformA == NULL || outCollision == NULL) return AF_FALSE;
    AF_FLOAT ra, rb;
    Mat3 R, AbsR;
    
    OBB a, b;

    // Initialize OBBs with correct HALF-EXTENTS
    a.c[0] = colliderA->boundingPos.x; a.c[1] = colliderA->boundingPos.y; a.c[2] = colliderA->boundingPos.z;
    a.u[0] = Vec3_NORMALIZE(Mat4_GetDirection(transformA->modelMat, 0));
    a.u[1] = Vec3_NORMALIZE(Mat4_GetDirection(transformA->modelMat, 1));
    a.u[2] = Vec3_NORMALIZE(Mat4_GetDirection(transformA->modelMat, 2));
	// boundingVolume should store half-extents (distance from center to edge)
    // For a mesh with vertices from -1 to +1, boundingVolume should be [1, 1, 1]
    // Apply scale to account for transform scaling
    a.e[0] = colliderA->boundingVolume.x * transformA->scale.x;
    a.e[1] = colliderA->boundingVolume.y * transformA->scale.y;
    a.e[2] = colliderA->boundingVolume.z * transformA->scale.z;

    b.c[0] = colliderB->boundingPos.x; b.c[1] = colliderB->boundingPos.y; b.c[2] = colliderB->boundingPos.z;
    b.u[0] = Vec3_NORMALIZE(Mat4_GetDirection(transformB->modelMat, 0));
    b.u[1] = Vec3_NORMALIZE(Mat4_GetDirection(transformB->modelMat, 1));
    b.u[2] = Vec3_NORMALIZE(Mat4_GetDirection(transformB->modelMat, 2));
    // boundingVolume should store half-extents (distance from center to edge)
    // For a mesh with vertices from -1 to +1, boundingVolume should be [1, 1, 1]
    // Apply scale to account for transform scaling
    b.e[0] = colliderB->boundingVolume.x * transformB->scale.x;
    b.e[1] = colliderB->boundingVolume.y * transformB->scale.y;
    b.e[2] = colliderB->boundingVolume.z * transformB->scale.z;

    // Compute rotation matrix expressing b in a's coordinate frame
    for(uint32_t i = 0; i < 3; ++i)
        for(uint32_t j = 0; j < 3; ++j)
            R.m[i][j] = Vec3_DOT(a.u[i], b.u[j]);

    // Compute translation vector t
    Vec3 t_world = (Vec3){b.c[0] - a.c[0], b.c[1] - a.c[1], b.c[2] - a.c[2]};
    // Bring translation into a's coordinate frame
    AF_FLOAT t[3] = {Vec3_DOT(t_world, a.u[0]), Vec3_DOT(t_world, a.u[1]), Vec3_DOT(t_world, a.u[2])};

    float minPenetration = AF_FLOAT_MAX;
    Vec3 bestAxis = {0,0,0};

    // Add epsilon to counteract arithmetic errors
    for(uint32_t i = 0; i < 3; ++i)
        for(uint32_t j = 0; j < 3; ++j)
            AbsR.m[i][j] = fabsf(R.m[i][j]) + AF_PHYSICS_EPSILON;

    // Test axes L = A0, L = A1, L = A2 (Face normals of A)
    for(uint32_t i = 0; i < 3; ++i){
        ra = a.e[i];
        rb = b.e[0] * AbsR.m[i][0] + b.e[1] * AbsR.m[i][1] + b.e[2] * AbsR.m[i][2];
        float penetration = (ra + rb) - fabsf(t[i]);
        if(penetration < 0) return AF_FALSE;
        if(penetration < minPenetration){
            minPenetration = penetration;
            bestAxis = a.u[i];
        }
    }

    // Test axes L = B0, L = B1, L = B2 (Face normals of B)
    for(uint32_t i = 0; i < 3; ++i){
        ra = a.e[0] * AbsR.m[0][i] + a.e[1] * AbsR.m[1][i] + a.e[2] * AbsR.m[2][i];
        rb = b.e[i];
        float penetration = (ra + rb) - fabsf(t[0] * R.m[0][i] + t[1] * R.m[1][i] + t[2] * R.m[2][i]);
        if(penetration < 0) return AF_FALSE;
        if(penetration < minPenetration){
            minPenetration = penetration;
            bestAxis = b.u[i];
        }
    }

    // Test edge-edge axes
    // Test axis L = A0 x B0
    ra = a.e[1] * AbsR.m[2][0] + a.e[2] * AbsR.m[1][0];
    rb = b.e[1] * AbsR.m[0][2] + b.e[2] * AbsR.m[0][1];
    if (!TestCrossAxis(a.u[0], b.u[0], ra, rb, t[2] * R.m[1][0] - t[1] * R.m[2][0], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A0 x B1
    ra = a.e[1] * AbsR.m[2][1] + a.e[2] * AbsR.m[1][1];
    rb = b.e[0] * AbsR.m[0][2] + b.e[2] * AbsR.m[0][0];
    if (!TestCrossAxis(a.u[0], b.u[1], ra, rb, t[2] * R.m[1][1] - t[1] * R.m[2][1], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A0 x B2
    ra = a.e[1] * AbsR.m[2][2] + a.e[2] * AbsR.m[1][2];
    rb = b.e[0] * AbsR.m[0][1] + b.e[1] * AbsR.m[0][0];
    if (!TestCrossAxis(a.u[0], b.u[2], ra, rb, t[2] * R.m[1][2] - t[1] * R.m[2][2], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A1 x B0
    ra = a.e[0] * AbsR.m[2][0] + a.e[2] * AbsR.m[0][0];
    rb = b.e[1] * AbsR.m[1][2] + b.e[2] * AbsR.m[1][1];
    if (!TestCrossAxis(a.u[1], b.u[0], ra, rb, t[0] * R.m[2][0] - t[2] * R.m[0][0], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A1 x B1
    ra = a.e[0] * AbsR.m[2][1] + a.e[2] * AbsR.m[0][1];
    rb = b.e[0] * AbsR.m[1][2] + b.e[2] * AbsR.m[1][0];
    if (!TestCrossAxis(a.u[1], b.u[1], ra, rb, t[0] * R.m[2][1] - t[2] * R.m[0][1], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A1 x B2
    ra = a.e[0] * AbsR.m[2][2] + a.e[2] * AbsR.m[0][2];
    rb = b.e[0] * AbsR.m[1][1] + b.e[1] * AbsR.m[1][0];
    if (!TestCrossAxis(a.u[1], b.u[2], ra, rb, t[0] * R.m[2][2] - t[2] * R.m[0][2], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A2 x B0
    ra = a.e[0] * AbsR.m[1][0] + a.e[1] * AbsR.m[0][0];
    rb = b.e[1] * AbsR.m[2][2] + b.e[2] * AbsR.m[2][1];
    if (!TestCrossAxis(a.u[2], b.u[0], ra, rb, t[1] * R.m[0][0] - t[0] * R.m[1][0], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A2 x B1
    ra = a.e[0] * AbsR.m[1][1] + a.e[1] * AbsR.m[0][1];
    rb = b.e[0] * AbsR.m[2][2] + b.e[2] * AbsR.m[2][0];
    if (!TestCrossAxis(a.u[2], b.u[1], ra, rb, t[1] * R.m[0][1] - t[0] * R.m[1][1], &minPenetration, &bestAxis)) return AF_FALSE;

    // Test axis L = A2 x B2
    ra = a.e[0] * AbsR.m[1][2] + a.e[1] * AbsR.m[0][2];
    rb = b.e[0] * AbsR.m[2][1] + b.e[1] * AbsR.m[2][0];
    if (!TestCrossAxis(a.u[2], b.u[2], ra, rb, t[1] * R.m[0][2] - t[0] * R.m[1][2], &minPenetration, &bestAxis)) return AF_FALSE;

    // No separating axis found, the OBBs must be intersecting.
    // Ensure the collision normal points from A to B for the resolver.
    if (Vec3_DOT(bestAxis, t_world) < 0.0f) {
        bestAxis = Vec3_MULT_SCALAR(bestAxis, -1.0f);
    }

	// Build contact manifold (approximate contact point)


	
    // Calculate contact point using OBB centers (which include the collider offset)
    Vec3 centerA = (Vec3){a.c[0], a.c[1], a.c[2]};
    Vec3 closestPointOnA = centerA;
    for (int i = 0; i < 3; i++) {
        float dist = Vec3_DOT(t_world, a.u[i]);
        if (dist > a.e[i]) dist = a.e[i];
        if (dist < -a.e[i]) dist = -a.e[i];
        closestPointOnA = Vec3_ADD(closestPointOnA, Vec3_MULT_SCALAR(a.u[i], dist));
    }
    Vec3 centerB = (Vec3){b.c[0], b.c[1], b.c[2]};
    Vec3 closestPointOnB = centerB;
    for (int i = 0; i < 3; i++) {
        float dist = Vec3_DOT(Vec3_MULT_SCALAR(t_world, -1.0f), b.u[i]);
        if (dist > b.e[i]) dist = b.e[i];
        if (dist < -b.e[i]) dist = -b.e[i];
        closestPointOnB = Vec3_ADD(closestPointOnB, Vec3_MULT_SCALAR(b.u[i], dist));
    }
    Vec3 contactPoint = Vec3_MULT_SCALAR(Vec3_ADD(closestPointOnA, closestPointOnB), 0.5f);
	

    // Populate the output struct
    outCollision->collided = AF_TRUE;
    outCollision->entity1ID = _entity1ID;
    outCollision->entity2ID = _entity2ID;
    outCollision->penetration = minPenetration;
    outCollision->normal = bestAxis;
    outCollision->collisionPoint = contactPoint;

    return AF_TRUE;
}

/*
====================
AF_Physics_TerrainTest
Test the entity against the terrain
====================
*/
af_bool_t AF_Physics_TerrainTest(AF_ECS* _ecs, uint32_t _entity1ID, AF_CTransform3D* _entity1Transform, AF_CCollider* _collider1, AF_CTerrain* _terrain, AF_CTransform3D* _terrainTransform){
	af_bool_t returnValue = AF_FALSE;
	
	if(_terrain == NULL || _terrain->heightMapData == NULL){
		AF_Log_Warning("AF_Physics_TerrainTest: No terrain or heightmap data found\n");
		return returnValue;
	}
	
	// 1: Calculate the total world size including transform scale
	AF_FLOAT terrainWorldSizeX = (AF_FLOAT)_terrain->numChunks * _terrain->gridScale * _terrainTransform->scale.x; 
	AF_FLOAT terrainWorldSizeZ = (AF_FLOAT)_terrain->numChunks * _terrain->gridScale * _terrainTransform->scale.z; 
	AF_FLOAT halfSizeX = terrainWorldSizeX * 0.5f;
	AF_FLOAT halfSizeZ = terrainWorldSizeZ * 0.5f;


	// 2: Map the world position (X,Z) to the normalised UV (0.0 to 1.0)
	AF_FLOAT u = (_entity1Transform->pos.x - (_terrainTransform->pos.x - halfSizeX)) / terrainWorldSizeX;
	AF_FLOAT v = (_entity1Transform->pos.z - (_terrainTransform->pos.z - halfSizeZ)) / terrainWorldSizeZ;

	// 3: boundary check
	if(u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f){
		return returnValue;
	}

	// 3: Bilinear Interpolation (Required to match GPU smooth surface)
    AF_FLOAT fx = u * (AF_FLOAT)(_terrain->heightMapWidth - 1);
    AF_FLOAT fz = v * (AF_FLOAT)(_terrain->heightMapHeight - 1);

    uint32_t x0 = (uint32_t)fx;
    uint32_t x1 = (x0 < _terrain->heightMapWidth - 1) ? x0 + 1 : x0;
    uint32_t z0 = (uint32_t)fz;
    uint32_t z1 = (z0 < _terrain->heightMapHeight - 1) ? z0 + 1 : z0;

    // NO-FLIP: Using raw heightmap coordinates
    uint32_t r0 = z0;
    uint32_t r1 = z1;

    // Sample the 4 surrounding pixels
    float h00 = (float)_terrain->heightMapData[r0 * _terrain->heightMapWidth + x0];
    float h10 = (float)_terrain->heightMapData[r0 * _terrain->heightMapWidth + x1];
    float h01 = (float)_terrain->heightMapData[r1 * _terrain->heightMapWidth + x0];
    float h11 = (float)_terrain->heightMapData[r1 * _terrain->heightMapWidth + x1];

    // Lerp factors
    float tx = fx - (float)x0;
    float tz = fz - (float)z0;

    // Blend the heights
    float top = h00 * (1.0f - tx) + h10 * tx;
    float bottom = h01 * (1.0f - tx) + h11 * tx;
    float blendedRaw = top * (1.0f - tz) + bottom * tz;

    // 4: Final world height calculation
    AF_FLOAT terrainHeight = (blendedRaw / 255.0f) * _terrain->heightScale * _terrainTransform->scale.y; 
    terrainHeight += _terrainTransform->pos.y; 

    // Look at the bottom of the entity's collider
    AF_FLOAT entityBottom = _entity1Transform->pos.y + _collider1->posOffset.y - _collider1->boundingVolume.y;

    if(entityBottom < terrainHeight){
        returnValue = AF_TRUE;

		// Log detailed collision info
		AF_Log("TERRAIN COLLISION [Entity %u]\n", _entity1ID);
		AF_Log("  Position: [%.2f, %.2f, %.2f]\n", _entity1Transform->pos.x, _entity1Transform->pos.y, _entity1Transform->pos.z);
		AF_Log("  Feet: %.4f | TerrainH: %.4f\n", entityBottom, terrainHeight);
		AF_Log("  UV Mapping: U:%.4f V:%.4f (Pixel: %.1f, %.1f)\n", u, v, fx, fz);
		AF_Log("  Samples: h00=%.1f h10=%.1f h01=%.1f h11=%.1f (BlendedRaw: %.2f)\n", h00, h10, h01, h11, blendedRaw);
		AF_Log("  Penetration: %.4f\n", terrainHeight - entityBottom);

		// populate collision data
		AF_Collision* collision1 = &_collider1->collision;
		collision1->collided = AF_TRUE;
		collision1->entity1ID = _entity1ID;
		collision1->entity2ID = UINT32_MAX; 
		// The penetration is the distance to move the ENTITY UP to clear the terrain
		collision1->penetration = terrainHeight - entityBottom; 
		collision1->normal = (Vec3){0, -1, 0}; // Normal pointing DOWN so ResolveCollision moves us UP
		collision1->collisionPoint = (Vec3){_entity1Transform->pos.x, terrainHeight, _entity1Transform->pos.z};

		if(collision1->callback != NULL){
			collision1->ecsPtr = _ecs;
			collision1->callback(collision1);
		}else{
			AF_Log_Warning("AF_Physics_TerrainTest: collision detected but no callback set on entity id_tag: %i\n", _entity1ID);
		}
	}
	return returnValue;
}

/*
====================
AF_Physics_CalculateBoxInverseInertiaTensor
Calculate the inverse inertia tensor for a box based on its dimensions and mass.
For a box (cuboid), the inertia tensor diagonal is:
I_x = (1/12) * mass * (height² + depth²)
I_y = (1/12) * mass * (width² + depth²)
I_z = (1/12) * mass * (width² + height²)
We return the INVERSE for easier physics calculations.
====================
*/
Vec3 AF_Physics_CalculateBoxInverseInertiaTensor(Vec3 halfExtents, float inverseMass) {
	if (inverseMass == 0.0f) {
		// Static object - infinite inertia (zero inverse inertia)
		Vec3 vecZero = {0.0f, 0.0f, 0.0f};
		return vecZero;
	}
	
	float mass = 1.0f / inverseMass;
	
	// Full extents (width, height, depth)
	float width = halfExtents.x * 2.0f;
	float height = halfExtents.y * 2.0f;
	float depth = halfExtents.z * 2.0f;
	
	// Inertia tensor diagonal elements for a box
	float I_x = (1.0f / 12.0f) * mass * (height * height + depth * depth);
	float I_y = (1.0f / 12.0f) * mass * (width * width + depth * depth);
	float I_z = (1.0f / 12.0f) * mass * (width * width + height * height);
	
	// Return inverse inertia tensor (avoid division by zero)
	Vec3 inverseInertia;
	inverseInertia.x = (I_x > 0.0001f) ? (1.0f / I_x) : 0.0f;
	inverseInertia.y = (I_y > 0.0001f) ? (1.0f / I_y) : 0.0f;
	inverseInertia.z = (I_z > 0.0001f) ? (1.0f / I_z) : 0.0f;
	
	return inverseInertia;
}

/*
====================
AF_Physics_TransformInertiaTensorToWorldSpace
Transform the local-space inverse inertia tensor to world-space.
I_world = R * I_local * R^T
Where R is the rotation matrix extracted from the model matrix basis vectors.
====================
*/
Mat4 AF_Physics_TransformInertiaTensorToWorldSpace(Vec3 localInertia, Mat4 modelMat) {
	// Extract rotation basis from columns 0, 1, 2
	Vec3 r = Vec3_NORMALIZE(Mat4_GetDirection(modelMat, 0)); // Right (Col 0)
	Vec3 u = Vec3_NORMALIZE(Mat4_GetDirection(modelMat, 1)); // Up    (Col 1)
	Vec3 f = Vec3_NORMALIZE(Mat4_GetDirection(modelMat, 2)); // Fwd   (Col 2)

	// I_world = R * I_diag * R_transpose
	// resulting element M_ij = (r_i*r_j*Ix) + (u_i*u_j*Iy) + (f_i*f_j*Iz)
	
	Mat4 I; 
	float x = localInertia.x;
	float y = localInertia.y;
	float z = localInertia.z;

	// Row 0
	I.rows[0].x = r.x * r.x * x + u.x * u.x * y + f.x * f.x * z;
	I.rows[0].y = r.x * r.y * x + u.x * u.y * y + f.x * f.y * z;
	I.rows[0].z = r.x * r.z * x + u.x * u.z * y + f.x * f.z * z;
	I.rows[0].w = 0;

	// Row 1
	I.rows[1].x = I.rows[0].y; // Symmetric
	I.rows[1].y = r.y * r.y * x + u.y * u.y * y + f.y * f.y * z;
	I.rows[1].z = r.y * r.z * x + u.y * u.z * y + f.y * f.z * z;
	I.rows[1].w = 0;

	// Row 2
	I.rows[2].x = I.rows[0].z; // Symmetric
	I.rows[2].y = I.rows[1].z; // Symmetric
	I.rows[2].z = r.z * r.z * x + u.z * u.z * y + f.z * f.z * z;
	I.rows[2].w = 0;
	
	// Row 3 (Homogeneous part not truly needed but kept consistent with Mat4)
	I.rows[3] = (Vec4){0, 0, 0, 1};

	return I;
}


/*
====================
AF_Physics_ApplyAngularImpulse
Apply angular impulse to rigidbody object.
Note: The world-space inverse inertia tensor MUST be provided.
====================
*/
void AF_Physics_ApplyAngularImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force, Mat4 _worldInertia){
	Vec3 angularForce = AF_Mat4_MULT_Vec3(_worldInertia, _force);
	_rigidbody->anglularVelocity = Vec3_ADD(_rigidbody->anglularVelocity, angularForce);
}

/*
====================
AF_Physics_ApplyLinearImpulse
Apply force to rigidbody object
====================
*/
void AF_Physics_ApplyLinearImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force){
	Vec3 linearForce;
	if(_rigidbody->inverseMass > 0){
		linearForce = Vec3_MULT_SCALAR(_force, _rigidbody->inverseMass);
	}else{
		linearForce = _force;
	}
	 
	_rigidbody->velocity = Vec3_ADD(_rigidbody->velocity, linearForce);
}

/*
====================
AF_PHYSICS_ADDFORCEATPOSITION
Add force at a position
====================
*/
/*
static void AF_Physics_AddForceAtPosition(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const Vec3* _addedForce, const Vec3* _position){
	Vec3 localPos = Vec3_MINUS(*_position, _transform->pos);

	_rigidbody->force = Vec3_ADD(_rigidbody->force, *_addedForce);
	_rigidbody->torque = Vec3_CROSS(localPos, *_addedForce);
}*/

// TODO move this to vec4
// Function to create a quaternion from angular velocity and time step

Vec4 createQuaternionFromAngularVelocity(Vec3 angVel, float dt) {
    // Calculate the scalar component (w) of the quaternion
    float halfDt = dt * 0.5f; // Half of the time step
    float angleMagnitude = Vec3_MAGNITUDE(angVel); // Calculate the magnitude of angular velocity
    
    // Safety check: if angular velocity is too small, return identity quaternion
    if (angleMagnitude < 0.0001f) {
        return Vec4_ZERO(); // Identity quaternion (no rotation)
    }
    
    float halfAngle = angleMagnitude * halfDt;
    float w = cosf(halfAngle); // Scalar part
    float sinHalfAngle = sinf(halfAngle);

    // Calculate the vector part of the quaternion (normalize the axis first)
    float scale = sinHalfAngle / angleMagnitude;
    Vec3 vectorPart = Vec3_MULT_SCALAR(angVel, scale);

    // Create and return the quaternion
    Vec4 q = {
        vectorPart.x,
        vectorPart.y,
        vectorPart.z,
        w
    };
    return q;
}

// Quaternion multiplication (Hamilton product)
// q1 * q2 where q = (x, y, z, w)
Vec4 Quat_MULT(Vec4 q1, Vec4 q2) {
    Vec4 result;
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return result;
}


// Convert Euler angles (in radians) to quaternion
// Standard game engine convention: euler.x=pitch, euler.y=yaw, euler.z=roll
Vec4 AF_EulerToQuaternion(Vec3 euler) {
    float cp = cosf(euler.x * 0.5f);  // pitch
    float sp = sinf(euler.x * 0.5f);
    float cy = cosf(euler.y * 0.5f);  // yaw
    float sy = sinf(euler.y * 0.5f);
    float cr = cosf(euler.z * 0.5f);  // roll
    float sr = sinf(euler.z * 0.5f);

    Vec4 q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    
    return q;
}

// Convert quaternion to 4x4 rotation matrix
Mat4 QuaternionToMat4(Vec4 q) {
    // Normalize the quaternion first
    float mag = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (mag > 0.0001f) {
        q.x /= mag;
        q.y /= mag;
        q.z /= mag;
        q.w /= mag;
    }
    
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    Mat4 mat;
    mat.rows[0].x = 1.0f - 2.0f * (yy + zz);
    mat.rows[0].y = 2.0f * (xy - wz);
    mat.rows[0].z = 2.0f * (xz + wy);
    mat.rows[0].w = 0.0f;

    mat.rows[1].x = 2.0f * (xy + wz);
    mat.rows[1].y = 1.0f - 2.0f * (xx + zz);
    mat.rows[1].z = 2.0f * (yz - wx);
    mat.rows[1].w = 0.0f;

    mat.rows[2].x = 2.0f * (xz - wy);
    mat.rows[2].y = 2.0f * (yz + wx);
    mat.rows[2].z = 1.0f - 2.0f * (xx + yy);
    mat.rows[2].w = 0.0f;

    mat.rows[3].x = 0.0f;
    mat.rows[3].y = 0.0f;
    mat.rows[3].z = 0.0f;
    mat.rows[3].w = 1.0f;

    return mat;
}


// Build model matrix from position, quaternion rotation, and scale
Mat4 Mat4_ToModelMat4_Quaternion(Vec3 pos, Vec4 quat, Vec3 scale) {
    // Get rotation matrix from quaternion
    Mat4 rot = QuaternionToMat4(quat);
    
    // Apply scale and position
    Mat4 result;
    result.rows[0].x = rot.rows[0].x * scale.x;
    result.rows[0].y = rot.rows[0].y * scale.y;
    result.rows[0].z = rot.rows[0].z * scale.z;
    result.rows[0].w = 0.0f;

    result.rows[1].x = rot.rows[1].x * scale.x;
    result.rows[1].y = rot.rows[1].y * scale.y;
    result.rows[1].z = rot.rows[1].z * scale.z;
    result.rows[1].w = 0.0f;

    result.rows[2].x = rot.rows[2].x * scale.x;
    result.rows[2].y = rot.rows[2].y * scale.y;
    result.rows[2].z = rot.rows[2].z * scale.z;
    result.rows[2].w = 0.0f;

    result.rows[3].x = pos.x;
    result.rows[3].y = pos.y;
    result.rows[3].z = pos.z;
    result.rows[3].w = 1.0f;

    return result;
}

/*
====================
AF_PHYSICS_INTEGRATEVELOCITY
Integrate the position and some dampening into the velocity
====================
*/

void AF_Physics_IntegrateVelocity(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt){
	//float frameDamping = powf ( DAMPING_FACTOR, _dt);

	Vec3 position = _transform->pos;
	Vec3 linearVelocity = _rigidbody->velocity;
	Vec3 linearDT = Vec3_MULT_SCALAR(linearVelocity, _dt);
	position = Vec3_ADD(position, linearDT);
	_transform->pos = position;

	// LinearDamping
	//_rigidbody->velocity = Vec3_MULT_SCALAR(_rigidbody->velocity, LINEAR_DAMPING);
	

	// Angular velocity and orientation
	Vec4 orientation = _transform->orientation;
	Vec3 angVel = _rigidbody->anglularVelocity;

	// Only integrate if there's significant angular velocity
	float angVelMag = Vec3_MAGNITUDE(angVel);
	if (angVelMag > 0.0001f) {
		// Create a quaternion representing the rotation from angular velocity
		Vec4 quatAngVel = createQuaternionFromAngularVelocity(angVel, _dt);
		// Apply the rotation: new_orientation = delta_rotation * current_orientation
		orientation = Quat_MULT(quatAngVel, orientation);
		orientation = Vec4_NORMALIZE(orientation);
		_transform->orientation = orientation;
		
		// ONLY convert quaternion back to Euler when the object actually rotates
		// This prevents fighting with other systems (like camera controllers) that set Euler angles directly
		// Using standard game engine convention: rot.x=pitch, rot.y=yaw, rot.z=roll
		float w = orientation.w, x = orientation.x, y = orientation.y, z = orientation.z;
		
		// Pitch (x-axis rotation) - looking up/down
		float sinp = 2.0f * (w * x + y * z);
		// Clamp to prevent NaN from asinf
		sinp = (sinp > 1.0f) ? 1.0f : ((sinp < -1.0f) ? -1.0f : sinp);
		_transform->rot.x = asinf(sinp) * (180.0f / AF_PI);
		
		// Yaw (y-axis rotation) - turning left/right
		float siny_cosp = 2.0f * (w * y - z * x);
		float cosy_cosp = 1.0f - 2.0f * (x * x + y * y);
		_transform->rot.y = atan2f(siny_cosp, cosy_cosp) * (180.0f / AF_PI);
		
		// Roll (z-axis rotation) - tilting side to side
		float sinr_cosp = 2.0f * (w * z + x * y);
		float cosr_cosp = 1.0f - 2.0f * (y * y + z * z);
		_transform->rot.z = atan2f(sinr_cosp, cosr_cosp) * (180.0f / AF_PI);
	}
	
	// FIX: Build the model matrix directly from the quaternion orientation
	// This prevents Euler-based drift from accumulating during physical simulation
	_transform->modelMat = Mat4_ToModelMat4_Quaternion(_transform->pos, _transform->orientation, _transform->scale);

	// Apply frame-rate independent damping using exponential decay: damping^dt
	// This ensures consistent behavior regardless of frame rate
	float linearDampingFactor = powf(LINEAR_DAMPING, _dt);
	float angularDampingFactor = powf(ANGULAR_DAMPING, _dt);
	
	float angularSpeed = Vec3_MAGNITUDE(angVel);
	
	// Apply adaptive angular damping - stronger when rotating slowly to help settle
	const float slowRotationThreshold = 0.5f; // radians per second
	if (angularSpeed < slowRotationThreshold && angularSpeed > 0.0f) {
		// Interpolate damping strength based on rotation speed
		float dampingStrength = angularSpeed / slowRotationThreshold; // 0 to 1
		// Use stronger damping (0.90) for slow rotation, normal (ANGULAR_DAMPING) for fast
		float adaptiveDamping = 0.90f + (dampingStrength * (ANGULAR_DAMPING - 0.90f));
		angularDampingFactor = powf(adaptiveDamping, _dt);
	}
	
	// Apply damping to velocities
	linearVelocity = Vec3_MULT_SCALAR(linearVelocity, linearDampingFactor);
	angVel = Vec3_MULT_SCALAR(angVel, angularDampingFactor);
	
	// Sleep very slow objects to prevent endless micro-movements
	const float sleepLinearThreshold = 0.01f;  // Very low - only stop truly stationary objects
	const float sleepAngularThreshold = 0.02f; // Slightly higher to stop slow rotation sooner
	
	float linearSpeed = Vec3_MAGNITUDE(linearVelocity);
	
	if (linearSpeed < sleepLinearThreshold) {
		linearVelocity = Vec3_ZERO();
	}
	if (angularSpeed < sleepAngularThreshold) {
		angVel = Vec3_ZERO();
	}
	
	_rigidbody->anglularVelocity = angVel;
	_rigidbody->velocity = linearVelocity;
}

/*
====================
AF_PHYSICS_INTEGRATEACCELL
Integrate gravity and acceleration into the velocity
====================
*/
void AF_Physics_IntegrateAccell(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt){
    // iterate over all the game objects
    if(AF_Component_GetEnabled(_rigidbody->enabled) != AF_TRUE){
        return;
    }
    
    float inverseMass = _rigidbody->inverseMass;

    if (inverseMass > 0.0f) {
        // == Linear Acceleration ==
        Vec3 force = _rigidbody->force;
        Vec3 linearAcceleration = Vec3_MULT_SCALAR(force, inverseMass);
        if (_rigidbody->gravity == AF_TRUE) {
            linearAcceleration.y += GRAVITY_SCALE;
        }
        _rigidbody->velocity = Vec3_ADD(_rigidbody->velocity, Vec3_MULT_SCALAR(linearAcceleration, _dt));
        
        // == Angular Acceleration (FIXED) ==
        // 1. Transform local inverse inertia tensor to world space as a full matrix
        Mat4 worldInvInertia = AF_Physics_TransformInertiaTensorToWorldSpace(_rigidbody->inertiaTensor, _transform->modelMat);
        
        // 2. Calculate angular acceleration from torque using matrix-vector multiplication: α = I⁻¹ * τ
        Vec3 angularAcceleration = AF_Mat4_MULT_Vec3(worldInvInertia, _rigidbody->torque);
        
        // 3. Update angular velocity: ω_new = ω_old + α * Δt
        _rigidbody->anglularVelocity = Vec3_ADD(_rigidbody->anglularVelocity, Vec3_MULT_SCALAR(angularAcceleration, _dt));

        // Clear forces and torques after applying them to prevent accumulation
        _rigidbody->force = Vec3_ZERO();
        _rigidbody->torque = Vec3_ZERO();
    }
}

/*
====================
AF_PHYSICS_SPHERE_RAYINTERSECTION
Calculate ray intersection hit test
====================
*/
af_bool_t AF_Physics_Sphere_RayIntersection(const Ray* _ray, const AF_CTransform3D* _transform, const AF_CCollider* _collider, AF_Collision* _collision){
	Vec3 spherePos = _transform->pos;
	//Box* sphereCollisionVolume = (Sphere_CollisionVolume*)_collider->boundingVolume;
	AF_FLOAT sphereRadius = _collider->boundingVolume.x;
	Vec3 direction = Vec3_MINUS(spherePos, _ray->position);
	//Then project the sphere’s origin onto our ray direction vector

	AF_FLOAT sphereProj = Vec3_DOT(direction, _ray->direction);
	
	// Is point behind the ray?
	if(sphereProj < 0.0f) {
		return AF_FALSE; //point is behind the ray!
	}	
	
	//Get closest point on ray line to sphere
	Vec3 point = Vec3_MULT_SCALAR(Vec3_ADD(_ray->position, _ray->direction), sphereProj);
	
	AF_FLOAT sphereDist = Vec3_MAGNITUDE(Vec3_MINUS(point, spherePos));

	if (sphereDist > sphereRadius) { 
		return AF_FALSE;
	}
	AF_FLOAT offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));//AF_Math_Sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

 
	_collision->rayDistance = sphereProj - (offset);
	_collision->collisionPoint = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, _collision->rayDistance));
	return AF_TRUE;
} 

/*
====================
AF_PHYSICS_Box_RAYINTERSECTION
Calculate ray intersection hit test against a box
====================
*/
af_bool_t AF_Physics_Box_RayIntersection(const Ray* _ray, const Vec3 _boxPos, const Vec3 _boxSize, AF_Collision* _collision) {
    Vec3 boxMin = Vec3_MINUS(_boxPos, _boxSize);
    Vec3 boxMax = Vec3_ADD(_boxPos, _boxSize);

    // Initial ray intersection times (t_min and t_max)
    AF_FLOAT t_min = -1.0f;
    AF_FLOAT t_max = -1.0f;
    
    // Check for each dimension (x, y, z)
    for (int i = 0; i < 3; ++i) {
        // Use a temporary array for cleaner access
        AF_FLOAT rayOrigin_i = (i == 0) ? _ray->position.x : ((i == 1) ? _ray->position.y : _ray->position.z);
        AF_FLOAT rayDir_i = (i == 0) ? _ray->direction.x : ((i == 1) ? _ray->direction.y : _ray->direction.z);
        AF_FLOAT boxMin_i = (i == 0) ? boxMin.x : ((i == 1) ? boxMin.y : boxMin.z);
        AF_FLOAT boxMax_i = (i == 0) ? boxMax.x : ((i == 1) ? boxMax.y : boxMax.z);

        // Calculate t1 and t2 for the current slab
        AF_FLOAT t1 = (boxMin_i - rayOrigin_i) / rayDir_i;
        AF_FLOAT t2 = (boxMax_i - rayOrigin_i) / rayDir_i;

        // Ensure t1 is the entry time and t2 is the exit time
        if (t1 > t2) {
            AF_FLOAT temp = t1;
            t1 = t2;
            t2 = temp;
        }

        // Update the overall t_min and t_max
        // t_min is the maximum of all entry times
        // t_max is the minimum of all exit times
        if (i == 0) { // First iteration
            t_min = t1;
            t_max = t2;
        } else {
            t_min = AF_MAX(t_min, t1);
            t_max = AF_MIN(t_max, t2);
        }
    }

    // Check if there is a valid intersection
    // The intersection exists if t_min <= t_max and a part of the intersection is in front of the ray origin (t_max > 0)
    if (t_min > t_max || t_max < 0.0f) {
        //AF_Log("AF_Physics_Box_RayIntersection: no intersection found\n");
        return AF_FALSE;
    }

    // A collision has occurred. The distance is t_min because that's the first time we hit a box face.
    // Calculate the collision point
    _collision->rayDistance = t_min;
    _collision->collisionPoint = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, _collision->rayDistance));

    //AF_Log("AF_Physics_Box_RayIntersection: result TRUE, distance: %f\n", _collision->rayDistance);
    return AF_TRUE;
}

/*
====================
AF_PHYSICS_AABB_RAYINTERSECTION
Calculate ray intersection hit test against an Axis Aligned Bounding Box
====================
*/
af_bool_t AF_Physics_AABB_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){
	Vec3 boxPos = _collider->boundingPos;
	Vec3 boxHalfSize = _collider->boundingVolume;
	return AF_Physics_Box_RayIntersection(_ray, boxPos, boxHalfSize, _collision);
} 

/*
====================
AF_PHYSICS_OBB_RAYINTERSECTION
Calculate ray intersection hit test against a Object Orientated Box
====================
*/
af_bool_t AF_Physics_OBB_RayIntersection(const Ray* _ray, const AF_CTransform3D* _worldTransform, const Vec3* _size, AF_Collision* _collision){
	if(_ray){}
	if(_worldTransform){}
	if(_size){}
	if(_collision){}
	//Vec4 orientation = {_worldTransform->rot.x, _worldTransform->rot.y, _worldTransform->rot.z, 1};
	//Vec3 postion = _worldTransform->pos;
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
	return AF_FALSE;
}

/*
====================
AF_PHYSICS_COLLISION_AABB
Simple aabb rect 
Return AF_TRUE if collission occured
====================
*/
af_bool_t AF_Physics_AABB(AF_Rect* _rect1, AF_Rect* _rect2){
	//https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection#:~:text=AABB%20%2D%20AABB%20collisions,the%20x%20and%20y%20axis.
	af_bool_t returnValue = AF_FALSE;
	// Collision x-axis?
	af_bool_t collisionX = _rect1->w >= _rect2->x &&
		_rect2->w >= _rect1->x;

	// Collision y-axis?
	af_bool_t collisionY = _rect1->h >= _rect2->y &&
		_rect2->h >= _rect1->y;

	if (collisionX && collisionY) {
		returnValue = AF_TRUE;
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
af_bool_t AF_Physics_Point_Inside_Rect(Vec2 _point, AF_Rect _rect){
	af_bool_t returnValue = AF_FALSE;
	if (_point.x >= _rect.x && _point.x <= (_rect.x + _rect.w) &&
        _point.y >= _rect.y && _point.y <= (_rect.y + _rect.h)) {
        returnValue = AF_TRUE;
    }
	return returnValue;
}

/*
====================
AF_PHYSICS_Plane_RAYINTERSECTION
Calculate ray intersection hit test against a Object Orientated Box
====================
*/
af_bool_t AF_Physics_Plane_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){

	Vec3 planePos = _collider->boundingPos;
	Vec3* _size = &_collider->boundingVolume;
	// assume a horizontal plane at planePos.y
	AF_FLOAT t = (planePos.y - _ray->position.y) / _ray->direction.y;

	af_bool_t returnValue = AF_TRUE;

	if(t < 0){
		returnValue = AF_FALSE;  // No intersection (plane is behind the ray or parallel)
		//printf("AF_Physics_Box_RayIntersection: no intersection, plane is behind the ray \n");
		return returnValue;
	}

	Vec3 intersection = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, t));
	

	// Check if the intersection is within the desired plane bounds (if the plane is finite)
	if(_size->y == 0 && _size->x == 0 && _size->z == 0){
		// bounds are all 0 so plane is counted as infinite.
		//printf("AF_Physics_Plane_RayIntersection: intersection on infintie plane at x: %f y: %f z: %f \n", intersection.x, intersection.y, intersection.z);
		returnValue = AF_TRUE;
		_collision->collisionPoint = intersection;
		_collision->rayDistance = t;
		return returnValue;
	}

	// Check if within size/bounds
	Vec2 intersectionPoint = {intersection.x, intersection.z};
	AF_Rect planeRect = {-_size->x/2, -_size->z/2, _size->x, _size->z};
	if(AF_Physics_Point_Inside_Rect(intersectionPoint, planeRect) == AF_FALSE){
		//printf("AF_Physics_Box_RayIntersection: no intersection, ray is out of bounds \nintX: %f intY: %f rectX:%f rectY:%f rectz:%f  \n", intersectionPoint.x, intersectionPoint.y, _size->x, _size->y, _size->z);
		returnValue = AF_FALSE;
		return returnValue;
	}
	
	//printf("AF_Physics_Plane_RayIntersection: intersection within bounds x: %f y: %f z: %f \n", intersection.x, intersection.y, intersection.z);
	returnValue = AF_TRUE;
	
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
af_bool_t AF_Physics_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){
	//const AF_CTransform3D* transform = _entity->transform;
	enum CollisionVolumeType type = _collider->type;

	switch(type){
		case Plane:
			return AF_Physics_Plane_RayIntersection(_ray, _collider, _collision);
		break;
		case AABB:
			return AF_Physics_AABB_RayIntersection(_ray, _collider, _collision);
		break;
		
		case OBB_Type:
			printf("AF_Physics_RayIntersection: OBB ray interaction not implemented\n");
			return AF_FALSE;
		break;

		case Sphere:
			return AF_FALSE;//AF_Physics_Sphere_RayIntersection(_ray, _entity->transform, _entity->collider, _collision);
		break;

		case Mesh:
			printf("AF_Physics_RayIntersection: Mesh ray interaction not implemented\n");
			return AF_FALSE;
		break;

		case Compound:
			printf("AF_Physics_RayIntersection: Compound ray interaction not implemented\n");
			return AF_FALSE;
		break;

		case Invalid:
			printf("AF_Physics_RayIntersection: Invalid collider type\n");
			return AF_FALSE;
		break;
	}
	
	return AF_FALSE;
}

/*
====================
AF_Physics_ImpulseResolveCollision
Resolve collision between two rigidbodies
====================
*/
void AF_Physics_ResolveCollision(AF_ECS* _ecs, uint32_t _entityAID, uint32_t _entityBID, AF_Collision* _collision){
	
	// FIX: Safety check for invalid IDs (like Terrain/World)
	if(_entityBID >= AF_ECS_TOTAL_ENTITIES) {
		// Minimal correction for static world - only move A
		AF_CTransform3D* transformA = &_ecs->transforms[_entityAID];
		AF_CCollider* colliderA = &_ecs->colliders[_entityAID];
		float correctionMagnitude = fmaxf(colliderA->collision.penetration - PENETRATION_SLOP, 0.0f);
		Vec3 correction = Vec3_MULT_SCALAR(colliderA->collision.normal, correctionMagnitude * PENETRATION_PERCENTAGE);
		transformA->pos = Vec3_MINUS(transformA->pos, correction);
		
		// For terrain, we still want to zero out the velocity against the normal
		AF_C3DRigidbody* rigidbodyA = &_ecs->rigidbodies[_entityAID];
		float velocityAlongNormal = Vec3_DOT(rigidbodyA->velocity, colliderA->collision.normal);
		if (velocityAlongNormal > 0.0f) {
			Vec3 impulse = Vec3_MULT_SCALAR(colliderA->collision.normal, -velocityAlongNormal);
			rigidbodyA->velocity = Vec3_ADD(rigidbodyA->velocity, impulse);
		}
		return; 
	}
	
	AF_C3DRigidbody* rigidbodyA = &_ecs->rigidbodies[_entityAID];
	AF_C3DRigidbody* rigidbodyB = &_ecs->rigidbodies[_entityBID];

	AF_CTransform3D* transformA = &_ecs->transforms[_entityAID];
	AF_CTransform3D* transformB = &_ecs->transforms[_entityBID];

	AF_CCollider* colliderA = &_ecs->colliders[_entityAID];
	AF_CCollider* colliderB = &_ecs->colliders[_entityBID];

	float totalMass;
	if (rigidbodyA->inverseMass == 0.0f && rigidbodyB->inverseMass == 0.0f) {
		totalMass = 0.0f;
		return;
	} else if (rigidbodyA->inverseMass == 0.0f) {
		totalMass = rigidbodyB->inverseMass;
	} else if (rigidbodyB->inverseMass == 0.0f) {
		totalMass = rigidbodyA->inverseMass;
	} else {
		totalMass = rigidbodyA->inverseMass + rigidbodyB->inverseMass;
	}

	// Seperate using projection
	// A value between 0.2 and 0.8 is standard. 1.0 can cause jitter.
	//float penetrationScale = 0.075f; // Adjust as needed
	//const float penetrationAllowance = 0.01f;

	// Calculate the total inverse mass for splitting the correction
    float totalInverseMass = rigidbodyA->inverseMass + rigidbodyB->inverseMass;
    if (totalInverseMass <= 0.0f) { // Both objects are static/immovable
        return;
    }

	af_bool_t hasRigidbodyA = AF_Component_GetEnabled(rigidbodyA->enabled);
	af_bool_t hasRigidbodyB = AF_Component_GetEnabled(rigidbodyB->enabled);

	// Baumgarte Stabilization: Separate objects with percentage-based penetration correction
	// Only correct penetration beyond the slop threshold to prevent jitter
	float correctionMagnitude = fmaxf(colliderA->collision.penetration - PENETRATION_SLOP, 0.0f);
	float correctionAmount = correctionMagnitude * PENETRATION_PERCENTAGE;
	
	// Only move objects that have a rigidbody and are not static (inverseMass > 0)
	if (rigidbodyA->inverseMass > 0.0f && hasRigidbodyA == AF_TRUE) {
		Vec3 correction = Vec3_MULT_SCALAR(colliderA->collision.normal, 
			correctionAmount * (rigidbodyA->inverseMass / totalInverseMass));
    	transformA->pos = Vec3_MINUS(transformA->pos, correction);
	}
	if (rigidbodyB->inverseMass > 0.0f && hasRigidbodyB == AF_TRUE) {
		Vec3 correction = Vec3_MULT_SCALAR(colliderB->collision.normal, 
			correctionAmount * (rigidbodyB->inverseMass / totalInverseMass));
		transformB->pos = Vec3_MINUS(transformB->pos, correction);
	}

	// Calculate relative vectors from the collider centers (not transform centers) to the collision point
	Vec3 relativeA = Vec3_MINUS(_collision->collisionPoint, colliderA->boundingPos);
	Vec3 relativeB = Vec3_MINUS(_collision->collisionPoint, colliderB->boundingPos);

	Vec3 angVelocityA = Vec3_CROSS(rigidbodyA->anglularVelocity, relativeA);
	Vec3 angVelocityB = Vec3_CROSS(rigidbodyB->anglularVelocity, relativeB);

	Vec3 fullVelocityA = Vec3_ADD(rigidbodyA->velocity, angVelocityA);
	Vec3 fullVelocityB = Vec3_ADD(rigidbodyB->velocity, angVelocityB);

	Vec3 contactVelocity = Vec3_MINUS(fullVelocityB, fullVelocityA);

	// Build up the impulse force
	float impulseForce = Vec3_DOT(contactVelocity, _collision->normal);
	
	// Check if objects are moving together (impulseForce < 0)
	// If they're moving apart, don't resolve (prevents objects from 'sticking' together)
	if (impulseForce >= 0.0f) {
		return; // Objects are already separating
	}

	// Work out the effect of inertia (using world-space transformed inertia tensor)
	// Transform the local-space inverse inertia tensors to world-space
	Mat4 worldInertiaTensorA = AF_Physics_TransformInertiaTensorToWorldSpace(rigidbodyA->inertiaTensor, transformA->modelMat);
	Mat4 worldInertiaTensorB = AF_Physics_TransformInertiaTensorToWorldSpace(rigidbodyB->inertiaTensor, transformB->modelMat);
	
	Vec3 crossRelativeNormalA = Vec3_CROSS(relativeA, _collision->normal);
	Vec3 tensorCrossRelativeNormalA = AF_Mat4_MULT_Vec3(worldInertiaTensorA, crossRelativeNormalA);
	Vec3 inertiaA = Vec3_CROSS(tensorCrossRelativeNormalA, relativeA);

	Vec3 crossRelativeNormalB = Vec3_CROSS(relativeB, _collision->normal);
	Vec3 tensorCrossRelativeNormalB = AF_Mat4_MULT_Vec3(worldInertiaTensorB, crossRelativeNormalB);
	Vec3 inertiaB = Vec3_CROSS(tensorCrossRelativeNormalB, relativeB);

	float angularEffect = Vec3_DOT(Vec3_ADD(inertiaA, inertiaB), _collision->normal);

	// Coefficient of restitution - use velocity-dependent value
	// For fast collisions, dissipate some kinetic energy (0.2)
	// For slow collisions (resting contacts), use zero restitution to help settle
	float relativeVelocityMag = fabsf(impulseForce);
	float cRestitution = 0.2f;
	
	const float restingThreshold = 1.0f; // Below this velocity, treat as resting contact
	if (relativeVelocityMag < restingThreshold) {
		// Interpolate restitution from 0 to 0.2 based on velocity
		cRestitution = (relativeVelocityMag / restingThreshold) * 0.2f;
	}
	
	float j = 0;

	float totalMassAngularEffect = totalMass + angularEffect;
	if(totalMassAngularEffect != 0.0f){
		j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);
	}else{
		j = 0;
	}
	
	
	
	Vec3 fullImpulse = Vec3_MULT_SCALAR(_collision->normal, j);

	// apply linear and angualr impulses in opposite directions 
	Vec3 negativeFullImpulse = Vec3_MULT_SCALAR(fullImpulse, -1);

	// Calculate angular impulses
	Vec3 angularImpulseA = Vec3_CROSS(relativeA, negativeFullImpulse);
	Vec3 angularImpulseB = Vec3_CROSS(relativeB, fullImpulse);
	
	
	// Detect resting contacts and heavily reduce/remove angular impulses to prevent rotation
	// Resting = low velocity collision + collision normal pointing mostly upward
	const float restingAngularThreshold = 1.5f; // Higher threshold - catch more resting cases
	const float restingNormalThreshold = 0.7f;  // Lower threshold - more permissive (Y > 0.7)
	
	af_bool_t isRestingContact = AF_FALSE;
	if (relativeVelocityMag < restingAngularThreshold && fabsf(_collision->normal.y) > restingNormalThreshold) {
		isRestingContact = AF_TRUE;
		// This is a resting contact - remove angular impulses completely
		Vec3 vecZero = {0, 0, 0};
		angularImpulseA = vecZero;
		angularImpulseB = vecZero;
	}

	// Apply impulses
	if (rigidbodyA->inverseMass > 0.0f && hasRigidbodyA == AF_TRUE) {
		AF_Physics_ApplyLinearImpulse(rigidbodyA, negativeFullImpulse);
		AF_Physics_ApplyAngularImpulse(rigidbodyA, angularImpulseA, worldInertiaTensorA);
		
		// For resting contacts, directly damp angular velocity to stop rotation
		if (isRestingContact) {
			rigidbodyA->anglularVelocity = Vec3_MULT_SCALAR(rigidbodyA->anglularVelocity, 0.9f);
		}
	}

	if (rigidbodyB->inverseMass > 0.0f && hasRigidbodyB == AF_TRUE) {
		AF_Physics_ApplyLinearImpulse(rigidbodyB, fullImpulse);
		AF_Physics_ApplyAngularImpulse(rigidbodyB, angularImpulseB, worldInertiaTensorB);
		
		// For resting contacts, directly damp angular velocity to stop rotation
		if (isRestingContact) {
			rigidbodyB->anglularVelocity = Vec3_MULT_SCALAR(rigidbodyB->anglularVelocity, 0.9f);
		}
	}
	

	// Friction
	const float mu = 0.1f; // Friction coefficient
	// Recalculate contact velocity after normal impulse
	angVelocityA = Vec3_CROSS(rigidbodyA->anglularVelocity, relativeA);
	angVelocityB = Vec3_CROSS(rigidbodyB->anglularVelocity, relativeB);
	Vec3 fullVelocityA_after = Vec3_ADD(rigidbodyA->velocity, angVelocityA);
	Vec3 fullVelocityB_after = Vec3_ADD(rigidbodyB->velocity, angVelocityB);
	contactVelocity = Vec3_MINUS(fullVelocityB_after, fullVelocityA_after);

	// Tangent vector
	float contactVelDotNormal = Vec3_DOT(contactVelocity, _collision->normal);
	Vec3 tangent = Vec3_MINUS(contactVelocity, Vec3_MULT_SCALAR(_collision->normal, contactVelDotNormal));
	float tangentMag = Vec3_MAGNITUDE(tangent);
	if (tangentMag > 0.0001f) {
		tangent = Vec3_MULT_SCALAR(tangent, 1.0f / tangentMag); // Normalize

		// Friction impulse
		float jt = -Vec3_DOT(contactVelocity, tangent);

		// Denominator for friction
		Vec3 crossRelativeTangentA = Vec3_CROSS(relativeA, tangent);
		Vec3 tensorCrossTangentA = AF_Mat4_MULT_Vec3(worldInertiaTensorA, crossRelativeTangentA);
		Vec3 inertiaTangentA = Vec3_CROSS(tensorCrossTangentA, relativeA);

		Vec3 crossRelativeTangentB = Vec3_CROSS(relativeB, Vec3_MULT_SCALAR(tangent, -1));
		Vec3 tensorCrossTangentB = AF_Mat4_MULT_Vec3(worldInertiaTensorB, crossRelativeTangentB);
		Vec3 inertiaTangentB = Vec3_CROSS(tensorCrossTangentB, relativeB);

		float denom = totalMass + Vec3_DOT(Vec3_ADD(inertiaTangentA, inertiaTangentB), tangent);
		if (denom > 0.0001f) {
			jt /= denom;
			// Clamp to Coulomb's law
			float maxJt = mu * fabsf(j);
			jt = fmaxf(-maxJt, fminf(jt, maxJt));

			Vec3 frictionImpulse = Vec3_MULT_SCALAR(tangent, jt);

			// Apply friction impulses
			if (rigidbodyA->inverseMass > 0.0f && hasRigidbodyA == AF_TRUE) {
				AF_Physics_ApplyLinearImpulse(rigidbodyA, Vec3_MULT_SCALAR(frictionImpulse, -1));
				Vec3 angularFrictionA = Vec3_CROSS(relativeA, Vec3_MULT_SCALAR(frictionImpulse, -1));
				AF_Physics_ApplyAngularImpulse(rigidbodyA, angularFrictionA, worldInertiaTensorA);
			}
			if (rigidbodyB->inverseMass > 0.0f && hasRigidbodyB == AF_TRUE) {
				AF_Physics_ApplyLinearImpulse(rigidbodyB, frictionImpulse);
				Vec3 angularFrictionB = Vec3_CROSS(relativeB, frictionImpulse);
				AF_Physics_ApplyAngularImpulse(rigidbodyB, angularFrictionB, worldInertiaTensorB);
			}
		}
	}

	// Apply stabilizing torque to help objects settle flat when resting
	// This helps objects rotate toward their most stable (flat) orientation
	float angVelMagA = Vec3_MAGNITUDE(rigidbodyA->anglularVelocity);
	float angVelMagB = Vec3_MAGNITUDE(rigidbodyB->anglularVelocity);
	
	// For object A: Apply stabilizing torque if it has gravity and low angular velocity
	if (rigidbodyA->inverseMass > 0.0f && hasRigidbodyA == AF_TRUE && 
	    rigidbodyA->gravity == AF_TRUE && angVelMagA < 3.0f) {
		
		// Calculate the "up" vector from current orientation
		// Correctly calculate the world-space "up" vector from the object's orientation
		Vec4 quat = transformA->orientation; // or transformB->orientation
		Vec3 localUp = {
			2.0f * (quat.x * quat.y - quat.w * quat.z),
			1.0f - 2.0f * (quat.x * quat.x + quat.z * quat.z),
			2.0f * (quat.y * quat.z + quat.w * quat.x)
		};
		Vec3 worldUp = {0, 1, 0};
		
		// Calculate torque to align local up with world up
		Vec3 torqueAxis = Vec3_CROSS(localUp, worldUp);
		float torqueMagnitude = Vec3_MAGNITUDE(torqueAxis);
		
		// Only apply if there's a meaningful misalignment
		if (torqueMagnitude > 0.01f) {
			// Scale torque based on velocity (stronger when slower)
			float torqueScale = 1.5f * (1.0f - (angVelMagA / 3.0f));
			Vec3 stabilizingTorque = Vec3_MULT_SCALAR(torqueAxis, torqueScale);
			rigidbodyA->torque = Vec3_ADD(rigidbodyA->torque, stabilizingTorque);
		}
	}
	
	// For object B: Apply stabilizing torque if it has gravity and low angular velocity
	if (rigidbodyB->inverseMass > 0.0f && hasRigidbodyB == AF_TRUE && 
	    rigidbodyB->gravity == AF_TRUE && angVelMagB < 3.0f) {
		
		// For object B:
		Vec4 quat = transformB->orientation;
		Vec3 localUp = {
			2.0f * (quat.x * quat.y - quat.w * quat.z),
			1.0f - 2.0f * (quat.x * quat.x + quat.z * quat.z),
			2.0f * (quat.y * quat.z + quat.w * quat.x)
		};
		Vec3 worldUp = {0, 1, 0};
		
		Vec3 torqueAxis = Vec3_CROSS(localUp, worldUp);
		float torqueMagnitude = Vec3_MAGNITUDE(torqueAxis);
		
		if (torqueMagnitude > 0.01f) {
			float torqueScale = 1.5f * (1.0f - (angVelMagB / 3.0f));
			Vec3 stabilizingTorque = Vec3_MULT_SCALAR(torqueAxis, torqueScale);
			rigidbodyB->torque = Vec3_ADD(rigidbodyB->torque, stabilizingTorque);
		}
	}
}

// Function to compare two CollisionInfo objects based on their hashes
af_bool_t AF_Physics_CollisionInfoLessThan(const AF_Collision* info1, const AF_Collision* info2) {
    // Calculate hash for the first CollisionInfo
    size_t hash1 = (size_t)info1->entity1ID + ((size_t)info1->entity2ID << 8);
    // Calculate hash for the second CollisionInfo
    size_t hash2 = (size_t)info2->entity1ID + ((size_t)info2->entity2ID << 8);

    // Return true if the hash of info1 is less than that of info2
    return (hash1 < hash2);
}

/**/
void AF_Physics_UpdateBroadphaseAABB(AF_CCollider* _collider){
	if(_collider->type == AABB){
		Vec3 boundingVolumeHalfDimensions = {_collider->boundingVolume.x*0.5f, _collider->boundingVolume.y*0.5f, _collider->boundingVolume.z*0.5f};
		_collider->broadphaseAABB = boundingVolumeHalfDimensions;
	}
}

// Function to handle narrow phase collision detection
void AF_Physics_NarrowPhase(AF_Collision* broadPhaseCollisions, size_t collisionCount, int numCollisionFrames) {
    if(broadPhaseCollisions){}
	if(collisionCount){}
	if(numCollisionFrames){}
	//AF_Collision allCollisions[1024]; // Example array to store all collisions
    //size_t allCollisionsCount = 0;       // Counter for all collisions

    // Iterate through the broad phase collisions
    //for (size_t i = 0; i < collisionCount; ++i) {
        //AF_Collision info = broadPhaseCollisions[i];

        // Check if the objects intersect
		// TODO: implement this
		/*
        if (ObjectIntersection(info.entity2, info.entity1, &info)) {
			//TODO: implement framesLeft
            //info.framesLeft = numCollisionFrames; // Set frames left
            ImpulseResolveCollision(info.entity1, info.entity2, info.collisionPoint); // Resolve collision
            allCollisions[allCollisionsCount++] = info; // Store collision info
        }*/
    //}

    // Optionally, you can print or process allCollisions here
    //printf("Total collisions: %zu\n", allCollisionsCount);
}

/*
====================
AF_PHYSICS_Raycast
Calculate ray intersection hit test against an Axis Aligned Bounding Box on all colliders in the ecs that are enabled
Returns AF_TRUE if a collision occured, and fills out the collision structure with the closest hit
====================
*/
af_bool_t AF_Physics_Raycast(const Ray* _ray, AF_ECS* _ecs, void* _physicsEngineHandle, AF_Collision* _collision) {
    (void)_physicsEngineHandle;
	af_bool_t foundCollision = AF_FALSE;
    AF_FLOAT closestDistance = AF_FLOAT_MAX; // Use a very large number
    AF_Collision tempCollision = AF_Collision_ZERO(); // Temporary storage for a potential hit

    // Set initial state of the collision struct to no hit
    _collision->collided = AF_FALSE;
    _collision->entity1ID = -1;

    // only search up to the current entity count
    for (uint32_t i = 0; i < _ecs->currentEntity; ++i) {
        AF_CCollider* collider = &_ecs->colliders[i];
        af_bool_t hasCollider = AF_Component_GetHas(collider->enabled);
        
        if (hasCollider == AF_FALSE) {
            continue;
        }
        
        AF_CTransform3D* transform = &_ecs->transforms[i];
        
        // This is a temporary struct to hold collision data for this specific entity
        AF_Collision currentCollision;
        currentCollision.collided = AF_FALSE;

        switch (collider->type) {
            case AABB:
                currentCollision.collided = AF_Physics_AABB_RayIntersection(_ray, collider, &currentCollision);
                break;
            case OBB_Type:
                currentCollision.collided = AF_Physics_OBB_RayIntersection(_ray, transform, &collider->boundingVolume, &currentCollision);
                break;
            case Plane:
                currentCollision.collided = AF_Physics_Plane_RayIntersection(_ray, collider, &currentCollision);
                break;
            case Sphere:
                currentCollision.collided = AF_Physics_Sphere_RayIntersection(_ray, transform, collider, &currentCollision);
                break;
            case Mesh:
                // TODO: Implement Mesh intersection
                break;
			case Compound:
				// TODO: Implement Compound intersection
			break;
			case Invalid:
				AF_Log("AF_Physics_Raycast: Invalid collider type for entity ID: %i\n", i);
				break;
        }

        // Check if we hit something and if it's closer than the previous closest hit
        if (currentCollision.collided == AF_TRUE) {
            // Note: Your ray-box intersection function now correctly sets rayDistance
            if (currentCollision.rayDistance < closestDistance) {
                closestDistance = currentCollision.rayDistance;
                // Store all the information from the current hit
                _collision->collided = AF_TRUE;
                _collision->entity1ID = i;
                _collision->entity2ID = 9999;
                _collision->rayDistance = currentCollision.rayDistance;
                _collision->collisionPoint = currentCollision.collisionPoint;
                
                // You can fill out the rest of the collision struct here as needed
                //_collision->normal = currentCollision.normal;
                //_collision->penetration = currentCollision.penetration;
                //_collision->callback = currentCollision.callback;
            }
        }
    }
    
    // After the loop, return whether a collision was found at all.
    // The _collision struct now holds the details of the closest hit.
    if (_collision->collided == AF_TRUE) {
        AF_Log("AF_Physics_Raycast: Found a collision with closest entity ID: %i at distance: %f\n", _collision->entity1ID, _collision->rayDistance);
        AF_Log("ray position x: %f y: %f z: %f \n", _ray->position.x, _ray->position.y, _ray->position.z);
        AF_Log("ray direction x: %f y: %f z: %f \n", _ray->direction.x, _ray->direction.y, _ray->direction.z);
        AF_Log("collision point x: %f y: %f z: %f \n", _collision->collisionPoint.x, _collision->collisionPoint.y, _collision->collisionPoint.z);
    } else {
        AF_Log("AF_Physics_Raycast: No collision found.\n");
    }
    
    return _collision->collided;
}


void AF_Physics_DrawBox(AF_CCollider* collider, float* color){
	(void)color;
	// render debug collider
                //draw all edges
                //if(collider->type == Plane){
	Vec3 pos = collider->boundingPos;//_ecs[i].transforms->pos;

	Vec3 bounds = collider->boundingVolume;
	// Top
	/*
	Vec3 top_bottomLeft = {pos.x - bounds.x/2, pos.x + bounds.x/2, pos.z - bounds.z/2};
	Vec3 top_topLeft =  {pos.x - bounds.x/2, pos.x + bounds.x/2, pos.z + bounds.z/2};
	Vec3 top_topRight =  {pos.x + bounds.x/2, pos.x + bounds.x/2, pos.z + bounds.z/2};
	Vec3 top_bottomRight =  {pos.x + bounds.x/2, pos.x + bounds.x/2, pos.z - bounds.z/2};
	*/
	
	  // Total lines: 12 edges * 2 vertices per edge
	Vec3 vertices[BOX_VERTEX_COUNT];

	// TODO: recieved many warnings, disabled for now
	AF_Log_Warning("AF_Physics_DrawBox: disabled filling in the vertices\n");
	
}

/*
====================
AF_Physics_TransformToAxis
Do shutdown things
====================
*/
AF_FLOAT AF_Physics_TransformToAxis(const AF_CTransform3D* _transform, const Vec3* _halfSize, Vec3 _axis){
	(void)_transform;
	Vec3 right = {1.0, 0.0, 0.0};
	Vec3 up = {0.0, 1.0, 0.0};
	Vec3 forward = {0.0, 0.0, 1.0};

	AF_FLOAT projection =
		_halfSize->x * fabsf(Vec3_DOT(_axis, right)) +
		_halfSize->y * fabsf(Vec3_DOT(_axis, up)) +
		_halfSize->z * fabsf(Vec3_DOT(_axis, forward));

	return projection;
}

/*
====================
AF_Physics_BuildContactManifold
Build contact manifold for collision between two OBBs
====================
*/
void AF_Physics_DetectBoxAndPoint(AF_CCollider* _boxCollider, const Vec3* _point, AF_Collision* outCollision) {
	(void)_boxCollider;
	(void)_point;
	(void)outCollision;
	/*
	From Ian Millingtons book "Game Physics Engine Development" p153, 13.3.5 COLLIDING TWO BOXES
	const Box &box,
	const Vector3 &point,
	CollisionData *data
	1. Consider each vertex of object A.
	2. Calculate the interpenetration of that vertex with object B.
	3. The deepest such interpenetration is retained.
	4. Do the same with object B’s vertices against object A.
	5. The deepest interpenetration overall is retained.
	The point–face detection code therefore looks like thi
	*/

	// Transform the point into box coordinates.

	// Check each axis, looking for the axis on which the

	// penetration is least deep.

	// Compile the contact.

	// Write the appropriate data.

	// Note that we don’t know what rigid body the point
	// belongs to, so we just use NULL. Where this is called
	// this value can be left, or filled in.
}

/*
====================
AF_Physics_Shutdown
Do shutdown things
====================
*/
void AF_Physics_Shutdown(void* _physicsEngineHandle){
	AF_Log("Physics: Shutdown\n");
	(void)_physicsEngineHandle;
	// don't need to free anything yet as we have no dynamic allocations
	
}

#ifdef __cplusplus
}
#endif

