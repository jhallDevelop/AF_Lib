/*
    ====================
    VEC4_H
    Author Name: jhalldevelop
    This header file defines 3D vector operations such as addition, subtraction,
    dot product, cross product, projection, normalization, magnitude, and orthogonalization.
    The functions are implemented as inline to improve performance by avoiding function call overhead.
    The vector structure is designed to store 3D vectors (x, y, z, 1) and operates with AF_FLOAT types.
    ====================
*/

#if defined(_WIN32) || defined(_WIN64)
// This part is for Windows platforms (MSVC compiler)
#ifdef AF_MATH_EXPORTS // This macro should be defined by the library project itself
#define AF_MATH_API __declspec(dllexport)
#else
#define AF_MATH_API __declspec(dllimport)
#endif
#else
// This part is for non-Windows platforms (GCC, Clang on macOS, Linux)
#define AF_MATH_API __attribute__((visibility("default")))
#endif

#ifndef VEC4_H
#define VEC4_H
#include "AF_Math/AF_Math_Define.h"
#include "AF_Math/AF_Math.h"
#include "AF_Math/AF_Vec3.h"
#ifdef __cplusplus
extern "C" {
#endif

    /*
    ====================
    Vec4 Struct
    Defines a 3D vector with x, y, z, w components.
    ====================
    */
typedef struct {
        AF_FLOAT x, y, z, w;
    } Vec4;

    /*
    ====================
    Vec4_ZERO
    Returns a zero-initialized vector (0, 0, 0, 0).
    ====================
    */
    static inline Vec4 Vec4_ZERO(void){
        Vec4 returnVec = {0, 0, 0, 0};
        return returnVec;
    }

    /*
    ====================
    Vec4_ADD
    Adds two vectors element-wise and returns the result.
    ====================
    */
    static inline Vec4 Vec4_ADD(Vec4 v1, Vec4 v2)
    {
        Vec4 result = {0, 0, 0, 0};
        result.x = v1.x + v2.x;
        result.y = v1.y + v2.y;
        result.z = v1.z + v2.z;
        result.w = v1.w + v2.w;
        return result;  
    } 

    /*
    ====================
    Vec4_MINUS
    Subtracts vector v2 from v1 element-wise and returns the result.
    ====================
    */
    static inline Vec4 Vec4_MINUS(Vec4 v1, Vec4 v2)
    {
        Vec4 result = {0, 0, 0, 0};
        result.x = v1.x - v2.x;
        result.y = v1.y - v2.y;
        result.z = v1.z - v2.z;
        result.w = v1.w - v2.w;
        return result;
    }

    /*
    ====================
    Vec4_MULT_SCALAR
    Multiplies each element of vector v by a scalar value f.
    ====================
    */
    static inline Vec4 Vec4_MULT_SCALAR(Vec4 v, AF_FLOAT f)
    {
        Vec4 result = {0, 0, 0, 0};
        result.x = v.x * f;
        result.y = v.y * f;
        result.z = v.z * f;
        result.w = v.w * f;
        return result;
    }

    /*
    ====================
    Vec4_MULT
    Multiplies two vectors element-wise and returns the result.
    ====================
    */
    static inline Vec4 Vec4_MULT(Vec4 v1, Vec4 v2)
    {
        Vec4 result = {0, 0, 0, 0};
        result.x = v1.x * v2.x;
        result.y = v1.y * v2.y;
        result.z = v1.z * v2.z;
        result.w = v1.w * v2.w;
        return result;
    }

    /*
    ====================
    Vec4_DIV_SCALAR
    Divides each element of vector v by scalar f.
    ====================
    */
    static inline Vec4 Vec4_DIV_SCALAR(Vec4 v, AF_FLOAT f)    
    {
        Vec4 result = {0, 0, 0, 0};
        result.x = v.x / f;
        result.y = v.y / f;
        result.z = v.z / f;
        result.w = v.w / f;
        return result;
    }

    /*
    ====================
    Vec4_EQUAL
    Returns 1 (true) if two vectors are equal element-wise, otherwise returns 0 (false).
    ====================
    */
    static inline char Vec4_EQUAL(Vec4 v1, Vec4 v2)
    {
        return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w);
    }

    /*
    ====================
    Vec4_DIV
    Divides vector v1 by vector v2 element-wise.
    ====================
    */
    static inline Vec4 Vec4_DIV(Vec4 v1, Vec4 v2)
    {
        Vec4 result = {0, 0, 0, 0};
        result.x = v1.x / v2.x;
        result.y = v1.y / v2.y;
        result.z = v1.z / v2.z;
        result.w = v1.w / v2.w;
        return result;
    }

    /*
    ====================
    Vec4_DOT
    Computes the dot product of two vectors.
    ====================
    */
    static inline AF_FLOAT Vec4_DOT(Vec4 v1, Vec4 v2)
    {
        AF_FLOAT _dot = 0;
        _dot += v1.x * v2.x;
        _dot += v1.y * v2.y;
        _dot += v1.z * v2.z;
        _dot += v1.w * v2.w;
        return _dot;
    }

    /*
    ====================
    Vec4_CROSS
    Computes the cross product of two 3D vectors (ignoring the w component).
    The result will have a w component of 0.
    ====================
    */
    static inline Vec4 Vec4_CROSS(Vec4 v1, Vec4 v2)
    {
        Vec4 cross = {0, 0, 0, 0};
        cross.x = v1.y * v2.z - v1.z * v2.y;
        cross.y = v1.z * v2.x - v1.x * v2.z;
        cross.z = v1.x * v2.y - v1.y * v2.x;
        cross.w = 0; // Cross product can only be calculated with 3D vectors
        return cross;
    }

    /*
    ====================
    Vec4_NORMALIZE
    Normalizes the vector (i.e., scales it to have a magnitude of 1).
    If the vector is of zero magnitude, it is returned unmodified.
    ====================
    */
    static inline Vec4 Vec4_NORMALIZE(Vec4 v)
    {   
        AF_FLOAT magnitude = AF_Math_Sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        AF_FLOAT epsilon = AF_EPSILON; // Threshold for considering magnitude as zero
        if (magnitude < epsilon) {
            // Return a default unit vector or other appropriate error handling
            return v;
        }
        Vec4 result = { v.x / magnitude, v.y / magnitude, v.z / magnitude, v.w / magnitude };
        return result;
    }

    /*
    ====================
    Vec4_MAGNITUDE
    Returns the magnitude (length) of the vector.
    ====================
    */
    static inline AF_FLOAT Vec4_MAGNITUDE(Vec4 v)
    {
        AF_FLOAT magnitude = 0;
        magnitude += v.x * v.x;
        magnitude += v.y * v.y;
        magnitude += v.z * v.z;
        magnitude += v.w * v.w;
        AF_FLOAT sqrMag = AF_Math_Sqrt(magnitude); 
        return sqrMag;
    }

    /*
    ====================
    Vec4_DISTANCE
    Computes the Euclidean distance between two vectors.
    ====================
    */
    static inline AF_FLOAT Vec4_DISTANCE(Vec4 v1, Vec4 v2)
    {
        AF_FLOAT distance = 0;
        distance += (v1.x - v2.x) * (v1.x - v2.x);
        distance += (v1.y - v2.y) * (v1.y - v2.y);
        distance += (v1.z - v2.z) * (v1.z - v2.z);
        distance += (v1.w - v2.w) * (v1.w - v2.w);
        return AF_Math_Sqrt(distance);
    }

    /*
    ====================
    Vec4_PROJECTION
    Projects vector v1 onto vector v2. 
    If the denominator is zero, a zero vector is returned.
    ====================
    */
    static inline Vec4 Vec4_PROJECTION(Vec4 v1, Vec4 v2)
    {
        // project v1 onto v2 using the formula: P = (P.Q/|Q|^2) * Q

        // Dot v1 . v2
        AF_FLOAT nom = Vec4_DOT(v1, v2);

        // Magnitude squared of v2
        AF_FLOAT denom = Vec4_MAGNITUDE(v2);
        denom *= denom;

        // Check for divide by zero
        AF_FLOAT epsilon = AF_EPSILON; // Threshold for considering magnitude as zero
        if (denom < epsilon) {
            Vec4 returnVec = {0, 0, 0, 0};
            return returnVec;
        }

        AF_FLOAT scalar = nom / denom;
        
        // scalar * v2
        Vec4 v3 = {0, 0, 0, 0};
        v3 = Vec4_MULT_SCALAR(v2, scalar);
        return v3;
    }

    /*
    ====================
    Vec4_ORTHOGONALISE
    Orthogonalizes vector v1 relative to vector v2 by subtracting
    the projection of v1 onto v2 from v1 itself.
    ====================
    */
    static inline Vec4 Vec4_ORTHOGONALISE(Vec4 v1, Vec4 v2)
    {
        Vec4 proj = Vec4_PROJECTION(v1, v2);
        Vec4 orthogonal = Vec4_MINUS(v1, proj);
        return orthogonal;
    }

    // =======================================
    // AF_Mat4_Quat_MULT
    // Quaternion multiplication (Hamilton product)
    // =======================================
    static inline Vec4 AF_Mat4_Quat_MULT(Vec4 q1, Vec4 q2) {
        Vec4 result;
        result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
        result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
        result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
        result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
        return result;
    }

    // =======================================
    // AF_Vec4_EulerToQuaternion
    // Convert Euler angles (in radians) to quaternion
    // Convention: euler.x=pitch (X-axis), euler.y=yaw (Y-axis), euler.z=roll (Z-axis)
    // Using YXZ intrinsic rotation order: Yaw -> Pitch -> Roll
    // =======================================
    static inline Vec4 AF_Vec4_EulerToQuaternion(Vec3 euler) {
        AF_FLOAT cy = cosf(euler.y * 0.5f);  // Yaw (Y-axis)
        AF_FLOAT sy = sinf(euler.y * 0.5f);
        AF_FLOAT cp = cosf(euler.x * 0.5f);  // Pitch (X-axis)
        AF_FLOAT sp = sinf(euler.x * 0.5f);
        AF_FLOAT cr = cosf(euler.z * 0.5f);  // Roll (Z-axis)
        AF_FLOAT sr = sinf(euler.z * 0.5f);

        Vec4 q;
        q.w = cy * cp * cr + sy * sp * sr;
        q.x = cy * sp * cr + sy * cp * sr;
        q.y = sy * cp * cr - cy * sp * sr;
        q.z = cy * cp * sr - sy * sp * cr;
        return q;
    }

    // =======================================
    // AF_Vec4_QuaternionToEuler
    // Convert quaternion to Euler angles (in radians)
    // Convention: euler.x=pitch (X-axis), euler.y=yaw (Y-axis), euler.z=roll (Z-axis)
    // Using YXZ intrinsic rotation order: Yaw -> Pitch -> Roll
    // Extraction formulas derived from rotation matrix M = Ry * Rx * Rz
    // =======================================
    static inline Vec3 AF_Vec4_QuaternionToEuler(Vec4 q) {
        Vec3 euler = Vec3_ZERO();
    
        // Convert quaternion to rotation matrix elements we need
        // For YXZ order: M[2][1] = -sin(pitch), M[2][0] = sin(yaw)*cos(pitch), M[2][2] = cos(yaw)*cos(pitch)
        AF_FLOAT m21 = 2.0f * (q.y * q.z - q.w * q.x);  // -sin(pitch)
        AF_FLOAT m20 = 2.0f * (q.x * q.z - q.w * q.y);  // sin(yaw)*cos(pitch) 
        AF_FLOAT m22 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);  // cos(yaw)*cos(pitch)
        AF_FLOAT m01 = 2.0f * (q.x * q.y + q.w * q.z);  // cos(pitch)*sin(roll)
        AF_FLOAT m11 = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);  // cos(pitch)*cos(roll)
        
        // Pitch (X-axis rotation) - can have gimbal lock at ±90 degrees
        AF_FLOAT sinp = -m21;
        if (fabsf(sinp) >= 1.0f) {
            euler.x = copysignf(AF_PI / 2.0f, sinp); // Gimbal lock at ±90 degrees
        } else {
            euler.x = asinf(sinp);
        }
        
        // Yaw (Y-axis rotation)
        euler.y = atan2f(m20, m22);
        
        // Roll (Z-axis rotation)
        euler.z = atan2f(m01, m11);
        
        return euler;
    }

    // =======================================
    // AF_Vec4_Quat_MULT
    // Quaternion multiplication (Hamilton product)
    // =======================================
    static inline Vec4 AF_Vec4_Quat_MULT(Vec4 q1, Vec4 q2) {
        Vec4 result;
        result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
        result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
        result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
        result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
        return result;
    }

    // =======================================
    // createQuaternionFromAngularVelocity
    // Create a quaternion representing rotation from angular velocity vector over time delta
    // =======================================
    static inline Vec4 AF_Vec4_CreateQuaternionFromAngularVelocity(Vec3 angVel, AF_FLOAT dt) {
        AF_FLOAT halfDt = dt * 0.5f;
        AF_FLOAT angleMagnitude = Vec3_MAGNITUDE(angVel);
        
        if (angleMagnitude < 0.0001f) {
            return Vec4_ZERO();
        }
        
        AF_FLOAT halfAngle = angleMagnitude * halfDt;
        AF_FLOAT w = cosf(halfAngle);
        AF_FLOAT sinHalfAngle = sinf(halfAngle);

        AF_FLOAT scale = sinHalfAngle / angleMagnitude;
        Vec3 vectorPart = Vec3_MULT_SCALAR(angVel, scale);

        Vec4 q = { vectorPart.x, vectorPart.y, vectorPart.z, w };
        return q;
    }

#ifdef __cplusplus
}
#endif
#endif /* VEC4_H */
