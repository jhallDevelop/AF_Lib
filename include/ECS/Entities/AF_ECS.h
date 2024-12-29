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
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "AF_Entity.h"
#include "ECS/Components/AF_Component.h"

#define AF_ECS_TOTAL_ENTITIES 65

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
    // TODO: don't ifdef, be smarter
    AF_Entity entities[AF_ECS_TOTAL_ENTITIES];
    AF_CSprite sprites[AF_ECS_TOTAL_ENTITIES];		// sprite cmponent
    
    AF_CTransform3D transforms[AF_ECS_TOTAL_ENTITIES];	// 3d transform component
    AF_C3DRigidbody rigidbodies[AF_ECS_TOTAL_ENTITIES];	// rigidbody component
	AF_CCollider colliders[AF_ECS_TOTAL_ENTITIES];	// Collider component
	AF_CCamera cameras[AF_ECS_TOTAL_ENTITIES];	// Camera component

    AF_CAnimation animations[AF_ECS_TOTAL_ENTITIES];	// animation Component
    AF_CMesh meshes[AF_ECS_TOTAL_ENTITIES];		// mesh component 	// TODO: turn this into a component type
	AF_CText texts[AF_ECS_TOTAL_ENTITIES];
	AF_CAudioSource audioSources[AF_ECS_TOTAL_ENTITIES];
	AF_CPlayerData playerDatas[AF_ECS_TOTAL_ENTITIES];
	AF_CSkeletalAnimation skeletalAnimations[AF_ECS_TOTAL_ENTITIES];
	AF_CAI_Behaviour aiBehaviours[AF_ECS_TOTAL_ENTITIES];
} AF_ECS;

/*
====================
AF_ECS_ReSyncComponents
Helper function to re-sync pointers loast likely when loading a save
====================
*/
static inline void AF_ECS_ReSyncComponents(AF_ECS* _ecs){
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_Entity* entity = &_ecs->entities[i];
		// original data is all living in fixed arrays per component
		entity->transform = &_ecs->transforms[i];
		entity->sprite = &_ecs->sprites[i];
	
		// Transform Component
		entity->transform = &_ecs->transforms[i];
		
		// Rigidbody3D
		entity->rigidbody = &_ecs->rigidbodies[i];

		// Colliders
		entity->collider = &_ecs->colliders[i];
		
		// Animation component
		entity->animation = &_ecs->animations[i];

		// Add Meshes
		entity->mesh = &_ecs->meshes[i];

		// Add text
		entity->text = & _ecs->texts[i];

		// Add audio
		entity->audioSource = &_ecs->audioSources[i];

		// player data
		entity->playerData = &_ecs->playerDatas[i];

		// skeletal animations
		entity->skeletalAnimation = &_ecs->skeletalAnimations[i];

		// ai Behaviours
		entity->aiBehaviour = &_ecs->aiBehaviours[i];
		
		// Camera Component
		entity->camera = &_ecs->cameras[i];

	}
}

