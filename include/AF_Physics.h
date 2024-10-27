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
#include "AF_Debug.h"
#include "AF_Ray.h"
#include "AF_Vec3.h"
#include "AF_Util.h"
#ifdef __cplusplus
extern "C" {
#endif

#define GRAVITY_SCALE -9.8
const float DAMPING_FACTOR = 0.05f;

static const Vec3 AF_PHYSICS_CUBE_COLLISION_FACES [6] =
{
	{ -1 , 0 , 0} , { 1 , 0 , 0} ,
	{ 0 , -1 , 0} , { 0 , 1 , 0} ,
	{ 0 , 0 , -1} , { 0 , 0 , 1} ,
};


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
void AF_Physics_Update(AF_ECS* _ecs, const float _dt);

/*
====================
AF_PHYSICS_SHUTDOWN
Definition for Physics shutdown 
====================
*/
void AF_Physics_Shutdown();

//=====HELPER FUNCTIONS=====

/*
====================
AF_PHYSICS_INTEGRATEVELOCITY
Integrate the position and some dampening into the velocity
====================
*/
static void AF_Physics_IntegrateVelocity(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt){
	float frameDamping = powf ( DAMPING_FACTOR, _dt);

	Vec3 position = _transform->pos;
	Vec3 linearVelocity = _rigidbody->velocity;
	Vec3 linearDT = Vec3_MULT_SCALAR(linearVelocity, _dt);
	position = Vec3_ADD(position, linearDT);
	_transform->pos = position;

	// LinearDamping
	linearVelocity = Vec3_MULT_SCALAR(linearVelocity, frameDamping);
	_rigidbody->velocity = linearVelocity;
}

/*
====================
AF_PHYSICS_INTEGRATEACCELL
Integrate gravity and acceleration into the velocity
====================
*/
static void AF_Physics_IntegrateAccell(AF_C3DRigidbody* _rigidbody, const float _dt){
	// iterate over all the game objects
	if(AF_Component_GetEnabled(_rigidbody->enabled != TRUE)){
		return;
	}
	float inverseMass = _rigidbody->inverseMass;

	Vec3 linearVel = _rigidbody->velocity;
	Vec3 force = _rigidbody->force;
	Vec3 accell = Vec3_MULT_SCALAR(force, inverseMass);

	if(_rigidbody->gravity == TRUE && inverseMass > 0){
		Vec3 accelGravity = {accell.x, accell.y + GRAVITY_SCALE, accell.z};
		accell = accelGravity;
	}

	Vec3 accelDT = Vec3_MULT_SCALAR(accell, _dt);
	linearVel = Vec3_ADD(linearVel, accelDT);
	_rigidbody->velocity = linearVel;
	
}


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
		//debugf("AF_Physics_Box_RayIntersection: no intersection, plane is behind the ray \n");
		return returnValue;
	}

	Vec3 intersection = Vec3_ADD(_ray->position, Vec3_MULT_SCALAR(_ray->direction, t));
	

	// Check if the intersection is within the desired plane bounds (if the plane is finite)
	if(_size->y == 0 && _size->x == 0 && _size->z == 0){
		// bounds are all 0 so plane is counted as infinite.
		//debugf("AF_Physics_Plane_RayIntersection: intersection on infintie plane at x: %f y: %f z: %f \n", intersection.x, intersection.y, intersection.z);
		returnValue = TRUE;
		_collision->collisionPoint = intersection;
		_collision->rayDistance = t;
		return returnValue;
	}

	// Check if within size/bounds
	Vec2 intersectionPoint = {intersection.x, intersection.z};
	AF_Rect planeRect = {-_size->x/2, -_size->z/2, _size->x, _size->z};
	if(AF_Physics_Point_Inside_Rect(intersectionPoint, planeRect) == FALSE){
		//debugf("AF_Physics_Box_RayIntersection: no intersection, ray is out of bounds \nintX: %f intY: %f rectX:%f rectY:%f rectz:%f  \n", intersectionPoint.x, intersectionPoint.y, _size->x, _size->y, _size->z);
		returnValue = FALSE;
		return returnValue;
	}
	
	//debugf("AF_Physics_Plane_RayIntersection: intersection within bounds x: %f y: %f z: %f \n", intersection.x, intersection.y, intersection.z);
	returnValue = TRUE;
	
	// For example, if the plane is bounded within an XZ range, you can check here

	_collision->collisionPoint = intersection;
	_collision->rayDistance = t;

	
	return returnValue;
}

