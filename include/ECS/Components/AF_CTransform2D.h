/*
===============================================================================
AF_CTransform2D_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CTRANSFORM2D_H
#define AF_CTRANSFORM2D_H
#include "AF_Math/AF_Vec2.h"
#include "AF_Lib_Define.h"
#include "AF_Component.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_CTransform2D
basic struct for the transform component
====================
*/
// Size is 64 bytes
typedef struct {
    PACKED_CHAR enabled;
    Vec2 pos;
    Vec2 rot;
    Vec2 scale;
} AF_CTransform2D;


#ifdef __cplusplus
}
#endif

AF_CTransform2D AF_CTransform2D_ZERO(void);
AF_CTransform2D AF_CTransform2D_ADD(void);

#endif  // AF_TRANSFORM2D_H
