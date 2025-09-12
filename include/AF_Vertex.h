/*
===============================================================================
AF_UTIL_H

Definition of vertex struct

===============================================================================
*/
#ifndef AF_VERTEX_H
#define AF_VERTEX_H
#include "AF_Math/AF_Vec3.h"
#include "AF_Math/AF_Vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_Vertex
vertex struct
====================
*/
typedef struct AF_Vertex {
    Vec3 position;    // 12 bytes
    Vec3 normal;      // 12 bytes
    Vec2 texCoord;    // 8 bytes
    Vec3 tangent;     // 12 bytes
    Vec3 bitangent;   // 12 bytes
 } AF_Vertex;


/*
====================
AF_Vertex_ZERO
Init the vertex
====================
*/
inline static AF_Vertex AF_Vertex_ZERO(void){
    AF_Vertex zeroVertex = {
        {0.0f, 0.0f, 0.0f}, // Position
        {0.0f, 0.0f, 0.0f}, // Normal
        {0.0f, 0.0f}, // TexCoord
        {0.0f, 0.0f, 0.0f}, // Tangent
        {0.0f, 0.0f, 0.0f} // BiTangent
    };
    return zeroVertex;
}

#ifdef __cplusplus
}
#endif

#endif
