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

#define GRAVITY_SCALE -9.8f  // Balanced gravity for stable physics
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
void AF_Physics_Init(AF_ECS* _ecs, void** _physicsEngineHandle);
AF_LIB_API void AF_Physics_Update(AF_ECS* _ecs, void* _physicsEngineHandle, const float _dt);
void AF_Physics_LateUpdate(AF_ECS* _ecs, void* _physicsEngineHandle);
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs, void* _physicsEngineHandle);
void AF_Physics_Reset(AF_ECS* _ecs, AF_ECS* _backupECS, void* _physicsEngineHandle);
void AF_Physics_Shutdown(void* _physicsEngineHandle);

AF_LIB_API void AF_Physics_Update_Bounds(AF_ECS* _ecs);
af_bool_t AF_Physics_Raycast(const Ray* _ray, AF_ECS* _ecs, void* _physicsEngineHandle, AF_Collision* _collision);






#ifdef __cplusplus
}
#endif

#endif //AF_PHYSICS_H