static void AF_Physics_DrawBox(AF_CCollider* collider, float* color){
	// render debug collider
                //draw all edges
                //if(collider->type == Plane){
	Vec3 pos = collider->pos;//_ecs[i].transforms->pos;
	Vec3 bounds = collider->boundingVolume;
	// Top
	/*
	Vec3 top_bottomLeft = {pos.x - bounds.x/2, pos.x + bounds.x/2, pos.z - bounds.z/2};
	Vec3 top_topLeft =  {pos.x - bounds.x/2, pos.x + bounds.x/2, pos.z + bounds.z/2};
	Vec3 top_topRight =  {pos.x + bounds.x/2, pos.x + bounds.x/2, pos.z + bounds.z/2};
	Vec3 top_bottomRight =  {pos.x + bounds.x/2, pos.x + bounds.x/2, pos.z - bounds.z/2};
	*/
	
	const int verticesCount = 24;  // Total lines: 12 edges * 2 vertices per edge
	Vec3 vertices[verticesCount];

	// Top face vertices
	vertices[0] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomLeft
	vertices[1] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topLeft

	vertices[2] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topLeft
	vertices[3] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topRight

	vertices[4] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topRight
	vertices[5] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomRight

	vertices[6] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomRight
	vertices[7] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomLeft

	// Bottom face vertices
	vertices[8]  = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomLeft
	vertices[9]  = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topLeft

	vertices[10] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topLeft
	vertices[11] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topRight

	vertices[12] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topRight
	vertices[13] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomRight

	vertices[14] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomRight
	vertices[15] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomLeft

	// Vertical edges connecting top and bottom faces
	vertices[16] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomLeft
	vertices[17] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomLeft

	vertices[18] = (Vec3){pos.x - bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topLeft
	vertices[19] = (Vec3){pos.x - bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topLeft

	vertices[20] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z + bounds.z/2};  // top-topRight
	vertices[21] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z + bounds.z/2};  // bottom-topRight

	vertices[22] = (Vec3){pos.x + bounds.x/2, pos.y + bounds.y/2, pos.z - bounds.z/2};  // top-bottomRight
	vertices[23] = (Vec3){pos.x + bounds.x/2, pos.y - bounds.y/2, pos.z - bounds.z/2};  // bottom-bottomRight


	

	AF_Debug_DrawLineArrayWorld(vertices, verticesCount, color, FALSE);
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



/*
====================
AF_Physics_ApplyLinearImpulse
Apply force to rigidbody object
====================
*/
static void AF_Physics_ApplyLinearImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force){
	_rigidbody->velocity = Vec3_MULT_SCALAR(_force, _rigidbody->inverseMass);
}

