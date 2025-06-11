/*
===============================================================================
AF_CLight_H

Header only Mesh loading
functions to load meshes, creating memory on the heap based on the size of the mesh

===============================================================================
*/
#ifndef AF_CLIGHT_H
#define AF_CLIGHT_H
#include "AF_Lib_Define.h"
#include "ECS/Components/AF_Component.h"
#include "AF_Math/AF_Vec3.h"

#ifdef __cplusplus
extern "C" {
#endif  
#define AF_CLIGHT_TYPE_COUNT 3

enum AF_Light_Type_e{
    AF_LIGHT_TYPE_AMBIENT = 0,
	AF_LIGHT_TYPE_SPOT = 1,
    AF_LIGHT_TYPE_POINT = 2,
	
};


// Mesh Struct
typedef struct AF_CLight {
    PACKED_CHAR enabled;
    enum AF_Light_Type_e lightType;
    Vec3 direction;
    Vec3 ambientCol;
    Vec3 diffuseCol;
    Vec3 specularCol;
    AF_FLOAT constant;
    AF_FLOAT linear;
    AF_FLOAT quadratic;
    AF_FLOAT cutOff;
    AF_FLOAT outerCutoff;
} AF_CLight;


AF_CLight AF_CLight_ZERO(void);
AF_CLight AF_CLight_ADD(void);

#ifdef __cplusplus
}
#endif  

#endif  // AF_CLIGHT_H
