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



/*
====================
AF_PHYSICS_INIT
Definition for Physics init
====================
*/
void AF_Physics_Init(AF_ECS* _ecs);

/*
====================
AF_PHYSICS_UPDATE
Definition for Physics update
====================
*/
AF_LIB_API void AF_Physics_Update(AF_ECS* _ecs, const float _dt);
AF_LIB_API void AF_Physics_Update_Bounds(AF_ECS* _ecs);
af_bool_t AF_Physics_Collision_Test(AF_ECS* _ecs);
void AF_Physics_GetInterval(const AF_CTransform3D* transform, const Vec3* halfSize, const Vec3* axis, AF_FLOAT* min, AF_FLOAT* max);
af_bool_t AF_Physics_AABB_Test(AF_ECS* _ecs, uint32_t _entity1ID, uint32_t _entity2ID, AF_CTransform3D* transformA, AF_CCollider* colliderA, AF_CTransform3D* transformB, AF_CCollider* colliderB, AF_Collision* outCollision);
af_bool_t AF_Physics_OBB_Test(AF_ECS* _ecs, uint32_t _entity1ID, uint32_t _entity2ID, AF_CTransform3D* transformA, AF_CCollider* colliderA, AF_CTransform3D* transformB, AF_CCollider* colliderB, AF_Collision* outCollision);
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

/*
====================
AF_PHYSICS_SHUTDOWN
Definition for Physics shutdown 
====================
*/
void AF_Physics_Shutdown(void);

//=====HELPER FUNCTIONS=====


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
static inline Vec3 AF_Physics_CalculateBoxInverseInertiaTensor(Vec3 halfExtents, float inverseMass) {
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
For a diagonal tensor, this is: I_world = R * I_local * R^T
Where R is the rotation matrix extracted from the model matrix.
====================
*/
static inline Vec3 AF_Physics_TransformInertiaTensorToWorldSpace(Vec3 localInertia, Mat4 modelMat) {
	// For a diagonal inertia tensor, we can use a simplified formula
	// Extract rotation basis vectors from model matrix
	Vec3 right = Mat4_GetDirection(modelMat, 0);   // X-axis
	Vec3 up = Mat4_GetDirection(modelMat, 1);      // Y-axis  
	Vec3 forward = Mat4_GetDirection(modelMat, 2); // Z-axis
	
	// Normalize to ensure orthonormality (in case of numerical errors)
	right = Vec3_NORMALIZE(right);
	up = Vec3_NORMALIZE(up);
	forward = Vec3_NORMALIZE(forward);
	
	// Transform diagonal inertia tensor: I_world = R * I_local * R^T
	// For diagonal tensors, each component is transformed by the corresponding axis
	Vec3 worldInertia;
	worldInertia.x = localInertia.x * (right.x * right.x) + 
	                  localInertia.y * (up.x * up.x) + 
	                  localInertia.z * (forward.x * forward.x);
	                  
	worldInertia.y = localInertia.x * (right.y * right.y) + 
	                  localInertia.y * (up.y * up.y) + 
	                  localInertia.z * (forward.y * forward.y);
	                  
	worldInertia.z = localInertia.x * (right.z * right.z) + 
	                  localInertia.y * (up.z * up.z) + 
	                  localInertia.z * (forward.z * forward.z);
	
	return worldInertia;
}

/*
====================
AF_Physics_ApplyAngularImpulse
Apply angular impulse to rigidbody object.
Note: The inertia tensor should already be in world-space when calling this.
====================
*/
static inline void AF_Physics_ApplyAngularImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force){
	Vec3 angularForce = Vec3_MULT(_rigidbody->inertiaTensor, _force);
	_rigidbody->anglularVelocity = Vec3_ADD(_rigidbody->anglularVelocity, angularForce);
}

