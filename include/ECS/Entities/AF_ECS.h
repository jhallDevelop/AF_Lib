/*
===============================================================================
AF_ECS_H defninitions
Entity Component System used to process entities and manage them
Effectively the scene graph 
New AF_ECS struct objects can be created to hold the entities for each scene.
===============================================================================
*/

#ifndef AF_ECS_H
#define AF_ECS_H
#include "AF_Lib_API.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "AF_Lib_Define.h"
#include "AF_Entity.h"
#include "ECS/Components/AF_Component.h"



#ifdef __cplusplus
extern "C" {
#endif



/*
====================
AF_ECS
Entity Component System struct
Used to hold all the entities
====================
*/
typedef struct {
    uint32_t entitiesCount;
    uint32_t currentEntity;
    AF_Entity entities[AF_ECS_TOTAL_ENTITIES];
	AF_CTransform3D transforms[AF_ECS_TOTAL_ENTITIES];	// 3d transform component
    AF_CSprite sprites[AF_ECS_TOTAL_ENTITIES];		// sprite cmponent
    
    AF_C3DRigidbody rigidbodies[AF_ECS_TOTAL_ENTITIES];	// rigidbody component
	AF_CCollider colliders[AF_ECS_TOTAL_ENTITIES];	// Collider component
	AF_CCamera cameras[AF_ECS_TOTAL_ENTITIES];	// Camera component

    AF_CAnimation animations[AF_ECS_TOTAL_ENTITIES];	// animation Component
    //AF_CMesh meshes[AF_ECS_TOTAL_ENTITIES];		// mesh component 	// TODO: turn this into a component type
	AF_CTerrain terrains[AF_ECS_TOTAL_ENTITIES];		// terrain component
	AF_CText texts[AF_ECS_TOTAL_ENTITIES];
	AF_CAudioSource audioSources[AF_ECS_TOTAL_ENTITIES];
	AF_CPlayerData playerDatas[AF_ECS_TOTAL_ENTITIES];
	AF_CSkeletalAnimation skeletalAnimations[AF_ECS_TOTAL_ENTITIES];
	AF_CAI_Behaviour aiBehaviours[AF_ECS_TOTAL_ENTITIES];
	AF_CEditorData editorData[AF_ECS_TOTAL_ENTITIES];
	AF_CInputController inputControllers[AF_ECS_TOTAL_ENTITIES];
	AF_CScript scripts[AF_ECS_TOTAL_ENTITIES * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY];
	AF_CLight lights[AF_ECS_TOTAL_ENTITIES];

	AF_CMesh_SparseSet meshSparseSet;		// sparse set for mesh component

} AF_ECS;


void AF_ECS_ReSyncComponents(AF_ECS* _ecs);
AF_LIB_API void AF_ECS_Init(AF_ECS* _ecs);
void AF_ECS_DeleteEntity(AF_ECS* _ecs, AF_Entity* _entity);
void AF_ECS_DuplicateEntity(AF_ECS* _ecs, AF_Entity* _entity);
AF_Entity* AF_ECS_CreateEntity(AF_ECS* _ecs);
//void AF_RemoveEntity(Entity _entity);
void AF_ECS_Update(AF_Entity* _entities);
void AF_ECS_LoadECSFromBinary(FILE* _filePtr, AF_ECS* _ecs);
void AF_ECS_SaveECS(FILE* _file, AF_ECS* _ecs);
AF_LIB_API uint32_t AF_ECS_GetCamera(AF_ECS* _ecs);
AF_LIB_API void AF_ECS_PropagateActiveState(AF_ECS* _ecs, uint32_t nodeID, af_bool_t parentIsActive);
AF_LIB_API void AF_ECS_CreateCamera(AF_ECS* _ecs, Vec3 _pos);
AF_LIB_API void AF_ECS_UpdateCameraVectors(AF_ECS* _ecs, uint32_t _cameraID, AF_FLOAT _windowWidth, AF_FLOAT _windowHeight);

AF_LIB_API uint32_t AF_ECS_FindEntityOfTag(AF_ECS* _ecs, AF_Entity_Tag_e _tag);

AF_CMesh* AF_ECS_GetMeshComponent(AF_ECS* _ecs, uint32_t entityID);
AF_CMesh* AF_ECS_AddMeshComponent(AF_ECS* _ecs, uint32_t entityID);

#ifdef __cplusplus
}
#endif


#endif //AF_ECS_H
