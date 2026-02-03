#include "AF_Physics.h"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <vector>



// Bullet data management struct
struct AF_BulletInternalData {
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	// Map ECS entity index to Bullet rigid body handles
	btRigidBody* bodies[AF_ECS_TOTAL_ENTITIES];
};

extern "C" {

// ========================================================================
// AF_Physics_Init
// ========================================================================
void AF_Physics_Init(AF_ECS* _ecs, void** _physicsEngineHandle) {
	if (_ecs == nullptr || _physicsEngineHandle == nullptr) {
		return;
	}

	// Allocate the internal data storage
	AF_BulletInternalData* bulletData = new AF_BulletInternalData();
	*_physicsEngineHandle = static_cast<void*>(bulletData);

	// Initialize Bullet components
	bulletData->collisionConfiguration = new btDefaultCollisionConfiguration();
	bulletData->dispatcher = new btCollisionDispatcher(bulletData->collisionConfiguration);
	bulletData->broadphase = new btDbvtBroadphase();
	bulletData->solver = new btSequentialImpulseConstraintSolver();
	bulletData->dynamicsWorld = new btDiscreteDynamicsWorld(
		bulletData->dispatcher, 
		bulletData->broadphase, 
		bulletData->solver, 
		bulletData->collisionConfiguration
	);

	bulletData->dynamicsWorld->setGravity(btVector3(0, GRAVITY_SCALE, 0));
	

	// Initialize body array
	for (int i = 0; i < AF_ECS_TOTAL_ENTITIES; ++i) {
		AF_C3DRigidbody* rb = &_ecs->rigidbodies[i];
		AF_CCollider* col = &_ecs->colliders[i];
		AF_CTransform3D* trans = &_ecs->transforms[i];
		AF_CTerrain* terrain = &_ecs->terrains[i];
		af_bool_t colEnabled = AF_Component_GetHasEnabled(col->enabled);

		if(colEnabled == AF_FALSE){
			continue;	
		}


		// init bt trans to be used
		btTransform btTrans;
        Vec4 q = AF_Vec4_EulerToQuaternion(Vec3_MULT_SCALAR(trans->rot, (AF_PI / 180.0f)));
        btTrans.setIdentity();
        btTrans.setOrigin(btVector3(trans->pos.x, trans->pos.y, trans->pos.z));
        btTrans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));
        
        bulletData->bodies[i] = nullptr;
        btCollisionShape* shape = nullptr;


		if (col->type == Sphere) {
			shape = new btSphereShape(col->boundingVolume.x);
		} else if (col->type == Terrain) {
			if(terrain == NULL){
				continue;
			}

            if (terrain->heightMapData == nullptr) {
                AF_Log_Error("AF_Physics_Init: Terrain heightMapData is NULL for entity %u. Heightmap physics skipped.\n", i);
                continue;
            }

            int width = (int)terrain->heightMapWidth;
            int length = (int)terrain->heightMapWidth; // heightmaps are always square for now 


            // The visual shader samples the texture as 0.0-1.0 and multiplies by heightScale.
            // So if pixel is 255 (1.0), height is 1.0 * 32.0 = 32.0.
            // Bullet receives raw bytes (0-255). It calculates height = byteValue * scale.
            // If we blindly pass 32.0 as scale, Bullet calculates 255 * 32.0 = 8160.0.
            // WE MUST DIVIDE BY 255.0f to match the visual representation!
            btScalar physicsHeightScale = (btScalar)terrain->heightScale / 255.0f;
            
            btScalar minH = 0.0f;
            btScalar maxH = 255.0f * physicsHeightScale; // Should equal terrain->heightScale
			shape = new btHeightfieldTerrainShape(
                width, length,
                terrain->heightMapData,
                physicsHeightScale,
                minH, maxH,
                1,
                PHY_UCHAR,
                false
            );
			
            btHeightfieldTerrainShape* heightfield = static_cast<btHeightfieldTerrainShape*>(shape);
           
            // FIXED: Use gridScale to determine vertex spacing if available.
            // Using boundingVolume forces the terrain into a specific box size, which squashes it if the volume is default (1.0).
            float scaleX, scaleZ;
            
            // If the terrain is composed of patches (chunks), gridScale is usually the size of ONE chunk.
            // The total world size = gridScale * numChunks.
            // The Bullet Heightfield needs to stretch the 256x256 image data across this TOTAL world size.
            
            float totalWorldSize = terrain->gridScale; // Start with patch size
            if (terrain->numChunks > 1) {
                totalWorldSize = terrain->gridScale * (float)terrain->numChunks;
            }

            if (totalWorldSize > 0.0001f) {
                // Determine the spacing between vertices (Scale)
                // Size = (Vertices - 1) * Scale
                // Scale = Size / (Vertices - 1)
                
                scaleX = totalWorldSize / (float)(width > 1 ? width - 1 : 1);
                scaleZ = totalWorldSize / (float)(length > 1 ? length - 1 : 1);
            } else {
                 AF_Log("AF_Physics_Init: Warning - Terrain gridScale is 0, falling back to Collider BoundingVolume for scaling.\n");
                 scaleX = (col->boundingVolume.x * 2.0f) / (float)(width > 1 ? width - 1 : 1);
                 scaleZ = (col->boundingVolume.z * 2.0f) / (float)(length > 1 ? length - 1 : 1);
            }

            heightfield->setLocalScaling(btVector3(scaleX, 1.0f, scaleZ));


            // Apply the vertical offset to the ALREADY declared btTrans
            btScalar middleHeight = (minH + maxH) * 0.5f;
            
            // Calculate the half-extents based on the ACTUAL scale used
            float terrainHalfWidth = ((width - 1) * scaleX) * 0.5f;
            float terrainHalfLength = ((length - 1) * scaleZ) * 0.5f;

            // Calculate where we are placing the body
            // Visual Terrain is Centered at trans->pos (Render uses (i - numChunks/2) offset).
            // Bullet Heightfield is also centered at its local origin.
            // So we want the Bullet Origin to be exactly at trans->pos.
            // (Only height needs offset because Bullet centers Y around 0)
            
            // Re-calc middle height with corrected scale
            

            btVector3 origin = btVector3(
                trans->pos.x, // + terrainHalfWidth, 
                trans->pos.y + (btScalar)((minH + maxH) * 0.5f), 
                trans->pos.z // + terrainHalfLength 
            );
            btTrans.setOrigin(origin);
            
            rb->inverseMass = 0.0f; // Static

		} else {
			// Bullet box extents are half-extents
			shape = new btBoxShape(btVector3(col->boundingVolume.x, col->boundingVolume.y, col->boundingVolume.z));
		}

	
		btScalar mass = 0.0f;
		if (col->type != Terrain && rb->inverseMass > 0.0f) {
			mass = 1.0f / rb->inverseMass;
		}

		if (rb->isKinematic) {
			mass = 0.0f;
		}

		
		btVector3 localInertia(0, 0, 0);
		if (mass > 0) {
			shape->calculateLocalInertia(mass, localInertia);
		}

		btDefaultMotionState* motionState = new btDefaultMotionState(btTrans);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
		// Increase friction and restitution for testing
		rbInfo.m_friction = 0.8f; 
		rbInfo.m_restitution = 0.2f;
		rbInfo.m_linearDamping = 0.05f;
		rbInfo.m_angularDamping = 0.05f;

		btRigidBody* body = new btRigidBody(rbInfo);
		
		// Simple user pointer to store ECS ID for reconstruction during raycasts
		body->setUserIndex(i);

		if (rb->isKinematic) {
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);
		} else if (bulletData->bodies[i] == nullptr && rb->inverseMass > 0.0f) {
            // Dynamic Body Settings
            // Enable CCD to prevent tunneling through thin terrain
            body->setCcdMotionThreshold(1e-7); 
            body->setCcdSweptSphereRadius(0.2f); // Adjust based on object size
        }

		bulletData->dynamicsWorld->addRigidBody(body);
		bulletData->bodies[i] = body;

		
		// Set gravity
		if (rb->gravity == AF_FALSE) {
			body->setGravity(btVector3(0, 0, 0));
		}
	}
}

