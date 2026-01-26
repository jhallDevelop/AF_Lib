/*
===============================================================================
AF_PHYSICS_H

Implementation for Physics helper functions such as
General collision check on all entities with filtering
AABB collision detection
Some code inspired by https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials
===============================================================================
*/
#ifndef AF_PHYSICS_H
#define AF_PHYSICS_H
#include "AF_Lib_API.h"
#include "AF_Lib_Define.h"
#include "ECS/Entities/AF_Entity.h"
#include "ECS/Components/AF_CCollider.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_CTransform2D.h"
#include "ECS/Components/AF_CTerrain.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_CollisionVolume.h"
#include "AF_Debug.h"
#include "AF_Ray.h"
#include "AF_Math/AF_Vec3.h"
#include "AF_Math/AF_Vec4.h"
#include "AF_Util.h"
#include "AF_Log.h"
//#include "AF_QuadTree.h"
#define FACES_COUNT 6
#define BOX_VERTEX_COUNT 24

#ifndef AF_MAX
#define AF_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef AF_MIN
#define AF_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GRAVITY_SCALE -150.0f  // Balanced gravity for stable physics
#define LINEAR_DAMPING 0.995f   // 0.5% linear energy loss per second (frame-rate independent)
#define ANGULAR_DAMPING 0.99f  // 1% angular energy loss per second (frame-rate independent)

// Penetration resolution constants (Baumgarte stabilization)
#define PENETRATION_PERCENTAGE 0.4f  // Percentage of penetration to resolve per frame (0.2-0.8 recommended)
#define PENETRATION_SLOP 0.01f       // Allow small penetration to prevent jitter

static const Vec3 AF_PHYSICS_CUBE_COLLISION_FACES [6] =
{
	{ -1 , 0 , 0} , { 1 , 0 , 0} ,
	{ 0 , -1 , 0} , { 0 , 1 , 0} ,
	{ 0 , 0 , -1} , { 0 , 0 , 1} ,
};



// General collision resolution function
void AF_Physics_Init(AF_ECS* _ecs);
AF_LIB_API void AF_Physics_Update(AF_ECS* _ecs, const float _dt);
void AF_Physics_LateUpdate(AF_ECS* _ecs);
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs);
void AF_Physics_Shutdown(void);

// Physics system functions
AF_LIB_API void AF_Physics_Update_Bounds(AF_ECS* _ecs);
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
af_bool_t AF_Physics_Raycast(const Ray* _ray, AF_ECS* _ecs, AF_Collision* _collision) ;
void AF_Physics_DrawBox(AF_CCollider* collider, float* color);

// Math Helpers for Quaternion integration
Vec4 createQuaternionFromAngularVelocity(Vec3 angVel, float dt);
Vec4 Quat_MULT(Vec4 q1, Vec4 q2);
Mat4 QuaternionToMat4(Vec4 q);
Mat4 Mat4_ToModelMat4_Quaternion(Vec3 pos, Vec4 quat, Vec3 scale);
Vec4 AF_EulerToQuaternion(Vec3 euler);



#ifdef __cplusplus
}
#endif

#endif //AF_PHYSICS_H
