/*
====================
MAT4_H
Author Name: jhalldevelop
Description: This header file contains functions for performing operations 
on 4x4 vectors, including addition, subtraction, multiplication, 
division, normalization, and distance calculations.
====================
*/
#ifndef MAT4_H
#define MAT4_H
#include "AF_Math/AF_Math.h"
#include "AF_Math/AF_Vec3.h"
#include "AF_Math/AF_Vec4.h"
#ifdef __cplusplus
extern "C" {
#endif

    /*
    ====================
    Mat4 Struct
    Defines a 3D vector with 4xVec4 components.
    ====================
    */
    typedef struct {
        Vec4 rows[4];
    } Mat4;
   
   /*
    ====================
    Mat4_IDENTITY
    Create an identity matrix.
    ====================
    */
    static inline Mat4 Mat4_IDENTITY(void){
        Mat4 returnMatrix = {{
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}            // Row order position
        }};
        return returnMatrix;
    }

      /*
    ====================
    Mat4_IDENTITY
    Create a matrix filled with 1
    ====================
    */
    static inline Mat4 Mat4_ONE(void){
        Mat4 returnMatrix = {{
            {1, 1, 1, 1},
            {1, 1, 1, 1},
            {1, 1, 1, 1},
            {1, 1, 1, 1}            // Row order position
        }};
        return returnMatrix;
    }

    // TODO: move this to AF_Math.h
    // Returns -1.0f for negative numbers, 1.0f for positive numbers, and 0.0f for zero.
    static inline float AF_SIGN(AF_FLOAT _val)
    {
        if (_val > 0.0f) return 1.0f;
        if (_val < 0.0f) return -1.0f;
        return 0.0f;
    }

    

    /*
    ====================
    Mat4_ZERO
    Create a zero matrix.
    ====================
    */
    static inline Mat4 Mat4_ZERO(void){
        Mat4 returnMatrix = {{
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}            // Row order position
        }};
        return returnMatrix;
    }

   

    /*
    ====================
    Mat4_ADD_M4
    Add two 4x4 matrices.
    ====================
    */
    static inline Mat4 Mat4_ADD_M4(Mat4 _leftM4, Mat4 _rightM4)
    {
        Mat4 result = Mat4_ZERO();

        result.rows[0] = Vec4_ADD(_leftM4.rows[0], _rightM4.rows[0]);
        result.rows[1] = Vec4_ADD(_leftM4.rows[1], _rightM4.rows[1]);
        result.rows[2] = Vec4_ADD(_leftM4.rows[2], _rightM4.rows[2]);
        result.rows[3] = Vec4_ADD(_leftM4.rows[3], _rightM4.rows[3]);
        return result;  
    }

    /*
    ====================
    Mat4_MINUS_M4
    Subtract one 4x4 matrix from another.
    ====================
    */
    static inline Mat4 Mat4_MINUS_M4(Mat4 _leftM4, Mat4 _rightM4)
    {
        Mat4 result = Mat4_ZERO();

        result.rows[0] = Vec4_MINUS(_leftM4.rows[0], _rightM4.rows[0]);
        result.rows[1] = Vec4_MINUS(_leftM4.rows[1], _rightM4.rows[1]);
        result.rows[2] = Vec4_MINUS(_leftM4.rows[2], _rightM4.rows[2]);
        result.rows[3] = Vec4_MINUS(_leftM4.rows[3], _rightM4.rows[3]);
        return result;  
    }

    /*
    ====================
    Mat4_MULT_SCALAR
    Multiply a 4x4 matrix by a scalar.
    ====================
    */
    static inline Mat4 Mat4_MULT_SCALAR (Mat4 _matrix, AF_FLOAT _f)
    {
        Mat4 result = Mat4_ZERO();

        result.rows[0] = Vec4_MULT_SCALAR(_matrix.rows[0], _f);
        result.rows[1] = Vec4_MULT_SCALAR(_matrix.rows[1], _f);
        result.rows[2] = Vec4_MULT_SCALAR(_matrix.rows[2], _f);
        result.rows[3] = Vec4_MULT_SCALAR(_matrix.rows[3], _f);
        return result;  
    }

    /*
    ====================
    Mat4_MULT_M4
    Multiply two 4x4 matrices.
    ====================
    */
    static inline Mat4 Mat4_MULT_M4(Mat4 _leftM4, Mat4 _rightM4) {
        Mat4 result = Mat4_ZERO(); // Zero-initialize the result matrix

        // This implementation calculates Result[i][j] = dot(Left.row[i], Right.column[j])
        // where Right.column[j] is assembled from the j-th component of each row vector.
        for (int i = 0; i < 4; ++i) {
            // Calculate the result for row `i`
            float x = _leftM4.rows[i].x * _rightM4.rows[0].x +
                _leftM4.rows[i].y * _rightM4.rows[1].x +
                _leftM4.rows[i].z * _rightM4.rows[2].x +
                _leftM4.rows[i].w * _rightM4.rows[3].x;

            float y = _leftM4.rows[i].x * _rightM4.rows[0].y +
                _leftM4.rows[i].y * _rightM4.rows[1].y +
                _leftM4.rows[i].z * _rightM4.rows[2].y +
                _leftM4.rows[i].w * _rightM4.rows[3].y;

            float z = _leftM4.rows[i].x * _rightM4.rows[0].z +
                _leftM4.rows[i].y * _rightM4.rows[1].z +
                _leftM4.rows[i].z * _rightM4.rows[2].z +
                _leftM4.rows[i].w * _rightM4.rows[3].z;

            float w = _leftM4.rows[i].x * _rightM4.rows[0].w +
                _leftM4.rows[i].y * _rightM4.rows[1].w +
                _leftM4.rows[i].z * _rightM4.rows[2].w +
                _leftM4.rows[i].w * _rightM4.rows[3].w;

            result.rows[i].x = x;
            result.rows[i].y = y;
            result.rows[i].z = z;
            result.rows[i].w = w;
        }

        return result;
    }


    /*
    ====================
    Mat4_DOT_M4
    Calculate the dot product of two 4x4 matrices.
    ====================
    */
    static inline Mat4 Mat4_DOT_M4(Mat4 left, Mat4 right) {
        Mat4 result = Mat4_ZERO();

        for (int i = 0; i < 4; ++i) {
            result.rows[i].x = left.rows[i].x * right.rows[0].x +
                               left.rows[i].y * right.rows[1].x +
                               left.rows[i].z * right.rows[2].x +
                               left.rows[i].w * right.rows[3].x;

            result.rows[i].y = left.rows[i].x * right.rows[0].y +
                               left.rows[i].y * right.rows[1].y +
                               left.rows[i].z * right.rows[2].y +
                               left.rows[i].w * right.rows[3].y;

            result.rows[i].z = left.rows[i].x * right.rows[0].z +
                               left.rows[i].y * right.rows[1].z +
                               left.rows[i].z * right.rows[2].z +
                               left.rows[i].w * right.rows[3].z;

            result.rows[i].w = left.rows[i].x * right.rows[0].w +
                               left.rows[i].y * right.rows[1].w +
                               left.rows[i].z * right.rows[2].w +
                               left.rows[i].w * right.rows[3].w;
        }

        return result;
    }

    /*
    ====================
    Mat4_DIV_SCALAR
    Divide a 4x4 matrix by a scalar.
    ====================
    */
    static inline Mat4 Mat4_DIV_SCALAR(Mat4 _v, AF_FLOAT _f)    
    {
        Mat4 result = Mat4_ZERO();

        result.rows[0] = Vec4_DIV_SCALAR(_v.rows[0], _f);
        result.rows[1] = Vec4_DIV_SCALAR(_v.rows[1], _f);
        result.rows[2] = Vec4_DIV_SCALAR(_v.rows[2], _f);
        result.rows[3] = Vec4_DIV_SCALAR(_v.rows[3], _f);
        return result;
    }

    /*
    ====================
    Mat4_DIV_M4
    Divide one 4x4 matrix by another.
    ====================
    */
    static inline Mat4 Mat4_DIV_M4(Mat4 _leftM4, Mat4 _rightM4)
    {
        Mat4 result = Mat4_ZERO();

        result.rows[0] = Vec4_DIV(_leftM4.rows[0], _rightM4.rows[0]);
        result.rows[1] = Vec4_DIV(_leftM4.rows[1], _rightM4.rows[1]);
        result.rows[2] = Vec4_DIV(_leftM4.rows[2], _rightM4.rows[2]);
        result.rows[3] = Vec4_DIV(_leftM4.rows[3], _rightM4.rows[3]);
        return result;
    }

    /*
    ====================
    Mat4_NORMALIZE
    Normalize a vector.
    If the magnitude of the vector is zero, the vector is not modified.
    ====================
    */
    static inline Vec4 Mat4_NORMALIZE(Vec4 v)
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
    Mat4_MAGNITUDE
    Calculate the magnitude of a vector.
    ====================
    */
    static inline AF_FLOAT Mat4_MAGNITUDE(Vec4 v)
    {
        AF_FLOAT magnitude = 0;
        magnitude += v.x * v.x;
        magnitude += v.y * v.y;
        magnitude += v.z * v.z;
        magnitude += v.w * v.w;
        return AF_Math_Sqrt(magnitude);
    }

    /*
    ====================
    Mat4_DISTANCE
    Calculate the distance between two vectors.
    ====================
    */
    static inline AF_FLOAT Mat4_DISTANCE(Vec4 v1, Vec4 v2)
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
    Mat4_SCALE_V4
    Scale a 4x4 matrix by a vector.
    This function scales each row of the matrix by the corresponding component of the vector.
    ====================
    */
    static inline Mat4 Mat4_SCALE_V4(Mat4 _matrix, Vec4 _scale)
    {
        Mat4 result = Mat4_ZERO();

        for (int i = 0; i < 4; ++i) {
            result.rows[i].x = _matrix.rows[i].x * _scale.x;
            result.rows[i].y = _matrix.rows[i].y * _scale.y;
            result.rows[i].z = _matrix.rows[i].z * _scale.z;
            result.rows[i].w = _matrix.rows[i].w * _scale.w;
        }

        return result;
    }

    /*
    ====================
    Mat4_ROTATE_V4
    Rotate a 4x4 matrix around a specified axis by a given angle (in radians).
    This function uses the axis-angle representation for rotation.
    ====================
    */
    static inline Mat4 Mat4_ROTATE_EulerV4(Mat4 _matrix, Vec4 _axis, AF_FLOAT _angle)
    {
        // Normalize the rotation axis
        Vec4 normalizedAxis = Mat4_NORMALIZE(_axis);
        AF_FLOAT cosAngle = AF_Math_Cos(_angle);
        AF_FLOAT sinAngle = AF_Math_Sin(_angle);
        AF_FLOAT oneMinusCos = 1.0f - cosAngle;

        // Create the rotation matrix
        Mat4 rotationMatrix = {{
            { cosAngle + normalizedAxis.x * normalizedAxis.x * oneMinusCos,
              normalizedAxis.x * normalizedAxis.y * oneMinusCos - normalizedAxis.z * sinAngle,
              normalizedAxis.x * normalizedAxis.z * oneMinusCos + normalizedAxis.y * sinAngle,
              0 },
            { normalizedAxis.y * normalizedAxis.x * oneMinusCos + normalizedAxis.z * sinAngle,
              cosAngle + normalizedAxis.y * normalizedAxis.y * oneMinusCos,
              normalizedAxis.y * normalizedAxis.z * oneMinusCos - normalizedAxis.x * sinAngle,
              0 },
            { normalizedAxis.z * normalizedAxis.x * oneMinusCos - normalizedAxis.y * sinAngle,
              normalizedAxis.z * normalizedAxis.y * oneMinusCos + normalizedAxis.x * sinAngle,
              cosAngle + normalizedAxis.z * normalizedAxis.z * oneMinusCos,
              0 },
            { 0, 0, 0, 1 } // Homogeneous coordinate row
        }};

        // Multiply the input matrix by the rotation matrix
        return Mat4_MULT_M4(_matrix, rotationMatrix);
    }

    /*
    ====================
    Mat4_TRANSFORM_V4
    Transform a 4D vector by a 4x4 matrix.
    This function applies the transformation defined by the matrix to the vector.
    ====================
    */
    static inline Vec4 Mat4_TRANSFORM_V4(Mat4 _matrix, Vec4 _vector)
    {
        Vec4 result;
        result.x = _matrix.rows[0].x * _vector.x + _matrix.rows[0].y * _vector.y + _matrix.rows[0].z * _vector.z + _matrix.rows[0].w * _vector.w;
        result.y = _matrix.rows[1].x * _vector.x + _matrix.rows[1].y * _vector.y + _matrix.rows[1].z * _vector.z + _matrix.rows[1].w * _vector.w;
        result.z = _matrix.rows[2].x * _vector.x + _matrix.rows[2].y * _vector.y + _matrix.rows[2].z * _vector.z + _matrix.rows[2].w * _vector.w;
        result.w = _matrix.rows[3].x * _vector.x + _matrix.rows[3].y * _vector.y + _matrix.rows[3].z * _vector.z + _matrix.rows[3].w * _vector.w;
        return result;
    }

    // ====================
    // AF_Mat4_QuaternionToMat4
    // Convert a quaternion to a 4x4 rotation matrix.
    // https://gabormakesgames.com/blog_quats_to_matrix.html
    // using the Shoemake method for conversion
    // ====================
    static inline Mat4 AF_Mat4_QuaternionToMat4(Vec4 q){
        AF_FLOAT ww = q.w * q.w;
        AF_FLOAT xx = q.x * q.x;
        AF_FLOAT yy = q.y * q.y;
        AF_FLOAT zz = q.z * q.z;

        AF_FLOAT wx = q.w * q.x;
        AF_FLOAT wy = q.w * q.y;
        AF_FLOAT wz = q.w * q.z;

        AF_FLOAT xy = q.x * q.y;
        AF_FLOAT xz = q.x * q.z;

        AF_FLOAT yz = q.y * q.z;

        Mat4 result = {{
            {ww + xx - yy - zz, 2.0f * xy - 2.0f * wz, 2.0f * xz + 2.0f * wy, 0},
            {2.0f * xy + 2.0f * wz, ww - xx + yy - zz, 2.0f * yz - 2.0f * wx, 0},
            {2.0f * xz - 2.0f * wy, 2.0f * yz + 2.0f * wx, ww -xx - yy + zz, 0},
            {0, 0, 0, 1.0f}
        }};
        return result;
    }

    
    // =========================
    // AF_Math_Lookat
    // Creates a view matrix that transforms coordinates from world space to view space.
    // =========================
    static inline Mat4 Mat4_Lookat(Vec3 _viewPosition, Vec3 _targetPosition, Vec3 _up){
        Vec3 forward = Vec3_NORMALIZE(Vec3_MINUS(_targetPosition, _viewPosition));
        Vec3 right = Vec3_NORMALIZE(Vec3_CROSS(forward, _up));
        Vec3 up = Vec3_CROSS(forward, right);

        // Row Major variant
        
        Mat4 returnMatrix = {{
            {right.x,       right.y,    right.z,    -Vec3_DOT(right, _viewPosition)},
            {up.x,          up.y,       up.z,       -Vec3_DOT(up, _viewPosition)},
            //{-forward.x,    -forward.y, -forward.z,  Vec3_DOT(forward, _viewPosition)},
            {-forward.x,    -forward.y, -forward.z,  Vec3_DOT(forward, _viewPosition)},
            {0,             0,          0,           1}
        }};

        return returnMatrix;
    }

