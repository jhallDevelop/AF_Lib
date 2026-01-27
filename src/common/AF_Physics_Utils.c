#include "AF_Physics.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========================================================================
// AF_Physics_Update_Bounds
// Update the physics bounds (used in Editor mode)
// ========================================================================
void AF_Physics_Update_Bounds(AF_ECS* _ecs)
{
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_CCollider* collider = &_ecs->colliders[i];
		collider->boundingPos = Vec3_ADD(_ecs->transforms[i].pos, collider->posOffset);
		collider->boundingRot = _ecs->transforms[i].rot;
	}
}

// ========================================================================
// Math Helpers
// ========================================================================

Vec4 createQuaternionFromAngularVelocity(Vec3 angVel, float dt) {
    float halfDt = dt * 0.5f;
    float angleMagnitude = Vec3_MAGNITUDE(angVel);
    
    if (angleMagnitude < 0.0001f) {
        return Vec4_ZERO();
    }
    
    float halfAngle = angleMagnitude * halfDt;
    float w = cosf(halfAngle);
    float sinHalfAngle = sinf(halfAngle);

    float scale = sinHalfAngle / angleMagnitude;
    Vec3 vectorPart = Vec3_MULT_SCALAR(angVel, scale);

    Vec4 q = { vectorPart.x, vectorPart.y, vectorPart.z, w };
    return q;
}

Vec4 Quat_MULT(Vec4 q1, Vec4 q2) {
    Vec4 result;
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return result;
}

Vec4 AF_EulerToQuaternion(Vec3 euler) {
    float cp = cosf(euler.x * 0.5f);
    float sp = sinf(euler.x * 0.5f);
    float cy = cosf(euler.y * 0.5f);
    float sy = sinf(euler.y * 0.5f);
    float cr = cosf(euler.z * 0.5f);
    float sr = sinf(euler.z * 0.5f);

    Vec4 q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    
    return q;
}

Mat4 QuaternionToMat4(Vec4 q) {
    float mag = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (mag > 0.0001f) {
        q.x /= mag; q.y /= mag; q.z /= mag; q.w /= mag;
    }
    
    float xx = q.x * q.x; float yy = q.y * q.y; float zz = q.z * q.z;
    float xy = q.x * q.y; float xz = q.x * q.z; float yz = q.y * q.z;
    float wx = q.w * q.x; float wy = q.w * q.y; float wz = q.w * q.z;

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

    mat.rows[3].x = 0.0f; mat.rows[3].y = 0.0f; mat.rows[3].z = 0.0f; mat.rows[3].w = 1.0f;

    return mat;
}

Mat4 Mat4_ToModelMat4_Quaternion(Vec3 pos, Vec4 quat, Vec3 scale) {
    Mat4 rot = QuaternionToMat4(quat);
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

#ifdef __cplusplus
}
#endif
