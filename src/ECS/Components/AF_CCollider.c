#include "ECS/Components/AF_CCollider.h"
#include "ECS/Components/AF_Component.h"


// dummy callback so our initialisation compiles when we call AF_CCollider_Add
void AF_Collision_DummyCallback(AF_Collision* _collisionPtr){
	if(_collisionPtr){}
}
/*
====================
AF_CCollider_ZERO
Add the CCollider component
enable the component and set some values to default values.
====================
*/
AF_CCollider AF_CCollider_ZERO(void){
	PACKED_CHAR component = AF_FALSE;
	component = AF_Component_SetHas(component, AF_FALSE);
	component = AF_Component_SetEnabled(component, AF_FALSE);
	AF_CCollider collider = {
		.enabled = component,
		.type = AF_COLLISION_TYPE_AABB,
		.boundingVolume = {1,1,1},
		.posOffset = {0,0,0},
		.boundingPos = {0,0,0},
		.boundingRot = {0,0,0},
		.showDebug = AF_FALSE,
		.collisionMeshData = AF_MeshData_ZERO(),
		.broadphaseAABB = {0,0,0}
	};

    collider.collision.collided  = AF_FALSE;
	collider.collision.entity1ID = 99999;
	collider.collision.entity2ID = 99999;
	collider.collision.callback = AF_Collision_DummyCallback;//(void (*)(AF_Collision))0;
	Vec3 defaultCollisionPoint = {0,0,0};
	collider.collision.collisionPoint = defaultCollisionPoint;
	collider.collision.rayDistance = 0.0f;
	collider.collision.ecsPtr = NULL;
	return collider;
}


/*
====================
AF_CCollider_ADD
Add the CCollider component
enable the component and set some values to default values.
====================
*/
AF_CCollider AF_CCollider_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CCollider collider = {
		.enabled = component,
		.type = AF_COLLISION_TYPE_AABB,
		.boundingVolume = {1,1,1},
		.posOffset = {0,0,0},
		.boundingPos = {0,0,0},
		.boundingRot = {0,0,0},
		.showDebug = AF_FALSE,
		.broadphaseAABB = {0,0,0}
	};
        collider.collision.collided  = AF_FALSE;
	collider.collision.entity1ID = 9999;
	collider.collision.entity2ID = 9999;
	collider.collision.callback = AF_Collision_DummyCallback;//(void (*)(AF_Collision))0;
	Vec3 defaultCollisionPoint = {0,0,0};
	collider.collision.collisionPoint = defaultCollisionPoint;
	collider.collision.rayDistance = 0.0f;
	collider.collision.ecsPtr = NULL;
	return collider;
}



/*
====================
AF_CCollider_SPHERE_ADD
Add the CCollider component
enable the component and set some values to default values.
====================
*/
AF_CCollider AF_CCollider_Sphere_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CCollider collider = {
		.enabled = component,
		.type = AF_COLLISION_TYPE_Sphere,
		.boundingVolume = {1,1,1},
		.posOffset = {0,0,0},
		.boundingPos = {0,0,0},
		.boundingRot = {0,0,0},
		.showDebug = AF_FALSE,
		.broadphaseAABB = {0,0,0}
	};
        collider.collision.collided  = AF_FALSE;
	collider.collision.entity1ID = 9999;
	collider.collision.entity2ID = 9999;
	collider.collision.callback = AF_Collision_DummyCallback;//(void (*)(AF_Collision))0;
	Vec3 defaultCollisionPoint = {0,0,0};
	collider.collision.collisionPoint = defaultCollisionPoint;
	collider.collision.rayDistance = 0.0f;
	collider.collision.ecsPtr = NULL;
	return collider;
}
//
/*
====================
AF_CCollider_BOX_ADD
Add the CCollider component
enable the component and set some values to default values.
====================
*/
AF_CCollider AF_CCollider_Box_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CCollider collider = {
		.enabled = component,
		.type = AF_COLLISION_TYPE_AABB,
		.boundingVolume = {1,1,1},
		.posOffset = {0,0,0},
		.boundingPos = {0,0,0},
		.boundingRot = {0,0,0},
		.showDebug = AF_FALSE,
		.broadphaseAABB = {0,0,0}
	};
        collider.collision.collided  = AF_FALSE;
	collider.collision.entity1ID = 9999;
	collider.collision.entity2ID = 9999;
	collider.collision.callback = AF_Collision_DummyCallback;//(void (*)(AF_Collision))0;
	Vec3 defaultCollisionPoint = {0,0,0};
	collider.collision.collisionPoint = defaultCollisionPoint;
	collider.collision.rayDistance = 0.0f;
	collider.collision.ecsPtr = NULL;
	return collider;
}

/*
====================
AF_CCollider_PLANE_ADD
Add the CCollider component
enable the component and set some values to default values.
====================
*/
AF_CCollider AF_CCollider_Plane_ADD(void){
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_CCollider collider = {
		.enabled = component,
		.type = AF_COLLISION_TYPE_Plane,
		.boundingVolume = {1,1,1},
		.posOffset = {0,0,0},
		.boundingPos = {0,0,0},
		.boundingRot = {0,0,0},
		.showDebug = AF_FALSE,
		.broadphaseAABB = {0,0,0}
	};
        collider.collision.collided  = AF_FALSE;
	collider.collision.entity1ID = 9999;
	collider.collision.entity2ID = 9999;
	collider.collision.callback = AF_Collision_DummyCallback;
	Vec3 defaultCollisionPoint = {0,0,0};
	collider.collision.collisionPoint = defaultCollisionPoint;
	collider.collision.rayDistance = 0.0f;
	collider.collision.ecsPtr = NULL;
	return collider;
}


/*
====================
AF_CCollider_ADD_TYPE
Add the CCollider of the MESH Type
enable the component and set some values to default values.
====================
*/

AF_CCollider AF_CCollider_ADD_TYPE(enum AF_CollisionVolumeType_e _volumeType){
	switch(_volumeType){
		case  AF_COLLISION_TYPE_AABB:
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_OBB:
			// TODO add OBB collider type
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_Plane:
			return AF_CCollider_Plane_ADD();
		break;

		case AF_COLLISION_TYPE_Sphere:
			// todo, add sphere collider type
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_Mesh:
			// todo, add mesh collider type
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_Capsule:
			// TODO: add capsule collider type
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_Terrain:
			// TODO: add terrain collider type
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_ConvexHull:
			// TODO: add convex hull collider type
			return AF_CCollider_Box_ADD();
		break;

		case AF_COLLISION_TYPE_Invalid:
			return AF_CCollider_Box_ADD();
		break;

		default:
			return AF_CCollider_Box_ADD();
		break;
	}
}