/*
====================
Mat4_ToMat4
Create a model matrix from a position, rotation and scale
====================
*/
static inline Mat4 Mat4_ToModelMat4(Vec3 _pos, Vec4 _rot, Vec3 _scale) {

    /**/
    // Identity matrix (column-major initialization)
    Mat4 returnMatrix = {{
        {1, 0, 0, 0},  // Column 0: X-axis
        {0, 1, 0, 0},  // Column 1: Y-axis
        {0, 0, 1, 0},  // Column 2: Z-axis
        {0, 0, 0, 1}   // Column 3: Translation
    }};
    
    // Translation (set in the last column)
    returnMatrix.rows[0].w = _pos.x;
    returnMatrix.rows[1].w = _pos.y;
    returnMatrix.rows[2].w = _pos.z;

    // Rotation (ZYX order)
   Mat4 rotation = AF_Mat4_QuaternionToMat4(_rot);

    // Scale (diagonal matrix, applied first)
    Mat4 scaling = {{
        {_scale.x, 0, 0, 0}, // Column 0
        {0, _scale.y, 0, 0}, // Column 1
        {0, 0, _scale.z, 0}, // Column 2
        {0, 0, 0, 1}         // Column 3
    }};

    // Combine transformations: Translation * Rotation * Scale
    returnMatrix = Mat4_MULT_M4(Mat4_MULT_M4(returnMatrix, rotation), scaling);

    return returnMatrix;
}