// ========================================================================
// AF_Physics_Update
// ========================================================================
void AF_Physics_Update(AF_ECS* _ecs, void* _physicsEngineHandle, const float _dt) {
	if (_ecs == nullptr || _physicsEngineHandle == nullptr) {
		return;
	}

	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);

	// 1. Sync ECS state to Bullet (creation/deletion/motion update)
	for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {
		AF_C3DRigidbody* rb = &_ecs->rigidbodies[i];
		AF_CCollider* col = &_ecs->colliders[i];
		AF_CTransform3D* trans = &_ecs->transforms[i];

		af_bool_t rbEnabled = AF_Component_GetHasEnabled(rb->enabled);

		if (rbEnabled == AF_FALSE){
			continue;	
		}
		
		if (bulletData->bodies[i] == nullptr) {
			continue;
		}
		btRigidBody* body = bulletData->bodies[i];
		
		// Check Kinematic state change
		bool wasKinematic = body->isKinematicObject();
		bool isKinematic = rb->isKinematic;

        // FIXED: Do not apply ECS velocity as an IMPULSE. This causes accumulation and tunneling.
        // Instead, if the ECS velocity is set (non-zero), we assume it's an override or input.
        // Ideally, we should sync Bullet -> ECS, not the other way around for dynamic bodies, 
        // unless there is specific input.
        // For now, let's trust Bullet's gravity and only apply ECS velocity if it's "significant" 
        // implies an external set. BUT, this fights collision.
        
        // BETTER APPROACH: 
        // 1. Only apply forces if rb->force is used (requires ECS change).
        // 2. Since we only have 'velocity' in this context, let's assumes rb->velocity is ONLY for initialization
        //    or overrides. 
        //    However, without syncing BACK, rb->velocity remains set.
        
        // Temporary fix: commenting out the "Apply Velocity as Impulse" to see if Physics works naturally with Gravity.
        /*
        if (Vec3_MAGNITUDE(rb->velocity) > 0.0001f || Vec3_MAGNITUDE(rb->torque) > 0.0001f) {
            body->activate(true);
            body->applyImpulse(
                btVector3(rb->velocity.x * _dt, rb->velocity.y * _dt, rb->velocity.z * _dt),
                btVector3(rb->torque.x * _dt, rb->torque.y * _dt, rb->torque.z * _dt)
            );
        }
        */
		
		if (wasKinematic != isKinematic) {
			if (isKinematic) {
				// Changed to Kinematic
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				body->setActivationState(DISABLE_DEACTIVATION);
				body->setMassProps(0.0f, btVector3(0, 0, 0));
				body->updateInertiaTensor();
			} else {
				// Changed to Dynamic
				body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
				body->setActivationState(DISABLE_DEACTIVATION); // CHANGED: FORCE ACTIVE to ensure it simulates
				//body->setActivationState(ISLAND_SLEEPING);
				body->activate(true);

				// Recalculate mass
				btScalar mass = 0.0f;
				if (rb->inverseMass > 0.0f) {
					mass = 1.0f / rb->inverseMass;
				}
				btVector3 localInertia(0, 0, 0);
				if (mass > 0) {
					body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
				}
				body->setMassProps(mass, localInertia);
				body->updateInertiaTensor();
			}
		} else if (!isKinematic) {
			// Regular mass update
			btScalar mass = 0.0f;
			if (rb->inverseMass > 0.0f) {
				mass = 1.0f / rb->inverseMass;
			}
			
			// Update mass if it changed
			btScalar currentMass = 0.0f;
			if (body->getInvMass() > 0.0f) {
				currentMass = 1.0f / body->getInvMass();
			}

			if (mass != currentMass) {
				btVector3 localInertia(0, 0, 0);
				if (mass > 0) {
					body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
				}
				body->setMassProps(mass, localInertia);
				body->updateInertiaTensor();
			}
            
            // Check if ECS velocity differs from Bullet velocity (User changed it via script like KeyMove.c)
             btVector3 currentBulletVel = body->getLinearVelocity();
             Vec3 ecsVel = rb->velocity;
             
             // Check difference with epsilon
             float dX = ecsVel.x - currentBulletVel.x();
             float dY = ecsVel.y - currentBulletVel.y();
             float dZ = ecsVel.z - currentBulletVel.z();
             
             if (dX*dX + dY*dY + dZ*dZ > 0.0001f) {
                 // User changed velocity in ECS. Override Bullet velocity.
                 // Special handling for scripts that zero out Y velocity (cancelling gravity):
                 // If ECS Y is 0 but Bullet Y is significant (falling), keep falling speed.
                 // This assumes scripts setting Y=0 likely meant "No Y Input", not "Levitate".
                 
                 btVector3 newVel(ecsVel.x, ecsVel.y, ecsVel.z);
                 if (ecsVel.y == 0.0f && fabsf(currentBulletVel.y()) > 0.1f) {
                     newVel.setY(currentBulletVel.y());
                 }
                 
                 body->setLinearVelocity(newVel);
                 body->activate(true);
             }
		}

		// If isKinematic is true, sync the transform FROM ECS TO Bullet
		if (isKinematic) {
			btTransform btTrans;
			btTrans.setIdentity();
			btTrans.setOrigin(btVector3(trans->pos.x, trans->pos.y, trans->pos.z));
			Vec4 q = AF_Vec4_EulerToQuaternion(Vec3_MULT_SCALAR(trans->rot, AF_PI / 180.0f));
			btTrans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));

			if (body->getMotionState()) {
				body->getMotionState()->setWorldTransform(btTrans);
			}
			body->setWorldTransform(btTrans);
		}

		// Sync Gravity
		btVector3 currentGravity = body->getGravity();
		btVector3 targetGravity(0, 0, 0);
		if (rb->gravity != AF_FALSE) {
			targetGravity.setValue(0, GRAVITY_SCALE, 0);
		}

		if (currentGravity != targetGravity) {
			body->setGravity(targetGravity);
			body->activate(true);
		}
		
	}

	// 2. Step simulation
	bulletData->dynamicsWorld->stepSimulation(_dt, 10, 1.0f / 60.0f); // Fixed time step

	// 3. Sync Bullet state back to ECS

	// 3. Sync Bullet state back to ECS
	for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {
		if (bulletData->bodies[i]) {
			AF_C3DRigidbody* rb = &_ecs->rigidbodies[i];
			AF_CCollider* col = &_ecs->colliders[i];
			if (rb->isKinematic ||  bulletData->bodies[i]->isStaticObject() || col->type == Terrain) {
				continue;
			} 
			
			btTransform btTrans;
			bulletData->bodies[i]->getMotionState()->getWorldTransform(btTrans);

			_ecs->transforms[i].pos.x = btTrans.getOrigin().getX();
			_ecs->transforms[i].pos.y = btTrans.getOrigin().getY();
			_ecs->transforms[i].pos.z = btTrans.getOrigin().getZ();

			btQuaternion q = btTrans.getRotation();
			_ecs->transforms[i].orientation = { (float)q.x(), (float)q.y(), (float)q.z(), (float)q.w() };
			
            // SYNC BACK VELOCITY
            const btVector3& vel = bulletData->bodies[i]->getLinearVelocity();
             _ecs->rigidbodies[i].velocity = { (float)vel.x(), (float)vel.y(), (float)vel.z() };

             const btVector3& angVel = bulletData->bodies[i]->getAngularVelocity();
             _ecs->rigidbodies[i].anglularVelocity = { (float)angVel.x(), (float)angVel.y(), (float)angVel.z() };

			// Update collider bounds
			
			// Reset collisions
			AF_Collision_Reset(&_ecs->colliders[i].collision);
		}
	}
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

