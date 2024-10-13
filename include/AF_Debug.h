/* ====================
 AF_DEBUG_H
 Definition file for AF_Debug
 Debug functions for rendering e.g. draw point, line, bounds, or 3d bounds
 ====================
*/
#ifndef AF_DEBUG_H
#define AF_DEBUG_H
#include "AF_Mesh.h"
#include "AF_Mat4.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_Debug_Init_DrawLine
Init the mesh buffers and create the shader for debug line drawing
====================
*/
AF_Mesh  AF_Debug_Init_DrawLine(void);

/*
====================
AF_Debug_DrawPoint
Drwa a pixel point at the given screen space location
Provide an x, and y location as well as a size
====================
*/
void AF_Debug_DrawPoint(float _xPos1, float _yPos1, float _size);

/*
====================
AF_Debug_DrawLine
Drwa a line between two points,
Also provide a line thickness size
====================
*/
void AF_Debug_DrawLine(float _xPos1, float _yPos1, float _xPos2, float _yPos2, float _size, AF_Mesh* _mesh, AF_Mat4 _viewMatrix, AF_Mat4 _projectionMatrix, AF_Mat4 _modelMatrix);

#ifdef __cplusplus
}
#endif
#endif // AF_DEBUG_H