/*
====================
AF_ECS_Init
Init helper function to initialise all the entities.
Entities are all loaded into memory at the start using the compile time define AF_ECS_TOTAL_ENTITIES
====================
*/
static inline void AF_ECS_Init(AF_ECS* _ecs){
	assert(_ecs != NULL && "AF_ECS_Init: argument is null");
	// Initialise all entities in the entity pool with default values
	_ecs->entitiesCount = AF_ECS_TOTAL_ENTITIES;

	
	for(uint32_t i = 0; i < AF_ECS_TOTAL_ENTITIES; i++){
		
		AF_Entity* entity = &_ecs->entities[i];
		flag_t* componentState = &entity->flags;
 		//entity->enabled = TRUE;
		entity->flags = AF_Component_SetEnabled(*componentState, TRUE);
		entity->id_tag = AF_ECS_AssignID(entity->id_tag, i);
		entity->id_tag = AF_ECS_AssignTag(entity->id_tag, 0);
			
		// Sprite Components
		_ecs->sprites[i] = AF_CSprite_ZERO();
	
		// Transform Component
		entity->parentTransform = NULL;
		
		// Transform Component
		_ecs->transforms[i] = AF_CTransform3D_ZERO();
		
		// Rigidbody3D
		_ecs->rigidbodies[i] = AF_C3DRigidbody_ZERO();

		// Colliders
		_ecs->colliders[i] = AF_CCollider_ZERO();
		
		// Animation component
		_ecs->animations[i] = AF_CAnimation_ZERO();

		// Add Meshes
		_ecs->meshes[i] = AF_CMesh_ZERO();

		// Add text
		_ecs->texts[i] = AF_CText_ZERO();

		// Add audio
		_ecs->audioSources[i] = AF_CAudioSource_ZERO();

		// player data
		_ecs->playerDatas[i] = AF_CPlayerData_ZERO();

		// skeletal animations
		_ecs->skeletalAnimations[i] = AF_CSkeletalAnimation_ZERO();

		// ai Behaviours
		_ecs->aiBehaviours[i] = AF_CAI_Behaviour_ZERO();
		
		// Camera Component
		_ecs->cameras[i] = AF_CCamera_ZERO();
		//entity->camera = NULL;//AF_CCamera_ZERO();
	}
	
	AF_ECS_ReSyncComponents(_ecs);
}

/*
====================
AF_ECS_CreateEntity
Helper function to enable the entity and pass on a pointer reference to it
All entities already exist in memory so this just enables it.
====================
*/
static inline AF_Entity* AF_ECS_CreateEntity(AF_ECS* _ecs){
	assert(_ecs != NULL && "AF_ECS_CreateEntity: argument is null");
	assert(_ecs->currentEntity <= _ecs->entitiesCount && "AF_ECS_CreateEntity: ECS: Ran out of entities !!!\n");
	
    // increment the entity count and return the reference to the next available entity
    _ecs->currentEntity++;

    AF_Entity* entity = &_ecs->entities[_ecs->currentEntity];
    entity->id_tag = AF_ECS_AssignID(entity->id_tag, _ecs->currentEntity);
    PACKED_CHAR* componentState = &entity->flags;
	entity->flags = AF_Component_SetHas(*componentState, TRUE);
	entity->flags = AF_Component_SetEnabled(*componentState, TRUE);

    // Give this entity a default transform component that is enabled
    
	
    
    if(entity == NULL){
		printf("AF_ECS: AF_ECS_CreateEntity entity failed, and is returining a null entity\n");
		return NULL;
	}

	if(entity->transform == NULL){
		printf("AF_ECS: AF_ECS_CreateEntity entity->transform failed, and is returining a null entity\n");
		return NULL;
	}

	*entity->transform = AF_CTransform3D_ADD();
    return entity;
	
}




//void AF_RemoveEntity(Entity _entity);
void AF_ECS_Update(AF_Entity* _entities);




/*
====================
AF_ECS_LoadEntities
Helper function to take in a file that hopefully is json format
And re-construct the ECS structure
====================
*/
static inline void AF_ECS_LoadECSFromBinary(FILE* _filePtr, AF_ECS* _ecs){
	// start reading the file contents.
	// does it look like json format
	// Start creating the entity
	// Start enabling/setting the components

	printf("==== Load ECS from binary file ====\n");
    if(_filePtr == NULL){
        printf("AF_File_PrintTextBuffer: FAILED to print buffer. _charBuffer is NULL\n");
        return;
    }
	fread(_ecs, sizeof(AF_ECS), 1, _filePtr); // Read the struct
	// resync the pointers so we don't get null reference
	AF_ECS_ReSyncComponents(_ecs);
    if (ferror(_filePtr)) {
        printf("Error while reading \n");
    }
}



/*
====================
AF_ECS_SaveEntities
Helper function that saves the ECS structure to json format
====================
*/
static inline void AF_ECS_SaveECS(FILE* _file, AF_ECS* _ecs){
	if( _file || _ecs){}
	// start reading the file contents.
	// does it look like json format
	// Start creating the entity
	// Start enabling/setting the components
}



#endif //AF_ECS_H
