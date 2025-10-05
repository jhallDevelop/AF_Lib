/*
===============================================================================
AF_PHYSICS Implementation 
Implementation of AF_Physics
===============================================================================
*/
#include "AF_Physics.h"
#include "AF_Log.h"

// Small constant to counteract floating point errors
#define AF_PHYSICS_EPSILON 0.00001f


float collisionColor[4] = {255,0, 0, 1};

typedef struct Mat3{
    // A 3x3 array to hold the matrix elements [row][column]
    float m[3][3];
} Mat3;

typedef struct OBB {
	AF_FLOAT c[3]; // OBB center point
	Vec3 u[3]; // Local x-, y-, and z-axes
	AF_FLOAT e[3]; // Positive halfwidth extents of OBB along each axis
} OBB;

/*
====================
AF_Physics_Init
Implementation of Init
====================
*/
void AF_Physics_Init(AF_ECS* _ecs){
	assert(_ecs != NULL && "Physics: Physics_Init pass in a null reference\n");
	AF_Log("Physics_Init: \n");


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
void AF_Physics_Update(AF_ECS* _ecs, const float _dt){
	assert(_ecs != NULL && "Physics: AF_Physics_Update pass in a null reference\n");
	// loop through and update all transforms based on their velocities
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
	AF_CTransform3D* transform = &_ecs->transforms[i];

	assert(transform != NULL && "Physics: AF_Physics_Update transform is null\n");
	
	AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i];

	

	if((AF_Component_GetHasEnabled(rigidbody->enabled) == AF_TRUE)) {
	
		//debgf("Physics: upate: velocity x: %f y: %f z: %f\n", rigidbody->velocity.x, rigidbody->velocity.y, rigidbody->velocity.z);
		// if the object isn't static
		if(rigidbody->inverseMass > 0 || rigidbody->isKinematic == AF_TRUE){
				AF_Physics_IntegrateAccell(rigidbody, _dt);
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
void AF_Physics_LateUpdate(AF_ECS* _ecs){
	assert(_ecs != NULL && "Physics: AF_Physics_LateUpdate pass in a null reference\n");

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
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs){
	assert(_ecs != NULL && "Physics: AF_Physics_LateRenderUpdate pass in a null reference\n");
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
		Vec4 corner = {corners[i].x, corners[i].y, corners[i].z};
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
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){

		if(AF_Component_GetHasEnabled(_ecs->colliders[i].enabled) == AF_FALSE){
			continue;
		}
		AF_Entity* entity1 = &_ecs->entities[i];
		uint32_t entity1ID = i;//AF_ECS_GetID(entity1->id_tag);
		AF_CCollider* collider1 = &_ecs->colliders[entity1ID];
		AF_CTransform3D* entity1Transform = &_ecs->transforms[entity1ID];
		
		
		// rayIntersectionTest everything
		for(uint32_t x = i + 1; x < _ecs->entitiesCount; ++x){
			if(AF_Component_GetHasEnabled(_ecs->colliders[x].enabled) == AF_FALSE){
				continue;
			}

			// check self
			if(i == x){
				continue;
			}

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

            af_bool_t collisionSuccess = AF_Physics_OBB_Test(_ecs, entity1ID, entity2ID, entity1Transform, collider1, entity2Transform, collider2, &collisionResult);
            if(collisionSuccess == AF_FALSE){
                continue;
            }

            // --- FIX: Add the collision handling logic here ---

            // 1. Copy the collision result into each collider's data structure.
            collider1->collision = collisionResult;
            collider2->collision = collisionResult;
            // Invert the normal for the second collider.
            collider2->collision.normal = Vec3_MULT_SCALAR(collisionResult.normal, -1.0f);

            // 2. Invoke the callbacks if they exist.
            if(collider1->collision.callback != NULL){
                collider1->collision.ecsPtr = _ecs;
                collider1->collision.callback(&collider1->collision);
            }
            if(collider2->collision.callback != NULL){
                collider2->collision.ecsPtr = _ecs;
                collider2->collision.callback(&collider2->collision);
            }

            // 3. Resolve the collision for non-kinematic objects.
            AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i];
            if(rigidbody->isKinematic == AF_FALSE){
                // Pass the correct collision data to the resolver.
                AF_Physics_ResolveCollision(_ecs, entity1ID, entity2ID, &collider1->collision);
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
			
			//TODO: where is __FLT_MAX__ defined? may not be portable
			float penetration = 2147483647.0;//__FLT_MAX__;
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
AF_Physics_Shutdown
Do shutdown things
====================
*/
void AF_Physics_Shutdown(void){
	AF_Log("Physics: Shutdown\n");
}