/*
====================
AF_Physics_ApplyLinearImpulse
Apply force to rigidbody object
====================
*/
static inline void AF_Physics_ApplyLinearImpulse( AF_C3DRigidbody *  _rigidbody, const Vec3 _force){
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

static inline Vec4 createQuaternionFromAngularVelocity(Vec3 angVel, float dt) {
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
static inline Vec4 Quat_MULT(Vec4 q1, Vec4 q2) {
    Vec4 result;
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return result;
}

// Convert Euler angles (in radians) to quaternion
// Standard game engine convention: euler.x=pitch, euler.y=yaw, euler.z=roll
static inline Vec4 AF_EulerToQuaternion(Vec3 euler) {
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
static inline Mat4 QuaternionToMat4(Vec4 q) {
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
static inline Mat4 Mat4_ToModelMat4_Quaternion(Vec3 pos, Vec4 quat, Vec3 scale) {
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

static inline void AF_Physics_IntegrateVelocity(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt){
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
static inline void AF_Physics_IntegrateAccell(AF_CTransform3D* _transform, AF_C3DRigidbody* _rigidbody, const float _dt){
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
        
        // == Angular Acceleration (THE FIX) ==
        // 1. Transform local inverse inertia tensor to world space
        Vec3 worldInvInertia = AF_Physics_TransformInertiaTensorToWorldSpace(_rigidbody->inertiaTensor, _transform->modelMat);
        
        // 2. Calculate angular acceleration from torque: α = I⁻¹ * τ
        Vec3 angularAcceleration = Vec3_MULT(worldInvInertia, _rigidbody->torque);
        
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
static inline af_bool_t AF_Physics_Sphere_RayIntersection(const Ray* _ray, const AF_CTransform3D* _transform, const AF_CCollider* _collider, AF_Collision* _collision){
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
static inline af_bool_t AF_Physics_Box_RayIntersection(const Ray* _ray, const Vec3 _boxPos, const Vec3 _boxSize, AF_Collision* _collision) {
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
static inline af_bool_t AF_Physics_AABB_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){
	Vec3 boxPos = _collider->boundingPos;
	Vec3* _size = &_collider->boundingVolume;
	//Vec3 boxHalfSize = Vec3_MULT_SCALAR(*_size, 0.5f);
	//Vec3 boxHalfSize = Vec3_DIV_SCALAR(*_size, 2);
	Vec3 boxHalfSize = Vec3_MULT_SCALAR(*_size, .5f);
	return AF_Physics_Box_RayIntersection(_ray, boxPos, boxHalfSize, _collision);
} 

/*
====================
AF_PHYSICS_OBB_RAYINTERSECTION
Calculate ray intersection hit test against a Object Orientated Box
====================
*/
static inline af_bool_t AF_Physics_OBB_RayIntersection(const Ray* _ray, const AF_CTransform3D* _worldTransform, const Vec3* _size, AF_Collision* _collision){
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
static inline af_bool_t AF_Physics_AABB(AF_Rect* _rect1, AF_Rect* _rect2){
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
static inline af_bool_t AF_Physics_Point_Inside_Rect(Vec2 _point, AF_Rect _rect){
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
static inline af_bool_t AF_Physics_Plane_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){

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
static inline af_bool_t AF_Physics_RayIntersection(const Ray* _ray, AF_CCollider* _collider, AF_Collision* _collision){
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
static inline void AF_Physics_ResolveCollision(AF_ECS* _ecs, uint32_t _entityAID, uint32_t _entityBID, AF_Collision* _collision){
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
	Vec3 worldInertiaTensorA = AF_Physics_TransformInertiaTensorToWorldSpace(rigidbodyA->inertiaTensor, transformA->modelMat);
	Vec3 worldInertiaTensorB = AF_Physics_TransformInertiaTensorToWorldSpace(rigidbodyB->inertiaTensor, transformB->modelMat);
	
	Vec3 crossRelativeNormalA = Vec3_CROSS(relativeA, _collision->normal);
	Vec3 tensorCrossRelativeNormalA = Vec3_MULT(worldInertiaTensorA, crossRelativeNormalA);
	Vec3 inertiaA = Vec3_CROSS(tensorCrossRelativeNormalA, relativeA);

	Vec3 crossRelativeNormalB = Vec3_CROSS(relativeB, _collision->normal);
	Vec3 tensorCrossRelativeNormalB = Vec3_MULT(worldInertiaTensorB, crossRelativeNormalB);
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
		AF_Physics_ApplyAngularImpulse(rigidbodyA, angularImpulseA);
		
		// For resting contacts, directly damp angular velocity to stop rotation
		if (isRestingContact) {
			rigidbodyA->anglularVelocity = Vec3_MULT_SCALAR(rigidbodyA->anglularVelocity, 0.9f);
		}
	}

	if (rigidbodyB->inverseMass > 0.0f && hasRigidbodyB == AF_TRUE) {
		AF_Physics_ApplyLinearImpulse(rigidbodyB, fullImpulse);
		AF_Physics_ApplyAngularImpulse(rigidbodyB, angularImpulseB);
		
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
		Vec3 tensorCrossTangentA = Vec3_MULT(worldInertiaTensorA, crossRelativeTangentA);
		Vec3 inertiaTangentA = Vec3_CROSS(tensorCrossTangentA, relativeA);

		Vec3 crossRelativeTangentB = Vec3_CROSS(relativeB, Vec3_MULT_SCALAR(tangent, -1));
		Vec3 tensorCrossTangentB = Vec3_MULT(worldInertiaTensorB, crossRelativeTangentB);
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
				AF_Physics_ApplyAngularImpulse(rigidbodyA, angularFrictionA);
			}
			if (rigidbodyB->inverseMass > 0.0f && hasRigidbodyB == AF_TRUE) {
				AF_Physics_ApplyLinearImpulse(rigidbodyB, frictionImpulse);
				Vec3 angularFrictionB = Vec3_CROSS(relativeB, frictionImpulse);
				AF_Physics_ApplyAngularImpulse(rigidbodyB, angularFrictionB);
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





//=======BROAD / NARROW PHASE========

/*
Order of execution
AF_Physics_UpdateBroadphaseAABB
||
AF_Physics_BroadPhase
||
AF_Physics_NarrowPhase
*/

/**/
static inline void AF_Physics_UpdateBroadphaseAABB(AF_CCollider* _collider){
	if(_collider->type == AABB){
		Vec3 boundingVolumeHalfDimensions = {_collider->boundingVolume.x*0.5f, _collider->boundingVolume.y*0.5f, _collider->boundingVolume.z*0.5f};
		_collider->broadphaseAABB = boundingVolumeHalfDimensions;
	}
}

/*
static void AF_Physics_BroadPhase(AF_ECS* _ecs){
	// TODO: broadphaseCollisions.clear();
	Vec2 treeSize = {1024, 1024};
	//QuadTree tree;

	for(int i = 0; i < _ecs->entitiesCount; ++i){
		Vec3 halfSizes;

		Vec3 pos = _ecs->transforms[i].pos;
		//QuadTree_Node* node = {NULL, 1, {pos.x, pos.y}, {halfSizes.x, halfSizes.y}, NULL};
		
		// Insert into our quad tree the node, its position and its half size bounds
		//AF_QuadTree_Insert(node, &_ecs->entities[i], &pos, &halfSizes, 7, 6);
		// TODO: actually insert into the quad tree
		//AF_QuadTree_Insert()

		// determine what objects may be colliding.
		
		tree.OperateOnContents([&](std::list < QuadTreeEntry < GameObject * > >& data ) {
		 CollisionInfo info ;
		
		for ( auto i = data . begin (); i != data . end (); ++ i ) {
			for ( auto j = std :: next ( i ); j != data . end (); ++ j ) {
		// is this pair of items already in the collision set -
		// if the same pair is in another quadtree node together etc
		info.a = min ((* i ).object , (* j ).object );
		info.b = max ((* i ).object , (* j ).object );
		broadphaseCollisions.insert ( info );
		

	}
}

*/

// Function to compare two CollisionInfo objects based on their hashes
static inline af_bool_t AF_Physics_CollisionInfoLessThan(const AF_Collision* info1, const AF_Collision* info2) {
    // Calculate hash for the first CollisionInfo
    size_t hash1 = (size_t)info1->entity1ID + ((size_t)info1->entity2ID << 8);
    // Calculate hash for the second CollisionInfo
    size_t hash2 = (size_t)info2->entity1ID + ((size_t)info2->entity2ID << 8);

    // Return true if the hash of info1 is less than that of info2
    return (hash1 < hash2);
}



// Function to handle narrow phase collision detection
static inline void AF_Physics_NarrowPhase(AF_Collision* broadPhaseCollisions, size_t collisionCount, int numCollisionFrames) {
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
static inline af_bool_t AF_Physics_Raycast(const Ray* _ray, AF_ECS* _ecs, AF_Collision* _collision) {
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



//=================


static inline void AF_Physics_DrawBox(AF_CCollider* collider, float* color){
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
	/*
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
	*/
	AF_Debug_DrawLineArrayWorld(vertices, BOX_VERTEX_COUNT, color, AF_FALSE);
}

#ifdef __cplusplus
}
#endif

#endif //AF_PHYSICS_H
