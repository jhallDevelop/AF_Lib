/*
===============================================================================
AF_COLLISIONVOLUME_H definitions

Definition for the collision volume structs
and helper functions.
Types of collision volumes include AABB, OBB, Mesh, Sphere and compound
===============================================================================
*/
#ifndef AF_COLLISIONVOLUME_H
#define AF_COLLISIONVOLUME_H
#include "AF_Lib_Define.h"
#include "AF_Math/AF_Vec3.h"
#include "AF_Math/AF_Vec4.h"

#ifdef __cplusplus
extern "C" {    
#endif

#define AF_COLLIDER_TYPE_COUNT 9

enum AF_CollisionVolumeType_e {
	AF_COLLISION_TYPE_AABB = 1, 
	AF_COLLISION_TYPE_OBB = 2, 
	AF_COLLISION_TYPE_Plane = 3,
	AF_COLLISION_TYPE_Sphere = 4, 
	AF_COLLISION_TYPE_Mesh = 5,
	AF_COLLISION_TYPE_Capsule = 6,
	AF_COLLISION_TYPE_Terrain = 7,
	AF_COLLISION_TYPE_ConvexHull = 8,
	AF_COLLISION_TYPE_Compound = 9,
	AF_COLLISION_TYPE_Invalid = 256	// end of valid types
};

typedef struct { 
    enum AF_CollisionVolumeType_e type;
    const char* name;
} AF_CollisionTypeMap;


static const AF_CollisionTypeMap AF_Collision_TypeMappings[] = {
    {AF_COLLISION_TYPE_AABB, "AABB"},
    {AF_COLLISION_TYPE_OBB, "OBB"},
    {AF_COLLISION_TYPE_Plane, "Plane"},
    {AF_COLLISION_TYPE_Sphere, "Sphere"},
    {AF_COLLISION_TYPE_Mesh, "Mesh"},
    {AF_COLLISION_TYPE_Capsule, "Capsule"},
    {AF_COLLISION_TYPE_Terrain, "Terrain"},
    {AF_COLLISION_TYPE_ConvexHull, "Convex Hull"},
	{AF_COLLISION_TYPE_Compound, "Compound"},
	{AF_COLLISION_TYPE_Invalid, "Invalid"}
};


// Define the key mappings
extern const AF_CollisionTypeMap AF_Collision_TypeMappings[];

/*
====================
Sphere_CollisionVolume Struct used to describe a sphere volume 
====================
*/
typedef struct{ 
	Vec3 bounds;
}Sphere_CollisionVolume;

/*
====================
Box_CollisionVolume Struct used to describe a box volume 
====================
*/
typedef struct{ 
	Vec3 bounds;
}Box_CollisionVolume;

/*
====================
AABB_CollisionVolume Struct used to describe a volume 
====================
*/
typedef struct{ 
	Vec3 bounds;
}AABB_CollisionVolume;

/*
====================
OBB_CollisionVolume Struct used to describe a volume 
====================
*/
typedef struct {
	Vec3 bounds;
}OBB_CollisionVolume;

/*
====================
Mesh_CollisionVolume Struct used to describe a volume 
====================
*/
typedef struct {
	Vec3 bounds;
}Mesh_CollisionVolume;

/*
====================
Compound_CollisionVolume Struct used to describe a volume 
====================
*/
typedef struct {
	Vec4 bounds;
}Compound_CollisionVolume;


/*
================
AF_Texture_Typemappings_ConvertToCharArray
// Construct and return the keys as an array of chars
================
*/
inline static void AF_CollisionVolume_TypeMap_ConvertToCharArray(const AF_CollisionTypeMap* _collisionMappings, const char** _charArray, uint32_t _size) {
    if (!_collisionMappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _collisionMappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}

#ifdef __cplusplus
}
#endif

#endif //AF_COLLISIONVOLUME_H
