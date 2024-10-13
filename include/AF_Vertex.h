/*
===============================================================================
AF_UTIL_H

Definition of vertex struct

===============================================================================
*/
#ifndef AF_VERTEX_H
#define AF_VERTEX_H
#include "AF_Vec3.h"
#include "AF_Vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_Vertex
vertex struct
====================
*/
typedef struct {
    AF_Vec3 position;
    AF_Vec3 normal;
    //AF_Vec3 tangent;
    //AF_Vec3 bitangent;
    AF_Vec2 texCoord;
} AF_Vertex;

#ifdef __cplusplus
}
#endif

#endif