/*
static inline Mat4 Mat4_ToModelMat4(Vec3 _pos, Vec3 _rot, Vec3 _scale){
    // Identity matrix
    Mat4 returnMatrix = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    
    // Translation
    returnMatrix.rows[0].w = _pos.x;
    returnMatrix.rows[1].w = _pos.y;
    returnMatrix.rows[2].w = _pos.z;

    // Rotation (ZYX order)
    float cx = cosf(_rot.x), sx = sinf(_rot.x);
    float cy = cosf(_rot.y), sy = sinf(_rot.y);
    float cz = cosf(_rot.z), sz = sinf(_rot.z);

    Mat4 rotationX = {{
        {1, 0, 0, 0},
        {0, cx, -sx, 0},
        {0, sx, cx, 0},
        {0, 0, 0, 1}
    }};

    Mat4 rotationY = {{
        {cy, 0, sy, 0},
        {0, 1, 0, 0},
        {-sy, 0, cy, 0},
        {0, 0, 0, 1}
    }};

    Mat4 rotationZ = {{
        {cz, -sz, 0, 0},
        {sz, cz, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    Mat4 rotation = Mat4_MULT_M4(Mat4_MULT_M4(rotationZ, rotationY), rotationX);

    // Scale
    Mat4 scaling = {{
        {_scale.x, 0, 0, 0},
        {0, _scale.y, 0, 0},
        {0, 0, _scale.z, 0},
        {0, 0, 0, 1}
    }};

    // Combine: Translation * Rotation * Scale
    returnMatrix = Mat4_MULT_M4(Mat4_MULT_M4(scaling, rotation), returnMatrix);

    return returnMatrix;
}

*/

