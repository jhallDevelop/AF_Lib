#include "AF_Physics.h"
// Suppress warnings from Bullet headers
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#pragma clang diagnostic pop
#include <vector>

// =================================================================================================
// AF_Physics_BilinearUpsampleHeightmap
// Upsamples a heightmap from srcW x srcH to dstW x dstH using bilinear interpolation.
// Matches GPU texture sampling behavior for accurate physics alignment.
// =================================================================================================
static unsigned char* AF_Physics_BilinearUpsampleHeightmap(
	const unsigned char* src, uint32_t srcW, uint32_t srcH,
	uint32_t dstW, uint32_t dstH)
{
	unsigned char* dst = new unsigned char[dstW * dstH];

	for (uint32_t dz = 0; dz < dstH; ++dz) {
		for (uint32_t dx = 0; dx < dstW; ++dx) {
			float sx = (float)dx / (float)(dstW - 1) * (float)(srcW - 1);
			float sz = (float)dz / (float)(dstH - 1) * (float)(srcH - 1);

			uint32_t x0 = (uint32_t)sx;
			uint32_t z0 = (uint32_t)sz;
			uint32_t x1 = (x0 < srcW - 1) ? x0 + 1 : x0;
			uint32_t z1 = (z0 < srcH - 1) ? z0 + 1 : z0;

			float fx = sx - (float)x0;
			float fz = sz - (float)z0;

			float h00 = (float)src[z0 * srcW + x0];
			float h10 = (float)src[z0 * srcW + x1];
			float h01 = (float)src[z1 * srcW + x0];
			float h11 = (float)src[z1 * srcW + x1];

			float h = h00 * (1.0f - fx) * (1.0f - fz)
					 + h10 * fx * (1.0f - fz)
					 + h01 * (1.0f - fx) * fz
					 + h11 * fx * fz;

			dst[dz * dstW + dx] = (unsigned char)(h + 0.5f);
		}
	}

	return dst;
}

// =================================================================================================
// AF_BulletDebugDraw
// Bullet debug drawer that collects line segments into a CPU buffer for later GL rendering
// =================================================================================================
class AF_BulletDebugDraw : public btIDebugDraw {
public:
	std::vector<float> lineVertices; // x,y,z per vertex, 2 vertices per line
	int _debugMode;

	AF_BulletDebugDraw() : _debugMode(DBG_DrawWireframe) {}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
		(void)color;
		lineVertices.push_back((float)from.getX());
		lineVertices.push_back((float)from.getY());
		lineVertices.push_back((float)from.getZ());
		lineVertices.push_back((float)to.getX());
		lineVertices.push_back((float)to.getY());
		lineVertices.push_back((float)to.getZ());
	}

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {
		(void)PointOnB; (void)normalOnB; (void)distance; (void)lifeTime; (void)color;
	}

	void reportErrorWarning(const char* warningString) override {
		AF_Log_Warning("Bullet: %s\n", warningString);
	}

	void draw3dText(const btVector3& location, const char* textString) override {
		(void)location; (void)textString;
	}

	void setDebugMode(int debugMode) override {
		_debugMode = debugMode;
	}

	int getDebugMode() const override {
		return _debugMode;
	}

	void clearLines() override {
		lineVertices.clear();
	}
};

// Bullet data management struct
struct AF_BulletInternalData {
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	// Map ECS entity index to Bullet rigid body handles
	btRigidBody* bodies[AF_ECS_TOTAL_ENTITIES];

	// Debug drawer for visualizing physics shapes
	AF_BulletDebugDraw* debugDrawer;

	// Per-entity upsampled heightmap buffers (owned by physics, freed on shutdown)
	unsigned char* upsampledHeightMaps[AF_ECS_TOTAL_ENTITIES];
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

	// Setup debug drawer
	bulletData->debugDrawer = new AF_BulletDebugDraw();
	bulletData->debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	bulletData->dynamicsWorld->setDebugDrawer(bulletData->debugDrawer);