/*
====================
AF_PHYSICS_AABB_Test
Calculate ray intersection hit test
====================
*/
static inline BOOL AF_Physics_AABB_Test(AF_ECS* _ecs){
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
	BOOL returnValue = FALSE;
	for(int i = 0; i < _ecs->entitiesCount; ++i){

		if(AF_Component_GetEnabled(_ecs->colliders[i].enabled == FALSE)){
			continue;
		}
		AF_Entity* entity1 = &_ecs->entities[i];
		AF_CCollider* collider1 = entity1->collider;
		
		
		// rayIntersectionTest everything
		for(int x = 0; x < _ecs->entitiesCount; ++x){
			if(AF_Component_GetEnabled(_ecs->colliders[x].enabled == FALSE)){
				continue;
			}

			// check self
			if(i == x){
				continue;
			}

			AF_Entity* entity2 = &_ecs->entities[x];
			AF_CCollider* collider2 = entity2->collider;


			Vec3* posA = &_ecs->transforms[i].pos;
			Vec3* posB = &_ecs->transforms[x].pos;
			Vec3 halfSizeA = Vec3_DIV_SCALAR(collider1->boundingVolume, 2);
			Vec3 halfSizeB = Vec3_DIV_SCALAR(collider2->boundingVolume, 2);

			Vec3 delta = Vec3_MINUS(*posA, *posB);
			Vec3 totalSize = Vec3_ADD(halfSizeA, halfSizeB);

			if(
				abs(delta.x) < totalSize.x  &&
				abs(delta.y) < totalSize.y && 
				abs(delta.z) < totalSize.z){

					// TODO: determine from what direction the collision occurs
					//ResolveCollision(_ecs, i, x);
					returnValue = TRUE;
					// Resolve collision
					//AF_PHYSICS_CUBE_COLLISION_FACES
					// Get the min and max of each cube
					Vec3 maxA = Vec3_ADD(collider1->pos, collider1->boundingVolume);
					Vec3 minA = Vec3_MINUS(collider1->pos, collider1->boundingVolume);

					Vec3 maxB = Vec3_ADD(collider2->pos, collider2->boundingVolume);
					Vec3 minB = Vec3_MINUS(collider2->pos, collider2->boundingVolume);

					int facesCount = 6;
					float distances [facesCount];
					
						 distances[0] = maxB.x - minA.x; // distance of box ’b ’ to ’ left ’ of ’a ’.
						 distances[1] = maxA.x - minB.x; // distance of box ’b ’ to ’ right ’ of ’a ’.
						 distances[2] = maxB.y - minA.y; // distance of box ’b ’ to ’ bottom ’ of ’a ’.
						 distances[3] = maxA.y - minB.y; // distance of box ’b ’ to ’ top ’ of ’a ’.
						 distances[4] = maxB.z - minA.z; // distance of box ’b ’ to ’ far ’ of ’a ’.
						 distances[5] = maxA.z - minB.z;  // distance of box ’b ’ to ’ near ’ of ’a ’.
					


					//TODO: where is __FLT_MAX__ defined? may not be portable
					float penetration = __FLT_MAX__;
					Vec3 bestAxis = {0,0,0};	// default value
					for(int j = 0; j < facesCount; ++j){
						if(distances[j] < penetration){
							penetration = distances[j];
							bestAxis = AF_PHYSICS_CUBE_COLLISION_FACES[j]; 
						}
					}

					// create a new collision struct
					AF_Collision collision1 = {returnValue, entity1, entity2, collider1->collision.callback, {0,0,0}, 0.0f, bestAxis, penetration}; 
					// TODO: i think the bestAxis should be inverted for the second object
					AF_Collision collision2 = {returnValue, entity2, entity1, collider2->collision.callback, {0,0,0}, 0.0f, Vec3_MULT_SCALAR(bestAxis, -1), penetration}; 
					
					// copy the new struct values to each collider
					collider1->collision = collision1;
					collider2->collision = collision2;

					collider1->collision.callback(&collider1->collision);
					collider2->collision.callback(&collider2->collision);

					// Apply collision resolution
					AF_CTransform3D* transform1 = &_ecs->transforms[i];
					AF_CTransform3D* transform2 = &_ecs->transforms[x];

					AF_C3DRigidbody* rigidbody1 = &_ecs->rigidbodies[i];
					AF_C3DRigidbody* rigidbody2 = &_ecs->rigidbodies[x];

					

					
					// compbined mass between two objects
					float totalMass = rigidbody1->inverseMass + rigidbody2->inverseMass;
					float entity1AdjMass = rigidbody1->inverseMass / totalMass;
					float entity2AdjMass = rigidbody2->inverseMass / totalMass;

					float penetrationScale = 0.075f; // Adjust as needed

					Vec3 adjustedPosition1 = Vec3_MINUS(transform1->pos, Vec3_MULT_SCALAR(collider1->collision.normal, (collider1->collision.penetration * entity1AdjMass * penetrationScale)));
					Vec3 adjustedPosition2 = Vec3_MINUS(transform2->pos, Vec3_MULT_SCALAR(collider2->collision.normal, (collider2->collision.penetration * entity2AdjMass * penetrationScale)));


					//Vec3 adjustedPosition1 = Vec3_MINUS(transform1->pos, Vec3_MULT_SCALAR(collider1->collision.normal, (collider1->collision.penetration * entity1AdjMass)));
					//Vec3 adjustedPosition2 = Vec3_MINUS(transform2->pos, Vec3_MULT_SCALAR(collider2->collision.normal, (collider2->collision.penetration * entity2AdjMass)));
					transform1->pos = adjustedPosition1;
					transform2->pos = adjustedPosition2;

					// calculate the relative velocity
					Vec3 relativeVelocity = Vec3_MINUS(rigidbody1->velocity, rigidbody2->velocity);

					// calcualte the impulse along the collisio normal
					float impulseForce = Vec3_DOT(relativeVelocity, collider1->collision.normal) * -1.0f;
					float restitution = 0.2f; // elasticity factor (0 = inelastic, 1 = fully elastic)

					Vec3 impulse = Vec3_MULT_SCALAR(collider1->collision.normal, impulseForce * restitution);

					// Apply the impulse to each object's velocity, scaled by their mass ratios
					Vec3 newVel1 = Vec3_ADD(rigidbody1->velocity, Vec3_MULT_SCALAR(impulse, entity1AdjMass));
					Vec3 newVel2 =  Vec3_ADD(rigidbody2->velocity, Vec3_MULT_SCALAR(impulse, -entity2AdjMass));
					rigidbody1->velocity = newVel1;
					rigidbody2->velocity = newVel2;

					//debugf("AF_Physics: AABB: newVel1 x: %f y: %f z: %f \n", newVel1.x, newVel1.y, newVel1.z);
					//debugf("AF_Physics: AABB: newVel2 x: %f y: %f z: %f \n", newVel2.x, newVel2.y, newVel2.z);
				}
		}
	}
	return returnValue;
}

static void AF_Physics_EarlyUpdate(AF_ECS* _ecs){
	// clear the velocities
	for(int i =0 ; _ecs->entitiesCount; ++i){
		AF_C3DRigidbody* rigidbody =  &_ecs->rigidbodies[i];
		// clear the velocity
		Vec3 zeroVelocity = {0,0,0};
		rigidbody->velocity = zeroVelocity;
	}
}

/*
====================
AF_Physics_LateUpdate
functino to be called during late update. allows rendering debug to occur and to occur after movmeent
======
*/
void AF_Physics_LateUpdate(AF_ECS* _ecs);

/*
====================
AF_Physics_LateRenderUpdate
functino to be called during late update. allows rendering debug to occur and to occur inbetween render calls, allowing debug rendering to occur
======
*/
void AF_Physics_LateRenderUpdate(AF_ECS* _ecs);

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