/*
====================
Mat4_Transpose
Transpose a matrix
====================
*/
static inline Mat4 Mat4_Transpose(const Mat4* mat) {
    Mat4 transposed;
    transposed.rows[0].x = mat->rows[0].x;
    transposed.rows[0].y = mat->rows[1].x;
    transposed.rows[0].z = mat->rows[2].x;
    transposed.rows[0].w = mat->rows[3].x;

    transposed.rows[1].x = mat->rows[0].y;
    transposed.rows[1].y = mat->rows[1].y;
    transposed.rows[1].z = mat->rows[2].y;
    transposed.rows[1].w = mat->rows[3].y;

    transposed.rows[2].x = mat->rows[0].z;
    transposed.rows[2].y = mat->rows[1].z;
    transposed.rows[2].z = mat->rows[2].z;
    transposed.rows[2].w = mat->rows[3].z;

    transposed.rows[3].x = mat->rows[0].w;
    transposed.rows[3].y = mat->rows[1].w;
    transposed.rows[3].z = mat->rows[2].w;
    transposed.rows[3].w = mat->rows[3].w;

    return transposed;
}

/*
====================
Mat4_Inverse
Inverse a matrix
====================
*/
static inline Mat4 Mat4_Inverse(const Mat4* mat) {
    Mat4 inv;
    float det;

    // This is the adjugate matrix, which is the transpose of the cofactor matrix.
    // The calculation is done directly using the input matrix's structure.
    inv.rows[0].x =  mat->rows[1].y * mat->rows[2].z * mat->rows[3].w - mat->rows[1].y * mat->rows[2].w * mat->rows[3].z - mat->rows[2].y * mat->rows[1].z * mat->rows[3].w + mat->rows[2].y * mat->rows[1].w * mat->rows[3].z + mat->rows[3].y * mat->rows[1].z * mat->rows[2].w - mat->rows[3].y * mat->rows[1].w * mat->rows[2].z;
    inv.rows[1].x = -mat->rows[1].x * mat->rows[2].z * mat->rows[3].w + mat->rows[1].x * mat->rows[2].w * mat->rows[3].z + mat->rows[2].x * mat->rows[1].z * mat->rows[3].w - mat->rows[2].x * mat->rows[1].w * mat->rows[3].z - mat->rows[3].x * mat->rows[1].z * mat->rows[2].w + mat->rows[3].x * mat->rows[1].w * mat->rows[2].z;
    inv.rows[2].x =  mat->rows[1].x * mat->rows[2].y * mat->rows[3].w - mat->rows[1].x * mat->rows[2].w * mat->rows[3].y - mat->rows[2].x * mat->rows[1].y * mat->rows[3].w + mat->rows[2].x * mat->rows[1].w * mat->rows[3].y + mat->rows[3].x * mat->rows[1].y * mat->rows[2].w - mat->rows[3].x * mat->rows[1].w * mat->rows[2].y;
    inv.rows[3].x = -mat->rows[1].x * mat->rows[2].y * mat->rows[3].z + mat->rows[1].x * mat->rows[2].z * mat->rows[3].y + mat->rows[2].x * mat->rows[1].y * mat->rows[3].z - mat->rows[2].x * mat->rows[1].z * mat->rows[3].y - mat->rows[3].x * mat->rows[1].y * mat->rows[2].z + mat->rows[3].x * mat->rows[1].z * mat->rows[2].y;

    inv.rows[0].y = -mat->rows[0].y * mat->rows[2].z * mat->rows[3].w + mat->rows[0].y * mat->rows[2].w * mat->rows[3].z + mat->rows[2].y * mat->rows[0].z * mat->rows[3].w - mat->rows[2].y * mat->rows[0].w * mat->rows[3].z - mat->rows[3].y * mat->rows[0].z * mat->rows[2].w + mat->rows[3].y * mat->rows[0].w * mat->rows[2].z;
    inv.rows[1].y =  mat->rows[0].x * mat->rows[2].z * mat->rows[3].w - mat->rows[0].x * mat->rows[2].w * mat->rows[3].z - mat->rows[2].x * mat->rows[0].z * mat->rows[3].w + mat->rows[2].x * mat->rows[0].w * mat->rows[3].z + mat->rows[3].x * mat->rows[0].z * mat->rows[2].w - mat->rows[3].x * mat->rows[0].w * mat->rows[2].z;
    inv.rows[2].y = -mat->rows[0].x * mat->rows[2].y * mat->rows[3].w + mat->rows[0].x * mat->rows[2].w * mat->rows[3].y + mat->rows[2].x * mat->rows[0].y * mat->rows[3].w - mat->rows[2].x * mat->rows[0].w * mat->rows[3].y - mat->rows[3].x * mat->rows[0].y * mat->rows[2].w + mat->rows[3].x * mat->rows[0].w * mat->rows[2].y;
    inv.rows[3].y =  mat->rows[0].x * mat->rows[2].y * mat->rows[3].z - mat->rows[0].x * mat->rows[2].z * mat->rows[3].y - mat->rows[2].x * mat->rows[0].y * mat->rows[3].z + mat->rows[2].x * mat->rows[0].z * mat->rows[3].y + mat->rows[3].x * mat->rows[0].y * mat->rows[2].z - mat->rows[3].x * mat->rows[0].z * mat->rows[2].y;

    inv.rows[0].z =  mat->rows[0].y * mat->rows[1].z * mat->rows[3].w - mat->rows[0].y * mat->rows[1].w * mat->rows[3].z - mat->rows[1].y * mat->rows[0].z * mat->rows[3].w + mat->rows[1].y * mat->rows[0].w * mat->rows[3].z + mat->rows[3].y * mat->rows[0].z * mat->rows[1].w - mat->rows[3].y * mat->rows[0].w * mat->rows[1].z;
    inv.rows[1].z = -mat->rows[0].x * mat->rows[1].z * mat->rows[3].w + mat->rows[0].x * mat->rows[1].w * mat->rows[3].z + mat->rows[1].x * mat->rows[0].z * mat->rows[3].w - mat->rows[1].x * mat->rows[0].w * mat->rows[3].z - mat->rows[3].x * mat->rows[0].z * mat->rows[1].w + mat->rows[3].x * mat->rows[0].w * mat->rows[1].z;
    inv.rows[2].z =  mat->rows[0].x * mat->rows[1].y * mat->rows[3].w - mat->rows[0].x * mat->rows[1].w * mat->rows[3].y - mat->rows[1].x * mat->rows[0].y * mat->rows[3].w + mat->rows[1].x * mat->rows[0].w * mat->rows[3].y + mat->rows[3].x * mat->rows[0].y * mat->rows[1].w - mat->rows[3].x * mat->rows[0].w * mat->rows[1].y;
    inv.rows[3].z = -mat->rows[0].x * mat->rows[1].y * mat->rows[3].z + mat->rows[0].x * mat->rows[1].z * mat->rows[3].y + mat->rows[1].x * mat->rows[0].y * mat->rows[3].z - mat->rows[1].x * mat->rows[0].z * mat->rows[3].y - mat->rows[3].x * mat->rows[0].y * mat->rows[1].z + mat->rows[3].x * mat->rows[0].z * mat->rows[1].y;

    inv.rows[0].w = -mat->rows[0].y * mat->rows[1].z * mat->rows[2].w + mat->rows[0].y * mat->rows[1].w * mat->rows[2].z + mat->rows[1].y * mat->rows[0].z * mat->rows[2].w - mat->rows[1].y * mat->rows[0].w * mat->rows[2].z - mat->rows[2].y * mat->rows[0].z * mat->rows[1].w + mat->rows[2].y * mat->rows[0].w * mat->rows[1].z;
    inv.rows[1].w =  mat->rows[0].x * mat->rows[1].z * mat->rows[2].w - mat->rows[0].x * mat->rows[1].w * mat->rows[2].z - mat->rows[1].x * mat->rows[0].z * mat->rows[2].w + mat->rows[1].x * mat->rows[0].w * mat->rows[2].z + mat->rows[2].x * mat->rows[0].z * mat->rows[1].w - mat->rows[2].x * mat->rows[0].w * mat->rows[1].z;
    inv.rows[2].w = -mat->rows[0].x * mat->rows[1].y * mat->rows[2].w + mat->rows[0].x * mat->rows[1].w * mat->rows[2].y + mat->rows[1].x * mat->rows[0].y * mat->rows[2].w - mat->rows[1].x * mat->rows[0].w * mat->rows[2].y - mat->rows[2].x * mat->rows[0].y * mat->rows[1].w + mat->rows[2].x * mat->rows[0].w * mat->rows[1].y;
    inv.rows[3].w =  mat->rows[0].x * mat->rows[1].y * mat->rows[2].z - mat->rows[0].x * mat->rows[1].z * mat->rows[2].y - mat->rows[1].x * mat->rows[0].y * mat->rows[2].z + mat->rows[1].x * mat->rows[0].z * mat->rows[2].y + mat->rows[2].x * mat->rows[0].y * mat->rows[1].z - mat->rows[2].x * mat->rows[0].z * mat->rows[1].y;

    // Calculate the determinant
    det = mat->rows[0].x * inv.rows[0].x + mat->rows[0].y * inv.rows[1].x + mat->rows[0].z * inv.rows[2].x + mat->rows[0].w * inv.rows[3].x;

    // If the determinant is zero, the matrix is not invertible.
    if (det == 0) {
        return Mat4_IDENTITY();
    }

    // Divide the adjugate matrix by the determinant to get the inverse
    det = 1.0f / det;
    
    Mat4 result;
    for (int i = 0; i < 4; i++) {
        result.rows[i] = Vec4_MULT_SCALAR(inv.rows[i], det);
    }

    return result;
}