// ========================================================================
// AF_Physics_Raycast
// ========================================================================
af_bool_t AF_Physics_Raycast(const Ray* _ray, AF_ECS* _ecs, void* _physicsEngineHandle, AF_Collision* _collision) {
	if (_ray == nullptr || _ecs == nullptr || _physicsEngineHandle == nullptr || _collision == nullptr) {
		return AF_FALSE;
	}

	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);
	
	btVector3 rayFrom(_ray->position.x, _ray->position.y, _ray->position.z);
	Vec3 endPos = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, 1000.0f)); // Max dist 1000
	btVector3 rayTo(endPos.x, endPos.y, endPos.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
	bulletData->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

	if (rayCallback.hasHit()) {
		_collision->collided = AF_TRUE;
		_collision->rayDistance = (float)rayCallback.m_closestHitFraction * 1000.0f;
		_collision->collisionPoint = { 
			(float)rayCallback.m_hitPointWorld.getX(), 
			(float)rayCallback.m_hitPointWorld.getY(), 
			(float)rayCallback.m_hitPointWorld.getZ() 
		};
		_collision->normal = {
			(float)rayCallback.m_hitNormalWorld.getX(),
			(float)rayCallback.m_hitNormalWorld.getY(),
			(float)rayCallback.m_hitNormalWorld.getZ()
		};

		// Recover entity ID from user index
		const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body) {
			_collision->entity1ID = body->getUserIndex();
		}

		return AF_TRUE;
	}

	return AF_FALSE;
}

