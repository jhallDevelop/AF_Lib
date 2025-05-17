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
	AF_CEditorData editorData[AF_ECS_TOTAL_ENTITIES];
	AF_CInputController inputControllers[AF_ECS_TOTAL_ENTITIES];
	AF_CScript scripts[AF_ECS_TOTAL_ENTITIES * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY];
	AF_CLight lights[AF_ECS_TOTAL_ENTITIES];
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

		// set the name
		//snprintf(entity->name, sizeof(entity->name), "%s", _ecs->entities[i].name);

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

		// Editor Component
		entity->editorData = &_ecs->editorData[i];

		// Input Controller
		entity->inputController = &_ecs->inputControllers[i];
		// null the func pointers for actions as they will get set correctly up by the editor
		// TODO: ensure the func pointers get updated if in non-editor mode e.g. changing scenes in a game 
		for(uint32_t x = 0; x < entity->inputController->inputActionCount; x++){
			entity->inputController->inputActions[x].actionFuncPtr = NULL;
		}

		// Scripts
        for(uint32_t z = 0; z < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; z++){
            // Use 'i' (the current entity index) and 'z' (the script index for this entity)
            entity->scripts[z] = &_ecs->scripts[(i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + z];
        }
		
		// Lights
		entity->light = &_ecs->lights[i];
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
		
		// set the name
		//snprintf(entity->name, sizeof(entity->name), "Entity: %u", i);

		// ===== Init all the component pointers =====
		// Transform Component
		entity->parentTransform = NULL;

		// original data is all living in fixed arrays per component
		entity->transform = NULL;
		entity->sprite = NULL;
	
		// Transform Component
		entity->transform = NULL;
		
		// Rigidbody3D
		entity->rigidbody = NULL;

		// Colliders
		entity->collider = NULL;
		
		// Animation component
		entity->animation = NULL;

		// Add Meshes
		entity->mesh = NULL;

		// Add text
		entity->text = NULL;

		// Add audio
		entity->audioSource = NULL;

		// player data
		entity->playerData = NULL;

		// skeletal animations
		entity->skeletalAnimation = NULL;

		// ai Behaviours
		entity->aiBehaviour = NULL;
		
		// Camera Component
		entity->camera = NULL;

		// Editor Component
		entity->editorData = NULL;
		
		// Input controller
		entity->inputController = NULL;

		// ===== Init all the component arrays =====
		// Sprite Components
		_ecs->sprites[i] = AF_CSprite_ZERO();
		
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

		// Editor Data
		_ecs->editorData[i] = AF_CEditorData_ZERO();

		// Input Controller
		_ecs->inputControllers[i] = AF_CInputController_ZERO();

		// Scripts
		for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
			_ecs->scripts[(i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + j] = AF_CScript_ZERO();
		}

		// Lights
		_ecs->lights[i] = AF_CLight_ZERO();
	}
	
	AF_ECS_ReSyncComponents(_ecs);
}

