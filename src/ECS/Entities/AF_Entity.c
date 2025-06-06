/*
===============================================================================
AF_Entity_H defninitions
The entity struct and helper functions
===============================================================================
*/
#include "ECS/Entities/AF_ECS.h"


PACKED_UINT32 AF_ECS_GetTag(PACKED_UINT32 _id_tag){
	// Unpacking the packed uint32_t
	return _id_tag & 0x7;
}

PACKED_UINT32 AF_ECS_GetID(PACKED_UINT32 _id_tag){
	// unpacking the packed uint32_t
	return (_id_tag >> 3) & 0x1FFFFFFF;
}

PACKED_UINT32 AF_ECS_AssignTag(PACKED_UINT32 _id_tag, PACKED_UINT32 _tagValue){
	// update the tag value in the packed uint32, keeping the id intact
	PACKED_UINT32 returnTag = (_tagValue & 0x7) | ((AF_ECS_GetID(_id_tag) & 0x1FFFFFFF) << 3);
	return returnTag;
}

PACKED_UINT32 AF_ECS_AssignID(PACKED_UINT32 _id_tag, PACKED_UINT32 _idValue){
	// update the id value in the packed uint32, keeping the tag intact
	PACKED_UINT32 returnID = (AF_ECS_GetTag(_idValue) & 0x7) | ((_id_tag & 0x1FFFFFFF) << 3);
	return returnID;
}