// ========================================================================
// AF_Physics_Shutdown
// ========================================================================
void AF_Physics_Shutdown(void* _physicsEngineHandle) {
	if (_physicsEngineHandle == nullptr) {
		return;
	}

	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);

	for (int i = 0; i < AF_ECS_TOTAL_ENTITIES; ++i) {
		if (bulletData->bodies[i]) {
			bulletData->dynamicsWorld->removeRigidBody(bulletData->bodies[i]);
			delete bulletData->bodies[i]->getMotionState();
			delete bulletData->bodies[i]->getCollisionShape();
			delete bulletData->bodies[i];
		}
	}

	delete bulletData->dynamicsWorld;
	delete bulletData->solver;
	delete bulletData->broadphase;
	delete bulletData->dispatcher;
	delete bulletData->collisionConfiguration;
	delete bulletData;
}

void AF_Physics_LateUpdate(AF_ECS* _ecs, void* _physicsEngineHandle) { 
	(void)_ecs; (void)_physicsEngineHandle; 

}
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs, void* _physicsEngineHandle) { 
	(void)_ecs; (void)_physicsEngineHandle; 
}


void AF_Physics_Reset(AF_ECS* _ecs, AF_ECS* _backupECS, void* _physicsEngineHandle){
	if(_ecs == NULL){
		AF_Log_Error("AF_Physics_Reset: ecs is null\n");
	}
	if(_physicsEngineHandle == NULL){
		AF_Log_Error("AF_Physics_Reset: _physics Engine handle is null\n");
	}

	
	AF_BulletInternalData* bulletInternalData = (AF_BulletInternalData*)_physicsEngineHandle;

	btVector3 zeroVector(0,0,0);

	for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {

		AF_C3DRigidbody* rb = &_ecs->rigidbodies[i];
		AF_CTransform3D* trans = &_ecs->transforms[i];
		bool isEnabled = AF_Component_GetHasEnabled(rb->enabled);
		btRigidBody* rigidbody = bulletInternalData->bodies[i];

		// Skip if component is disabled or the physics body doesn't exist
		if (!isEnabled || rigidbody == nullptr) {
			continue;
		}

		
		// 2. Reset velocities and forces in both Bullet and ECS
		rb->velocity = Vec3_ZERO();
		rb->anglularVelocity = Vec3_ZERO();
		rb->force = Vec3_ZERO();
		rb->torque = Vec3_ZERO();

		
		rigidbody->clearForces();
		rigidbody->setLinearVelocity(zeroVector);
		rigidbody->setAngularVelocity(zeroVector);

		// 3. Reset Transform
		btVector3 initialPosition(trans->pos.x, trans->pos.y, trans->pos.z);
		Vec4 q = AF_Vec4_EulerToQuaternion(Vec3_MULT_SCALAR(trans->rot, AF_PI / 180.0f));
		btQuaternion initialOrientation(q.x, q.y, q.z, q.w);

		
		btTransform initialTransform;
		initialTransform.setIdentity();
		initialTransform.setOrigin(initialPosition);
		initialTransform.setRotation(initialOrientation);

		// Reset Bullet transform and motion state
		rigidbody->setWorldTransform(initialTransform);
		if (rigidbody->getMotionState()) {
			rigidbody->getMotionState()->setWorldTransform(initialTransform);
		}

		// 4. Sync properties like mass which might have been reset from backup
		btScalar mass = 0.0f;
		if (rb->inverseMass > 0.0f) {
			mass = 1.0f / rb->inverseMass;
		}

		btVector3 localInertia(0, 0, 0);
		if (mass > 0) {
			rigidbody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
		}
		rigidbody->setMassProps(mass, localInertia);
		rigidbody->updateInertiaTensor();

		// 5. Ensure the body is active so it starts falling/moving again
		rigidbody->activate(true);
	}
}

} // extern "C"
