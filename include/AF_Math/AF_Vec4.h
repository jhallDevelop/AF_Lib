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
#include "AF_Math/AF_Math.h"
#include "AF_Math/AF_Vec3.h"
#include "AF_Log.h"
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
    // AF_Vec4_Quat_ADD
    // Quaternion Add
    // =======================================
    static inline Vec4 AF_Vec4_Quat_ADD(Vec4 q1, Vec4 q2) {
        q1.x += q2.x;
        q1.y += q2.y;
        q1.z += q2.z;
        q1.w += q2.w;
        return q1;
    }

    // =======================================
    // AF_Vec4_Quat_Minus
    // Quaternion Minus
    // =======================================
    static inline Vec4 AF_Vec4_Quat_MINUS(Vec4 q1, Vec4 q2) {
        q1.x -= q2.x;
        q1.y -= q2.y;
        q1.z -= q2.z;
        q1.w -= q2.w;
        return q1;
    }

    // =======================================
    // AF_Vec4_Quat_GetAxis
    // Normalise the vector component of the quaternion to get the rotation axis. Returns a Vec3.
    // =======================================
    static inline Vec3 AF_Vec4_Quat_GetAxis(Vec4 q) {
       
        Vec3 axis = {q.x, q.y, q.z};
        return Vec3_NORMALIZE(axis);
    }

    // =======================================
    // AF_Vec4_Quat_GetAngleDegrees
    // Get the angle of rotation in degrees from a quaternion
    // =======================================
    static inline AF_FLOAT AF_Vec4_Quat_GetAngleDegrees(Vec4 q) {
       
        return 2.0f * (acosf(q.w) * AF_180_DIV_PI_d);
    }

    // =======================================
    // AF_Vec4_Quat_GetAngleEuler
    // Get the angle of rotation in Euler radians from a quaternion
    // =======================================
    static inline AF_FLOAT AF_Vec4_Quat_GetAngleEuler(Vec4 q) {
       
        return 2.0f * (acosf(q.w));
    }



    // =======================================
    // AF_Vec4_EulerToQuaternion
    // Convert Euler angles (in radians) to quaternion
    // Convention: euler.x=pitch (X-axis), euler.y=yaw (Y-axis), euler.z=roll (Z-axis)
    // Using YXZ intrinsic rotation order: Yaw -> Pitch -> Roll
    //https://gabormakesgames.com/blog_quats_create.html
    // =======================================
    static inline Vec4 AF_Vec4_EulerToQuaternion(Vec3 _euler) {
        // construct the quaternion for each component
        Vec4 xResult = {0, 0, 0, 0};
        // X-axis rotation (pitch)
        xResult.x = sinf(_euler.x * 0.5f);
        xResult.w = cosf(_euler.x * 0.5f);

        // Y-axis rotation (yaw)
        Vec4 yResult = {0, 0, 0, 0};
        yResult.y = sinf(_euler.y * 0.5f);
        yResult.w = cosf(_euler.y * 0.5f);

        // Z-axis rotation (roll)
        Vec4 zResult = {0, 0, 0, 0};
        zResult.z = sinf(_euler.z * 0.5f);
        zResult.w = cosf(_euler.z * 0.5f);


        // combine the components back into a Vec4
        Vec4 q = AF_Vec4_Quat_MULT(AF_Vec4_Quat_MULT(xResult, yResult), zResult);

        return q;
    }



    // =======================================
    // AF_Vec4_QuaternionToEuler
    // Convert quaternion to Euler angles (in radians)
    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
    // =======================================
    static inline Vec3 AF_Vec4_QuaternionToEuler(Vec4 q) {
        
        Vec3 euler = {0, 0, 0};
        // Up/Down Test for Gimbal Lock
        AF_FLOAT test = (q.x * q.y) + (q.z * q.w);
        // Stright up/down gimbal lock cases
        if(test > 0.499f) { 
            euler.x = 0.0f;
            euler.y = 2.0f * atan2f(q.x, q.w);
            euler.z = AF_PI_DIV_2_d;
            return euler;
        }
        // Stright up/down gimbal lock cases
        if( test < -0.499f) { // South Pole
            euler.x = 0.0f;
            euler.y = -2.0f * atan2f(q.x, q.w);
            euler.z = -AF_PI_DIV_2_d;
            return euler;
        }
        // X-Axis
        euler.x = atan2f(2.0f * ((q.w * q.x) + (q.y * q.z)), 1.0f - 2.0f * ((q.x * q.x) + (q.y * q.y)));

        // Yaw (Y-axis rotation)
        euler.y = atan2f(2.0f * ((q.y * q.w) - (q.x * q.z)), 1.0f - 2.0f * ((q.y * q.y) + (q.z * q.z)));

        // Pitch (X-axis rotation)
        euler.z = asinf(2.0f * ((q.x * q.y) + (q.z * q.w)));
        
        return euler;
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

    // =======================================
    // AF_Vec4_Quat_FromToRotation
    // Create a quaternion that rotates from a rotation towards a given rotation using halfway method
    // https://gabormakesgames.com/blog_quats_create.html
    // =======================================
    static inline Vec4 AF_Vec4_Quat_FromToRotation(Vec3 _from, Vec3 _to){
        Vec3 p0 = Vec3_NORMALIZE(_from);
        Vec3 p1 = Vec3_NORMALIZE(_to);
        Vec4 result = Vec4_ZERO();
        Vec3 upVector = {0, 1, 0};
        Vec3 forwardVector = {0, 0, 1};
        
        // Get the axis
        if(Vec3_DOT(p0, p1) < -0.9999f){
            Vec3 mostOrthoganal = {1, 0, 0};

            if(fabsf(p0.y) < fabsf(p0.x)) {
                mostOrthoganal = upVector;
            }

            if(fabsf(p0.z) < fabsf(p0.y) && fabsf(p0.z) < fabsf(p0.x)){
                mostOrthoganal = forwardVector;
            }

            Vec3 axis = Vec3_NORMALIZE(Vec3_CROSS(p0, mostOrthoganal));
            Vec4 axisQuat = {axis.x, axis.y, axis.z, 0};
            result = axisQuat;
            return Vec4_NORMALIZE(result);
        }

        Vec3 half = Vec3_NORMALIZE(Vec3_ADD(p0, p1));
        Vec3 axis = Vec3_CROSS(p0, half);

        result.x = axis.x;
        result.y = axis.y;
        result.z = axis.z;
        result.w = Vec3_DOT(p0, half);

        return Vec4_NORMALIZE(result);
    }

    // =======================================
    // AF_Vec4_Quat_RotateAxis
    // Rotate a 3D vector using a quaternion. Returns the rotated vector 3
    // Often used to get the forward, up, or right vector of an object based on its rotation quaternion. 
    // e.g. pass in the world forward vector (0, 0, -1) to get the forward direction of an object.
    // =======================================
    static inline Vec3 AF_Vec4_Quat_RotateVec3(Vec4 _quat, Vec3 _vec3){
        // Extract quaternion components
        Vec3 u = {_quat.x, _quat.y, _quat.z};
        AF_FLOAT s = _quat.w;

        // v = 2.0f * dot(u, v) * u + (s*s - dot(u, u))
        Vec3 result = Vec3_ADD(Vec3_ADD(Vec3_MULT_SCALAR(u, 2.0f * Vec3_DOT(u, _vec3)),
        Vec3_MULT_SCALAR(_vec3, s * s - Vec3_DOT(u, u))), 
        Vec3_MULT_SCALAR(Vec3_CROSS(u, _vec3), 
        2.0f * s));
        return result;
    }

    // =======================================
    // AF_Vec4_Quat_LookAt
    // Given a direct, and up, return a new quaternian representing look at.
    // https://gabormakesgames.com/blog_quats_create.html
    // =======================================
    static inline Vec4 AF_Vec4_Quat_LookAt(Vec3 _direction, Vec3 _up){
        // Normalise input data
        _direction = Vec3_NORMALIZE(_direction);
        //AF_Log("Normalized Direction: %.2f, %.2f, %.2f\n", _direction.x, _direction.y, _direction.z);
        _up = Vec3_NORMALIZE(_up);

        // Step 1: find quaternion that rotates from forward to direction
        Vec3 forwardVec = {0, 0, -1};
        Vec4 fromFrorwardtoDirection = AF_Vec4_Quat_FromToRotation(forwardVec, _direction);
        /*
        AF_Log("From Forward To Direction Quaternion: %.2f, %.2f, %.2f, %.2f\n", 
               fromFrorwardtoDirection.x,
               fromFrorwardtoDirection.y,
               fromFrorwardtoDirection.z,
               fromFrorwardtoDirection.w);
               */
        // Step 2: Make sure up is perpendicular to desired direction
        Vec3 right = Vec3_CROSS(_direction, _up);
        // If direction and up are parallel (e.g. looking straight up), this fails (zero vector).
        // Fallback or precision check might be needed here, but for now standard logic:
        if (Vec3_MAGNITUDE_SQ(right) < 0.0001f) {
            Vec3 rightVector = {1, 0, 0};
            right = rightVector; // Fallback right vector
        }
        _up = Vec3_CROSS(right, _direction);
        _up = Vec3_NORMALIZE(_up);
        //AF_Log("Perpendicular Up: %.2f, %.2f, %.2f\n", _up.x, _up.y, _up.z);

        Vec3 worldUp = {0, 1, 0};
        Vec3 objectUp = AF_Vec4_Quat_RotateVec3(fromFrorwardtoDirection, worldUp);//Vec3_MULT(_up, fromFrorwardtoDirectionVec3);


        // Step 4: create quaternion from object up to desired up
        Vec4 fromObjectUpToDesiredUp = AF_Vec4_Quat_FromToRotation(objectUp, _up);
        /*
        AF_Log("From Object Up To Desired Up: %.2f, %.2f, %.2f, %.2f\n", 
           fromObjectUpToDesiredUp.x,
           fromObjectUpToDesiredUp.y,
           fromObjectUpToDesiredUp.z,
           fromObjectUpToDesiredUp.w);
        */
        // Step 5: combine rotations in revese ! forward applied first, then up
       Vec4 result = AF_Vec4_Quat_MULT(fromObjectUpToDesiredUp, fromFrorwardtoDirection);
       /*AF_Log("Result Before Normalize: %.2f, %.2f, %.2f, %.2f\n", 
           result.x, result.y, result.z, result.w);
           */


        return Vec4_NORMALIZE(result);
    }



    

#ifdef __cplusplus
}
#endif
#endif /* VEC4_H */