	// Initialize arrays
	for (int j = 0; j < AF_ECS_TOTAL_ENTITIES; ++j) {
		bulletData->upsampledHeightMaps[j] = nullptr;
		bulletData->bodies[j] = nullptr;	
	}

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
		// Apply posOffset so the physics body is placed at the collider center
		btTransform btTrans;
        Vec4 q = trans->rot; // quaternion (x, y, z, w)
        btTrans.setIdentity();
        btTrans.setOrigin(btVector3(
            trans->pos.x + col->posOffset.x,
            trans->pos.y + col->posOffset.y,
            trans->pos.z + col->posOffset.z
        ));
        btTrans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));
        
        btCollisionShape* shape = nullptr;


		if (col->type == AF_COLLISION_TYPE_Sphere) {
			shape = new btSphereShape(col->boundingVolume.x);
		} else if (col->type == AF_COLLISION_TYPE_Terrain) {
			if(terrain == NULL){
				continue;
			}

            if (terrain->heightMapData == nullptr) {
                AF_Log_Error("AF_Physics_Init: Terrain heightMapData is NULL for entity %u. Heightmap physics skipped.\n", i);
                continue;
            }

            int srcWidth = (int)terrain->heightMapWidth;
            int srcHeight = (int)terrain->heightMapWidth; // heightmaps are always square for now

            AF_Log("AF_Physics_Init: Entity %u terrain heightmap is %dx%d, physicsResolution=%u\n",
                   i, srcWidth, srcHeight, terrain->physicsResolution);

            // Determine physics grid resolution (upsample if needed)
            const unsigned char* physicsData = terrain->heightMapData;
            int width = srcWidth;
            int length = srcHeight;

            if (terrain->physicsResolution > 0
                && (int)terrain->physicsResolution != srcWidth) {
                int dstRes = (int)terrain->physicsResolution;
                unsigned char* upsampled = AF_Physics_BilinearUpsampleHeightmap(
                    terrain->heightMapData,
                    (uint32_t)srcWidth, (uint32_t)srcHeight,
                    (uint32_t)dstRes, (uint32_t)dstRes
                );
                bulletData->upsampledHeightMaps[i] = upsampled;
                physicsData = upsampled;
                width = dstRes;
                length = dstRes;
                AF_Log("AF_Physics_Init: Upsampled heightmap %dx%d -> %dx%d for entity %u\n",
                       srcWidth, srcHeight, dstRes, dstRes, i);
            }

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
                physicsData,
                physicsHeightScale,
                minH, maxH,
                1,
                PHY_UCHAR,
                false
            );
			
            btHeightfieldTerrainShape* heightfield = static_cast<btHeightfieldTerrainShape*>(shape);
           
            // FIXED: Use gridScale to determine vertex spacing if available.
            // Using boundingVolume forces the terrain into a specific box size, which squashes it if the volume is default (1.0).
            AF_FLOAT scaleX, scaleZ;
            
            // If the terrain is composed of patches (chunks), gridScale is usually the size of ONE chunk.
            // The total world size = gridScale * numChunks.
            // The Bullet Heightfield needs to stretch the 256x256 image data across this TOTAL world size.
            
            AF_FLOAT totalWorldSize = terrain->gridScale; // Start with patch size
            if (terrain->numChunks > 1) {
                totalWorldSize = terrain->gridScale * (AF_FLOAT)terrain->numChunks;
            }

            if (totalWorldSize > 0.0001f) {
                // Determine the spacing between vertices (Scale)
                // Size = (Vertices - 1) * Scale
                // Scale = Size / (Vertices - 1)
                
                scaleX = totalWorldSize / (AF_FLOAT)(width > 1 ? width - 1 : 1);
                scaleZ = totalWorldSize / (AF_FLOAT)(length > 1 ? length - 1 : 1);
            } else {
                 AF_Log("AF_Physics_Init: Warning - Terrain gridScale is 0, falling back to Collider BoundingVolume for scaling.\n");
                 scaleX = (col->boundingVolume.x * 2.0f) / (AF_FLOAT)(width > 1 ? width - 1 : 1);
                 scaleZ = (col->boundingVolume.z * 2.0f) / (AF_FLOAT)(length > 1 ? length - 1 : 1);
            }

            heightfield->setLocalScaling(btVector3(scaleX, 1.0f, scaleZ));


            // Apply the vertical offset to the ALREADY declared btTrans
            btScalar middleHeight = (minH + maxH) * 0.5f;
            
            // Calculate the half-extents based on the ACTUAL scale used
            AF_FLOAT terrainHalfWidth = ((width - 1) * scaleX) * 0.5f;
            AF_FLOAT terrainHalfLength = ((length - 1) * scaleZ) * 0.5f;

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
		if (col->type != AF_COLLISION_TYPE_Terrain && rb->inverseMass > 0.0f) {
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
void AF_Physics_Update(AF_ECS* _ecs, void* _physicsEngineHandle, const AF_FLOAT _dt) {
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

			// Add angular velocity
			btVector3 currentBulletAnglVel = body->getAngularVelocity();
			Vec3 ecsAngVel = rb->anglularVelocity;

			AF_FLOAT dAngX = ecsAngVel.x - currentBulletAnglVel.x();
			AF_FLOAT dAngY = ecsAngVel.y - currentBulletAnglVel.y();
			AF_FLOAT dAngZ = ecsAngVel.z - currentBulletAnglVel.z();

			if(dAngX*dAngX + dAngY*dAngY + dAngZ*dAngZ > 0.0001f || Vec3_MAGNITUDE(ecsAngVel) < 0.0001f) {
				btVector3 newAngVel(ecsAngVel.x, ecsAngVel.y, ecsAngVel.z);
				body->setAngularVelocity(newAngVel);
				body->activate(true);
			}

			// Handle Rotation Constraints (Freeze Rotation)
            // ECS: 1.0 = Freeze, 0.0 = Free
            // Bullet: 0.0 = Freeze, 1.0 = Free
            btVector3 targetAngularFactor(
                rb->freezAngularVelocity.x > 0.0001f ? 0.0f : 1.0f,
                rb->freezAngularVelocity.y > 0.0001f ? 0.0f : 1.0f,
                rb->freezAngularVelocity.z > 0.0001f ? 0.0f : 1.0f
            );

            // Only update if the factor has changed to avoid unnecessary overhead
            if (body->getAngularFactor() != targetAngularFactor) {
                body->setAngularFactor(targetAngularFactor);
                // Wake up body so the freeze/unfreeze takes effect immediately
                body->activate(true);
            }
            
            // Check if ECS velocity differs from Bullet velocity (User changed it via script like KeyMove.c)
             btVector3 currentBulletVel = body->getLinearVelocity();
             Vec3 ecsVel = rb->velocity;
             
             // Check difference with epsilon
             AF_FLOAT dX = ecsVel.x - currentBulletVel.x();
             AF_FLOAT dY = ecsVel.y - currentBulletVel.y();
             AF_FLOAT dZ = ecsVel.z - currentBulletVel.z();
             
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
			btTrans.setOrigin(btVector3(
				trans->pos.x + col->posOffset.x,
				trans->pos.y + col->posOffset.y,
				trans->pos.z + col->posOffset.z
			));
			
			Vec4 q = trans->rot; // quaternion (x, y, z, w)
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
			if (rb->isKinematic ||  bulletData->bodies[i]->isStaticObject() || col->type == AF_COLLISION_TYPE_Terrain) {
				continue;
			} 
			
			btTransform btTrans;
			bulletData->bodies[i]->getMotionState()->getWorldTransform(btTrans);

			// Subtract posOffset so we store the entity position, not the collider center
			_ecs->transforms[i].pos.x = btTrans.getOrigin().getX() - col->posOffset.x;
			_ecs->transforms[i].pos.y = btTrans.getOrigin().getY() - col->posOffset.y;
			_ecs->transforms[i].pos.z = btTrans.getOrigin().getZ() - col->posOffset.z;

			btQuaternion q = btTrans.getRotation();
			_ecs->transforms[i].rot = { 
				(AF_FLOAT)q.x(), 
				(AF_FLOAT)q.y(), 
				(AF_FLOAT)q.z(), 
				(AF_FLOAT)q.w() };
			
            // SYNC BACK VELOCITY
            const btVector3& vel = bulletData->bodies[i]->getLinearVelocity();
             _ecs->rigidbodies[i].velocity = { (AF_FLOAT)vel.x(), (AF_FLOAT)vel.y(), (AF_FLOAT)vel.z() };

             const btVector3& angVel = bulletData->bodies[i]->getAngularVelocity();
             _ecs->rigidbodies[i].anglularVelocity = { (AF_FLOAT)angVel.x(), (AF_FLOAT)angVel.y(), (AF_FLOAT)angVel.z() };

			// Update collider bounds

			// convert quaternion-to-euler conversion can cause gimbal lock and is not ideal, but for bounds update it should be sufficient.


			// convert from radians to degrees
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
		_collision->rayDistance = (AF_FLOAT)rayCallback.m_closestHitFraction * 1000.0f;
		_collision->collisionPoint = { 
			(AF_FLOAT)rayCallback.m_hitPointWorld.getX(), 
			(AF_FLOAT)rayCallback.m_hitPointWorld.getY(), 
			(AF_FLOAT)rayCallback.m_hitPointWorld.getZ() 
		};
		_collision->normal = {
			(AF_FLOAT)rayCallback.m_hitNormalWorld.getX(),
			(AF_FLOAT)rayCallback.m_hitNormalWorld.getY(),
			(AF_FLOAT)rayCallback.m_hitNormalWorld.getZ()
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
af_bool_t AF_Physics_Shutdown(void* _physicsEngineHandle) {
	AF_Log("AF_Physics_Shutdown: Shutting down Bullet physics engine.\n");
	if (_physicsEngineHandle == nullptr) {
		AF_Log_Error("AF_Physics_Shutdown: Physics engine handle is null during shutdown.\n");
		return AF_FALSE;
	}
	
	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);

	if(bulletData == nullptr) {
		AF_Log_Error("AF_Physics_Shutdown: Failed Shutting down Bullet physics engine. Probably causing a mem leak\n");
		return AF_FALSE;
	}

	if(bulletData->dynamicsWorld == nullptr) {
		AF_Log_Error("AF_Physics_Shutdown: Bullet dynamics world is null during shutdown. Probably causing a mem leak\n");
		return AF_FALSE;
	}
	for (int i = 0; i < AF_ECS_TOTAL_ENTITIES; ++i) {
		if (bulletData->bodies[i] != nullptr) {
			bulletData->dynamicsWorld->removeRigidBody(bulletData->bodies[i]);
		}
	}
	
	for (int i = 0; i < AF_ECS_TOTAL_ENTITIES; ++i) {
        if (bulletData->bodies[i] != nullptr) {
            
            // Delete Motion State
            btMotionState* ms = bulletData->bodies[i]->getMotionState();
            if(ms) {
                delete ms;
            }

            // Delete Collision Shape
            btCollisionShape* shape = bulletData->bodies[i]->getCollisionShape();
            if (shape) {
                delete shape;
            }
            
            // Delete Rigid Body
            delete bulletData->bodies[i];
            bulletData->bodies[i] = nullptr;
        }
        if (bulletData->upsampledHeightMaps[i] != nullptr) {
            delete[] bulletData->upsampledHeightMaps[i];
            bulletData->upsampledHeightMaps[i] = nullptr;
        }
    }
	
	delete bulletData->dynamicsWorld;
	bulletData->dynamicsWorld = nullptr;

	delete bulletData->solver;
	bulletData->solver = nullptr;

	delete bulletData->broadphase;
	bulletData->broadphase = nullptr;

	delete bulletData->dispatcher;
	bulletData->dispatcher = nullptr;

	delete bulletData->collisionConfiguration;
	bulletData->collisionConfiguration = nullptr;

	delete bulletData->debugDrawer;
	bulletData->dynamicsWorld = nullptr;

	delete bulletData;
	bulletData = nullptr;
	AF_Log("AF_Physics_Shutdown: Physics engine handle is valid, proceeding with shutdown.\n");
	return AF_TRUE;
}

void AF_Physics_LateUpdate(AF_ECS* _ecs, void* _physicsEngineHandle) { 
	(void)_ecs; (void)_physicsEngineHandle; 

}
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs, void* _physicsEngineHandle) { 
	(void)_ecs; (void)_physicsEngineHandle; 
}

// =================================================================================================
// AF_DebugDrawTriangleCallback
// Triangle callback that draws wireframe edges through the debug drawer
// =================================================================================================
class AF_DebugDrawTriangleCallback : public btTriangleCallback {
public:
	btIDebugDraw* debugDrawer;
	btTransform worldTransform;
	btVector3 color;

	AF_DebugDrawTriangleCallback(btIDebugDraw* _drawer, const btTransform& _transform, const btVector3& _color)
		: debugDrawer(_drawer), worldTransform(_transform), color(_color) {}

	void processTriangle(btVector3* triangle, int partId, int triangleIndex) override {
		(void)partId; (void)triangleIndex;
		btVector3 v0 = worldTransform * triangle[0];
		btVector3 v1 = worldTransform * triangle[1];
		btVector3 v2 = worldTransform * triangle[2];
		debugDrawer->drawLine(v0, v1, color);
		debugDrawer->drawLine(v1, v2, color);
		debugDrawer->drawLine(v2, v0, color);
	}
};

// =================================================================================================
// AF_Physics_DebugDraw
// Custom debug draw that uses AABB-limited triangle processing for terrain shapes.
// For a 2048x2048 heightfield, debugDrawWorld() iterates ALL ~8M triangles.
// This version only processes triangles within _radius of _cameraPos.
// =================================================================================================
void AF_Physics_DebugDraw(void* _physicsEngineHandle, Vec3 _cameraPos, float _radius) {
	if (_physicsEngineHandle == nullptr) {
		return;
	}
	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);
	if (bulletData->debugDrawer == nullptr || bulletData->dynamicsWorld == nullptr) {
		return;
	}
	bulletData->debugDrawer->clearLines();

	btCollisionObjectArray& objects = bulletData->dynamicsWorld->getCollisionObjectArray();
	btVector3 color(0, 1, 0); // green wireframe
	btVector3 camBt(_cameraPos.x, _cameraPos.y, _cameraPos.z);

	for (int i = 0; i < objects.size(); ++i) {
		btCollisionObject* obj = objects[i];
		btCollisionShape* shape = obj->getCollisionShape();
		btTransform worldTransform = obj->getWorldTransform();

		if (shape->getShapeType() == TERRAIN_SHAPE_PROXYTYPE) {
			// Terrain: only process triangles within _radius of camera
			// Transform camera position into the terrain's local space for the AABB query
			btVector3 localCam = worldTransform.inverse() * camBt;
			btVector3 localMin(localCam.x() - _radius, localCam.y() - _radius, localCam.z() - _radius);
			btVector3 localMax(localCam.x() + _radius, localCam.y() + _radius, localCam.z() + _radius);

			btConcaveShape* concaveShape = static_cast<btConcaveShape*>(shape);
			AF_DebugDrawTriangleCallback triCallback(bulletData->debugDrawer, worldTransform, color);
			concaveShape->processAllTriangles(&triCallback, localMin, localMax);
		} else {
			// Non-terrain: use Bullet's built-in debug draw (cheap for boxes/spheres)
			bulletData->dynamicsWorld->debugDrawObject(worldTransform, shape, color);
		}
	}
}

