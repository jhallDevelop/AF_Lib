/*
===============================================================================
AF_Entity_H defninitions
The entity struct and helper functions
===============================================================================
*/

#ifndef AF_Entity_H
#define AF_Entity_H
#include "AF_Lib_API.h"
#include "AF_Lib_Define.h"
#include "ECS/Components/AF_CSprite.h"
#include "ECS/Components/AF_CCollider.h"
#include "ECS/Components/AF_CText.h"
#include "ECS/Components/AF_CAudioSource.h"
#include "ECS/Components/AF_CPlayerData.h"
#include "ECS/Components/AF_CSkeletalAnimation.h"
#include "ECS/Components/AF_CAI_Behaviour.h"
#include "ECS/Components/AF_CEditorData.h"

// Components
#include "ECS/Components/AF_C3DRigidbody.h"
#include "ECS/Components/AF_CMesh.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_CAnimation.h"
#include "ECS/Components/AF_CCamera.h"
#include "ECS/Components/AF_CInputController.h"
#include "ECS/Components/AF_CScript.h"
#include "ECS/Components/AF_CLight.h"

#define AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY 4

#ifdef __cplusplus
extern "C" {
#endif


/*
====================
AF_Entity
Entity struct
Contains a copy of each component type
All components sizes are known at compile time 
except for AF_Mesh which loads verts and indices from a file
If 2D game then loaded verts are known at compile time as its just a quad hard coded
====================
*/
// Size of struct is exactly 64 bytes
typedef struct AF_Entity {
    flag_t flags;	// Entity has ben enabled
    PACKED_UINT32 id_tag;		// Packed datatype holding both a tag and ID. id of the entity. ID can be 0 to 536, 870, 911, tag holds up to 8 variants    
} AF_Entity;

// Little helper struct that can be use
typedef struct AF_EntityPair {
    AF_Entity* entity1;
    AF_Entity* entity2;
} EntityPair;

// =============== Entity tags ==================
// These are used to identify the type of entity, e.g. Camera, Player, Enemy
// Tags are stored in the lower 3 bits of the packed uint32_t id_tag
#define AF_ENTITY_TAGS_COUNT 12 

typedef enum AF_Entity_Tag_e{
    AF_Entity_Tag_Default = 0, // Custom tag for user defined entities
    AF_Entity_Tag_Camera = 1,
    AF_Entity_Tag_Player = 2,
    AF_Entity_Tag_Enemy = 3, 
    AF_Entity_Tag_Ground = 4,
    AF_Entity_Tag_Environment = 5,
    AF_Entity_Tag_UI = 6,
    AF_Entity_Tag_NPC = 7,
    AF_Entity_Tag_Projectile = 8,
    AF_Entity_Tag_Script = 9, // Used for scripts that are not attached
    AF_Entity_Tag_Sound = 10, // Used for sound entities
    AF_Entity_Tag_Light = 11 // Used for light entities
    
    // Add custom tags
} AF_Entity_Tag_e;

// Define the tag mappings
typedef struct AF_Entity_Tag {
    AF_Entity_Tag_e tag;
    const char* name; // Name of the tag
} AF_Entity_Tag;

// Define the tag mappings
static const AF_Entity_Tag AF_Entity_TagMappings[] = {
    {AF_Entity_Tag_Default, "Default"}, // Default tag
    {AF_Entity_Tag_Camera, "Camera"},
    {AF_Entity_Tag_Player, "Player"},
    {AF_Entity_Tag_Enemy, "Enemy"},
    {AF_Entity_Tag_Ground, "Ground"},
    {AF_Entity_Tag_Environment, "Environment"},
    {AF_Entity_Tag_UI, "UI"},
    {AF_Entity_Tag_NPC, "NPC"},
    {AF_Entity_Tag_Projectile, "Projectile"},
    {AF_Entity_Tag_Script, "Script"},
    {AF_Entity_Tag_Sound, "Sound"},
    {AF_Entity_Tag_Light, "Light"}
};

// Define the tag mappings
extern const AF_Entity_Tag AF_Entity_TagMappings[];


void AF_Entity_Tagmappings_ConvertToCharArray(const AF_Entity_Tag* _tagMappings, const char** _charArray, uint32_t _size);
PACKED_UINT32 AF_ECS_GetTag(PACKED_UINT32 _id_tag);
AF_LIB_API PACKED_UINT32 AF_ECS_GetID(PACKED_UINT32 _id_tag);
PACKED_UINT32 AF_ECS_AssignTag(PACKED_UINT32 _id_tag, PACKED_UINT32 _tagValue);
PACKED_UINT32 AF_ECS_AssignID(PACKED_UINT32 _id_tag, PACKED_UINT32 _idValue);


#ifdef __cplusplus
}
#endif

#endif //AF_Entity_H