static inline void AF_ECS_DeleteEntity(AF_ECS* _ecs, AF_Entity* _entity){
	AF_Log("AF_ECS_DeleteEntity\n");
	//entity->enabled = TRUE;
	_entity->flags = AF_Component_SetEnabled(_entity->flags, FALSE);
	_entity->flags = AF_Component_SetHas(_entity->flags, FALSE);
	
	// set the name
	//snprintf(entity->name, sizeof(entity->name), "Entity: %u", i);

	// ===== Init all the component pointers =====
	// Transform Component
	_entity->parentTransform = NULL;

	// original data is all living in fixed arrays per component
	*_entity->transform = AF_CTransform3D_ZERO();
	*_entity->sprite = AF_CSprite_ZERO();
	
	// Rigidbody3D
	*_entity->rigidbody = AF_C3DRigidbody_ZERO();

	// Colliders
	*_entity->collider = AF_CCollider_ZERO();
	
	// Animation component
	*_entity->animation = AF_CAnimation_ZERO();

	// Add Meshes
	*_entity->mesh = AF_CMesh_ZERO();

	// Add text
	*_entity->text = AF_CText_ZERO();

	// Add audio
	*_entity->audioSource = AF_CAudioSource_ZERO();

	// player data
	*_entity->playerData = AF_CPlayerData_ZERO();

	// skeletal animations
	*_entity->skeletalAnimation = AF_CSkeletalAnimation_ZERO();

	// ai Behaviours
	*_entity->aiBehaviour = AF_CAI_Behaviour_ZERO();
	
	// Camera Component
	*_entity->camera = AF_CCamera_ZERO();

	// Editor Component
	*_entity->editorData = AF_CEditorData_ZERO();
	
	// Input controller
	*_entity->inputController = AF_CInputController_ZERO();

	// Scripts
	for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
		uint32_t entityID = AF_ECS_GetID(_entity->id_tag);
		uint32_t scriptIndex = (entityID * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + j;
		
		// check range of id
		_ecs->scripts[scriptIndex] = AF_CScript_ZERO();
	}

	// Lights
	*_entity->light = AF_CLight_ZERO();

	if(_ecs->entitiesCount != 0){
		_ecs->currentEntity--;
	}
	
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
	// increment the entity counter
	_ecs->currentEntity++;
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

	//printf("==== Load ECS from binary file ====\n");
    if(_filePtr == NULL){
        printf("AF_File_PrintTextBuffer: FAILED to print buffer. _charBuffer is NULL\n");
        return;
    }
	fread(_ecs, sizeof(AF_ECS), 1, _filePtr); // Read the struct
	if (ferror(_filePtr)) {
        printf("AF_ECS_LoadECSFromBinary: Error while reading \n");
		return;
    }
	// resync the pointers so we don't get null reference
	AF_ECS_ReSyncComponents(_ecs);
    
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

/*
====================
AF_ECS_GetCamera
Helper function that saves the ECS structure to json format
====================
*/
static inline AF_Entity* AF_ECS_GetCamera(AF_ECS* _ecs){
    AF_Entity* cameraEntity = NULL;
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        AF_CCamera* entityCameraComponent = &_ecs->cameras[i];
        BOOL hasCamera = AF_Component_GetHas(entityCameraComponent->enabled);
        if(hasCamera == TRUE){
            cameraEntity = &_ecs->entities[i];
            AF_Log("AF_ECS_GetCamera: GetCamera: Success\n");
        }
    }
	if(cameraEntity == NULL){
		AF_Log_Error("AF_ECS_GetCamera: GetCamera: FAILED\n");
		
	}
    return cameraEntity;
}

/*
====================
AF_ECS_UpdateCameraVectors
Helper function updates the camera vectors
====================
*/
static inline void AF_ECS_UpdateCameraVectors(AF_Entity* _cameraEntityPtr, AF_FLOAT _windowWidth, AF_FLOAT _windowHeight){

    AF_CTransform3D* cameraTransform = _cameraEntityPtr->transform;
    AF_CCamera* camera = _cameraEntityPtr->camera;
    BOOL hasCameraComponent = AF_Component_GetHas(camera->enabled);
    if(hasCameraComponent == FALSE){
        AF_Log_Error("AF_Camera_UpdateCameraVectors: Can't update camera vectors, passed entity has no camera component\n");
        return;
    }
    Vec3 front = camera->cameraFront;


    // calculate Right
    Vec3 right = Vec3_NORMALIZE(Vec3_CROSS(front, camera->cameraWorldUp));
	
    // calculate up
    Vec3 up = Vec3_NORMALIZE(Vec3_CROSS(right, front));
	
    // Calculate view matrix:vs
    Mat4 viewMatrix = Mat4_Lookat(cameraTransform->pos, Vec3_ADD(cameraTransform->pos,front), up);
    camera->viewMatrix = viewMatrix;

    // Calculate projection matrix
	// TODO: disbaled setting projection matrix
    //camera->projectionMatrix = AF_Camera_GetPerspectiveProjectionMatrix(camera);

    // update the cameras model matrix to be used in shaders later
    cameraTransform->modelMat = Mat4_ToModelMat4(cameraTransform->pos, cameraTransform->rot, cameraTransform->scale);


    
    //camera->windowWidth = _windowWidth;
    //camera->windowHeight = _windowHeight;
    camera->projectionMatrix = AF_Camera_GetPerspectiveProjectionMatrix(_cameraEntityPtr->camera, _windowWidth, _windowHeight);
    
}


#endif //AF_ECS_H