// =================================================================================================
// AF_Physics_GetDebugLineCount
// Returns the number of float vertices (3 floats per vertex, 2 vertices per line)
// =================================================================================================
uint32_t AF_Physics_GetDebugLineCount(void* _physicsEngineHandle) {
	if (_physicsEngineHandle == nullptr) {
		return 0;
	}
	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);
	if (bulletData->debugDrawer == nullptr) {
		return 0;
	}
	// Each line = 2 vertices = 6 floats. Vertex count = total floats / 3
	return (uint32_t)(bulletData->debugDrawer->lineVertices.size() / 3);
}

// =================================================================================================
// AF_Physics_GetDebugLineVertices
// Returns pointer to raw float vertex data (x,y,z per vertex)
// =================================================================================================
const float* AF_Physics_GetDebugLineVertices(void* _physicsEngineHandle) {
	if (_physicsEngineHandle == nullptr) {
		return nullptr;
	}
	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);
	if (bulletData->debugDrawer == nullptr || bulletData->debugDrawer->lineVertices.empty()) {
		return nullptr;
	}
	return bulletData->debugDrawer->lineVertices.data();
}

// =================================================================================================
// AF_Physics_DebugDrawClear
// Clears the debug line buffer
// =================================================================================================
void AF_Physics_DebugDrawClear(void* _physicsEngineHandle) {
	if (_physicsEngineHandle == nullptr) {
		return;
	}
	AF_BulletInternalData* bulletData = static_cast<AF_BulletInternalData*>(_physicsEngineHandle);
	if (bulletData->debugDrawer != nullptr) {
		bulletData->debugDrawer->clearLines();
	}
}


void AF_Physics_Reset(AF_ECS* _ecs, AF_ECS* _backupECS, void* _physicsEngineHandle){
	if(_ecs == NULL){
		AF_Log_Error("AF_Physics_Reset: ecs is null\n");
	}
	if(_physicsEngineHandle == NULL){
		AF_Log_Error("AF_Physics_Reset: _physics Engine handle is null\n");
	}
	if(_backupECS == NULL){
		AF_Log_Error("AF_Physics_Reset: backupECS is null\n");
	}

	
	AF_BulletInternalData* bulletInternalData = (AF_BulletInternalData*)_physicsEngineHandle;

	btVector3 zeroVector(0,0,0);

	for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {

		AF_C3DRigidbody* rb = &_ecs->rigidbodies[i];
		AF_CTransform3D* trans = &_ecs->transforms[i];
		AF_CCollider* col = &_ecs->colliders[i];
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

		// 3. Reset Transform (apply posOffset so Bullet body matches collider bounds)
		btVector3 initialPosition(
			trans->pos.x + col->posOffset.x,
			trans->pos.y + col->posOffset.y,
			trans->pos.z + col->posOffset.z
		);
		Vec4 q = trans->rot;
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
