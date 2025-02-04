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

void AF_MeshLoad_Component(AF_CMesh* _meshComponent, AF_Assets* _assets);
void AF_MeshLoad_LoadModel(AF_Assets* _assets, AF_CMesh* _meshComponent, const char* path);



#endif
