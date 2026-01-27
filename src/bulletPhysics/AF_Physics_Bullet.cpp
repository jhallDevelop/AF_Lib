#include "AF_Physics.h"
#include <btBulletDynamicsCommon.h>
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
		bulletData->bodies[i] = nullptr;
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

		bool isEnabled = AF_Component_GetHasEnabled(rb->enabled);

		if (isEnabled && bulletData->bodies[i] == nullptr) {
			btCollisionShape* shape = nullptr;
			if (col->type == Sphere) {
				shape = new btSphereShape(col->boundingVolume.x);
			} else {
				// Bullet box extents are half-extents
				shape = new btBoxShape(btVector3(col->boundingVolume.x, col->boundingVolume.y, col->boundingVolume.z));
			}

			btTransform btTrans;
			btTrans.setIdentity();
			btTrans.setOrigin(btVector3(trans->pos.x, trans->pos.y, trans->pos.z));
			
			Vec4 q = AF_EulerToQuaternion(Vec3_MULT_SCALAR(trans->rot, AF_PI / 180.0f));
			btTrans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));

			btScalar mass(rb->inverseMass > 0 ? 1.0f / rb->inverseMass : 0.0f);
			btVector3 localInertia(0, 0, 0);
			if (mass > 0) shape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* motionState = new btDefaultMotionState(btTrans);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
			
			// Simple user pointer to store ECS ID for reconstruction during raycasts
			body->setUserIndex(i);

			bulletData->dynamicsWorld->addRigidBody(body);
			bulletData->bodies[i] = body;
		} 
		else if (!isEnabled && bulletData->bodies[i] != nullptr) {
			bulletData->dynamicsWorld->removeRigidBody(bulletData->bodies[i]);
			delete bulletData->bodies[i]->getMotionState();
			delete bulletData->bodies[i]->getCollisionShape();
			delete bulletData->bodies[i];
			bulletData->bodies[i] = nullptr;
		}
	}

	// 2. Step simulation
	bulletData->dynamicsWorld->stepSimulation(_dt, 10);

	// 3. Sync Bullet state back to ECS
	for (uint32_t i = 0; i < _ecs->entitiesCount; ++i) {
		if (bulletData->bodies[i]) {
			btTransform btTrans;
			bulletData->bodies[i]->getMotionState()->getWorldTransform(btTrans);

			_ecs->transforms[i].pos.x = btTrans.getOrigin().getX();
			_ecs->transforms[i].pos.y = btTrans.getOrigin().getY();
			_ecs->transforms[i].pos.z = btTrans.getOrigin().getZ();

			btQuaternion q = btTrans.getRotation();
			_ecs->transforms[i].orientation = { (float)q.x(), (float)q.y(), (float)q.z(), (float)q.w() };
			
			// Reset collisions
			AF_Collision_Reset(&_ecs->colliders[i].collision);
		}
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
	if (_physicsEngineHandle == nullptr) return;

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

void AF_Physics_LateUpdate(AF_ECS* _ecs, void* _physicsEngineHandle) { (void)_ecs; (void)_physicsEngineHandle; }
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs, void* _physicsEngineHandle) { (void)_ecs; (void)_physicsEngineHandle; }

} // extern "C"