/*
====================
Mat4_ObliqueProjection
// Takes a projection matrix and a plane defined in CAMERA VIEW SPACE.
// Returns a new, modified oblique projection matrix.
====================
*/
inline static Mat4 Mat4_ObliqueProjection(Mat4 projection, Vec4 clipPlane)
{
    Vec4 q;
    q.x = (AF_SIGN(clipPlane.x) + projection.rows[0].z) / projection.rows[0].x;
    q.y = (AF_SIGN(clipPlane.y) + projection.rows[1].z) / projection.rows[1].y;
    q.z = -1.0f;
    q.w = (1.0f + projection.rows[2].z) / projection.rows[2].w;

    Vec4 c = Vec4_MULT_SCALAR(clipPlane, 2.0f / Vec4_DOT(clipPlane, q));

    Mat4 obliqueProj = projection;
    obliqueProj.rows[2].x = c.x;
    obliqueProj.rows[2].y = c.y;
    obliqueProj.rows[2].z = c.z + 1.0f;
    obliqueProj.rows[2].w = c.w;

    return obliqueProj;
}


/*
====================
Mat4_Mat4_MULT_V4
// Multiplies a 4x4 matrix by a Vec4 (column vector)
// Assumes your Mat4 struct has a 'rows' member like: Vec4 rows[4];
====================
*/
static inline Vec4 Mat4_MULT_V4(Mat4 m, Vec4 v)
{
    Vec4 result;
    result.x = m.rows[0].x * v.x + m.rows[0].y * v.y + m.rows[0].z * v.z + m.rows[0].w * v.w;
    result.y = m.rows[1].x * v.x + m.rows[1].y * v.y + m.rows[1].z * v.z + m.rows[1].w * v.w;
    result.z = m.rows[2].x * v.x + m.rows[2].y * v.y + m.rows[2].z * v.z + m.rows[2].w * v.w;
    result.w = m.rows[3].x * v.x + m.rows[3].y * v.y + m.rows[3].z * v.z + m.rows[3].w * v.w;
    return result;
}


