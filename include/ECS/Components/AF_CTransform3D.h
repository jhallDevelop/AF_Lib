/*
===============================================================================
AF_CTransform3D_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CTRANSFORM3D_H
#define AF_CTRANSFORM3D_H
#include "AF_Math/AF_Vec3.h"
#include "AF_Math/AF_Vec4.h"
#include "AF_Lib_Define.h"
#include "AF_Component.h"
#include "AF_Math/AF_Mat4.h"

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
    Vec3 pos;
    Vec4 rot;       // Represented as Quaternion (x, y, z, w)
    Vec3 scale;
    Mat4 modelMat;
} AF_CTransform3D;

AF_CTransform3D AF_CTransform3D_ZERO(void);
AF_CTransform3D AF_CTransform3D_ADD(void);

#ifdef __cplusplus
}
#endif

#endif  // AF_TRANSFORM3D_H
