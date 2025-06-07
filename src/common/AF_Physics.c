/*
===============================================================================
AF_PHYSICS Implementation 
Implementation of AF_Physics
===============================================================================
*/
#include "AF_Physics.h"
#include "AF_Log.h"


float collisionColor[4] = {255,0, 0, 1};

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
	AF_CTransform3D* parentTransform =_ecs->entities[i].parentTransform;
	if(parentTransform == NULL){
		return;
	}
	//assert(parentTransform != NULL && "Physics: AF_Physics_Update parent transform is null\n");
	// make sure the position matches the parent if we have one
	if(_ecs->entities[i].parentTransform != NULL){
		transform->pos = Vec3_ADD(parentTransform->pos, transform->localPos);
		transform->scale = Vec3_MULT(parentTransform->scale, transform->localScale);
		transform->rot = Vec3_ADD(parentTransform->rot, transform->localRot);
	}
    
	AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i];

	

	if((AF_Component_GetHas(rigidbody->enabled) == AF_TRUE) && (AF_Component_GetEnabled(rigidbody->enabled) == AF_TRUE)){
	
		
		//debgf("Physics: upate: velocity x: %f y: %f z: %f\n", rigidbody->velocity.x, rigidbody->velocity.y, rigidbody->velocity.z);
		// if the object isn't static
		if(rigidbody->inverseMass > 0 || rigidbody->isKinematic == AF_TRUE){
				AF_Physics_IntegrateAccell(rigidbody, _dt);
				AF_Physics_IntegrateVelocity(transform, rigidbody, _dt);  
		    }

		}

		AF_CCollider* collider = &_ecs->colliders[i];
		// update the bounds position
		collider->pos = _ecs->transforms[i].pos;
		// clear all collsision except keep the callback
		AF_Collision clearedCollision = {AF_FALSE, NULL, NULL, collider->collision.callback, {0,0,0}, 0.0f, {0,0,0}, 0};
		collider->collision = clearedCollision;
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
	AF_Physics_AABB_Test(_ecs);

	// call the collision pairs

	// Resolve collision between two objects
}

/*
====================
AF_Physics_LateRenderUpdate
Implementation of late render update
====================
*/
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs){
	assert(_ecs != NULL && "Physics: AF_Physics_LateRenderUpdate pass in a null reference\n");
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_CCollider* collider = &_ecs->colliders[i];
		if(collider->showDebug != AF_TRUE){
			//AF_Log("Physics: LateRenderUpate: not showing debug %i\n", i);
			continue;
		}

		if(collider->collision.collided != AF_TRUE){
			//AF_Log("Physics: LateRenderUpate: not colided\n");
			continue;
		}
		//AF_Log("Physics: LateRenderUpate: draw debug\n");
		//AF_Physics_DrawBox(collider, collisionColor);	
	}
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

