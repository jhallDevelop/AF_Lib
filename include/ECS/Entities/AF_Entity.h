/*
===============================================================================
AF_Entity_H defninitions
The entity struct and helper functions
===============================================================================
*/

#ifndef AF_Entity_H
#define AF_Entity_H
//#include "AF_ECS.h"
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
    // TODOD
    AF_CTransform3D* parentTransform;
    AF_CTransform3D* transform;	// 3d transform component
    AF_CSprite* sprite;		// sprite cmponent
    AF_C3DRigidbody* rigidbody; // 3d rigidbody
    AF_CCollider* collider;	// Collider component
    AF_CAnimation* animation;	// animation Component
    AF_CCamera* camera;		// camera component
    AF_CMesh* mesh;		// mesh component 	// TODO: turn this into a component type
    AF_CText* text;
    AF_CAudioSource* audioSource;
    AF_CPlayerData* playerData;
    AF_CSkeletalAnimation* skeletalAnimation;
    AF_CAI_Behaviour* aiBehaviour;
    AF_CEditorData* editorData;
    AF_CInputController* inputController;
    AF_CScript* scripts[AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY];
    AF_CLight* light;
} AF_Entity;

// Little helper struct that can be use
typedef struct AF_EntityPair {
    AF_Entity* entity1;
    AF_Entity* entity2;
} EntityPair;

PACKED_UINT32 AF_ECS_GetTag(PACKED_UINT32 _id_tag);
PACKED_UINT32 AF_ECS_GetID(PACKED_UINT32 _id_tag);
PACKED_UINT32 AF_ECS_AssignTag(PACKED_UINT32 _id_tag, PACKED_UINT32 _tagValue);
PACKED_UINT32 AF_ECS_AssignID(PACKED_UINT32 _id_tag, PACKED_UINT32 _idValue);

#ifdef __cplusplus
}
#endif

#endif //AF_Entity_H
