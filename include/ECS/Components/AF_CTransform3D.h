/*
===============================================================================
AF_CTransform3D_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CTRANSFORM3D_H
#define AF_CTRANSFORM3D_H
#include "AF_Vec3.h"
#include "AF_Vec4.h"
#include "AF_Lib_Define.h"
#include "AF_Component.h"
#include "AF_Mat4.h"

#ifdef __cplusplus
extern "C" {
#endif




/*
====================
AF_CTransform3D
basic struct for the transform component
====================
*/
// Size is 64 bytes
typedef struct {
    PACKED_CHAR enabled;
    //af_bool_t has;// = AF_FALSE;
    //af_bool_t enabled;// = AF_FALSE;
    Vec3 pos;// = {0.0f, 0.0f, 0.0f};
    Vec3 localPos;
    Vec3 rot;// = {0.0f, 0.0f, 0.0f};
    Vec3 localRot;
    Vec3 scale;// = {1.0f, 1.0f, 1.0f};
    Vec3 localScale;
    Vec4 orientation;   // rotation represented as a quaternion
    Mat4 modelMat;
} AF_CTransform3D;

AF_CTransform3D AF_CTransform3D_ZERO(void);
AF_CTransform3D AF_CTransform3D_ADD(void);

#ifdef __cplusplus
}
#endif

#endif  // AF_TRANSFORM3D_H