/*
====================
Mat4_GetDirection
// Extracts a direction vector (X=0, Y=1, Z=2) from a model matrix
====================
*/
static inline Vec3 Mat4_GetDirection(Mat4 m, int axis) {
    Vec3 result;
    if (axis == 0) { // X-axis
        result.x = m.rows[0].x;
        result.y = m.rows[1].x;
        result.z = m.rows[2].x;
    } else if (axis == 1) { // Y-axis
        result.x = m.rows[0].y;
        result.y = m.rows[1].y;
        result.z = m.rows[2].y;
    } else if (axis == 2) { // Z-axis
        result.x = m.rows[0].z;
        result.y = m.rows[1].z;
        result.z = m.rows[2].z;
    } else { // Should not happen
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }
    return result;
}

/*
====================
Mat4_GetPosition
// Extracts the position vector from a model matrix
====================
*/
static inline Vec3 Mat4_GetPosition(Mat4 m) {
    Vec3 result;
    result.x = m.rows[0].w;
    result.y = m.rows[1].w;
    result.z = m.rows[2].w;
    return result;
}

/*
====================
Mat4_SetPosition
// Extracts the position vector from a model matrix
// Add this helper function if you don't have one.
// It sets the translation part of a matrix (assumes translation is in the last column).
====================
*/
static inline void Mat4_SetPosition(Mat4* mat, Vec3 pos) {
    mat->rows[0].w = pos.x;
    mat->rows[1].w = pos.y;
    mat->rows[2].w = pos.z;
}

/*
====================
Mat4_GetRotation
// Extracts the rotation vector (Euler angles) from a model matrix
====================
*/
static inline Vec3 Mat4_GetRotation(const Mat4* mat) {
    Vec3 euler;

    // Extract the rotation angles from the matrix
    euler.x = atan2f(mat->rows[2].y, mat->rows[2].z);
    euler.y = atan2f(-mat->rows[2].x, sqrtf(mat->rows[2].y * mat->rows[2].y + mat->rows[2].z * mat->rows[2].z));
    euler.z = atan2f(mat->rows[1].x, mat->rows[0].x);

    return euler;
}


#ifdef __cplusplus
}
#endif
#endif // MAT4_H
