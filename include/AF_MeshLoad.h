/*
===============================================================================
AF_MESHLOAD_H

Definition of Mesh Load functions that will be implemented using different mesh loading libraries like assimp
===============================================================================
*/
#ifndef AF_MESHLOAD_H
#define AF_MESHLOAD_H

#include "ECS/Components/AF_CMesh.h"
#include "AF_Assets.h"

// Publically accessible function to trigger further mesh loading implemented
BOOL AF_MeshLoad_Load(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* path);



#endif
