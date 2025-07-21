#include "ECS/Entities/AF_ECS.h"
/*
====================
AF_ECS_ReSyncComponents
Helper function to re-sync pointers loast likely when loading a save
====================
*/
void AF_ECS_ReSyncComponents(AF_ECS* _ecs){
	return;
	for(uint32_t i = 0; i < _ecs->entitiesCount; ++i){
		AF_Entity* entity = &_ecs->entities[i];
		if(entity == NULL){
			//AF_Log_Error("AF_ECS_ReSyncComponents: entity is NULL at index %u\n", i);
			continue;
		}

		// set the name
		//snprintf(entity->name, sizeof(entity->name), "%s", _ecs->entities[i].name);

		// original data is all living in fixed arrays per component

	}
}

/*
====================
AF_ECS_Init
Init helper function to initialise all the entities.
Entities are all loaded into memory at the start using the compile time define AF_ECS_TOTAL_ENTITIES
====================
*/
void AF_ECS_Init(AF_ECS* _ecs){
	assert(_ecs != NULL && "AF_ECS_Init: argument is null");
	// Initialise all entities in the entity pool with default values
	_ecs->entitiesCount = AF_ECS_TOTAL_ENTITIES;
	_ecs->currentEntity = 0; // init to 0
	
	for(uint32_t i = 0; i < AF_ECS_TOTAL_ENTITIES; i++){
		
		AF_Entity* entity = &_ecs->entities[i];
		flag_t* componentState = &entity->flags;
 		//entity->enabled = AF_TRUE;
		entity->flags = AF_Component_SetEnabled(*componentState, AF_TRUE);
		entity->id_tag = AF_ECS_AssignID(entity->id_tag, i);
		entity->id_tag = AF_ECS_AssignTag(entity->id_tag, 0);
		
		// set the name
		//snprintf(entity->name, sizeof(entity->name), "Entity: %u", i);

		// ===== Init all the component pointers =====
		

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

void AF_ECS_DeleteEntity(AF_ECS* _ecs, AF_Entity* _entity){
	AF_Log("AF_ECS_DeleteEntity\n");
	//entity->enabled = AF_TRUE;
	_entity->flags = AF_Component_SetEnabled(_entity->flags, AF_FALSE);
	_entity->flags = AF_Component_SetHas(_entity->flags, AF_FALSE);
	
	// set the name
	//snprintf(entity->name, sizeof(entity->name), "Entity: %u", i);

	// ===== Init all the component pointers =====
	
	// Scripts
	for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
		uint32_t entityID = AF_ECS_GetID(_entity->id_tag);
		uint32_t scriptIndex = (entityID * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + j;
		
		// check range of id
		_ecs->scripts[scriptIndex] = AF_CScript_ZERO();
	}


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
AF_Entity* AF_ECS_CreateEntity(AF_ECS* _ecs){
	assert(_ecs != NULL && "AF_ECS_CreateEntity: argument is null");
	assert(_ecs->currentEntity <= _ecs->entitiesCount && "AF_ECS_CreateEntity: ECS: Ran out of entities !!!\n");
    // increment the entity count and return the reference to the next available entity
    AF_Entity* entity = &_ecs->entities[_ecs->currentEntity];
    entity->id_tag = AF_ECS_AssignID(entity->id_tag, _ecs->currentEntity);
    PACKED_CHAR* componentState = &entity->flags;
	entity->flags = AF_Component_SetHas(*componentState, AF_TRUE);
	entity->flags = AF_Component_SetEnabled(*componentState, AF_TRUE);

    // Give this entity a default transform component that is enabled
    if(entity == NULL){
		printf("AF_ECS: AF_ECS_CreateEntity entity failed, and is returining a null entity\n");
		return NULL;
	}

	uint32_t entityID = AF_ECS_GetID(entity->id_tag);
	_ecs->transforms[entityID] = AF_CTransform3D_ZERO();	
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
void AF_ECS_LoadECSFromBinary(FILE* _filePtr, AF_ECS* _ecs){
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
void AF_ECS_SaveECS(FILE* _file, AF_ECS* _ecs){
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
uint32_t AF_ECS_GetCamera(AF_ECS* _ecs){
    uint32_t cameraEntityID = 0;
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        AF_CCamera* entityCameraComponent = &_ecs->cameras[i];
        af_bool_t hasCamera = AF_Component_GetHas(entityCameraComponent->enabled);
        if(hasCamera == AF_TRUE){
            cameraEntityID = i;
            //AF_Log("AF_ECS_GetCamera: GetCamera: Success\n");
			break;
        }
    }
	
    return cameraEntityID;
}

/*
====================
AF_ECS_UpdateCameraVectors
Helper function updates the camera vectors
====================
*/
void AF_ECS_UpdateCameraVectors(AF_ECS* _ecs, uint32_t _cameraID, AF_FLOAT _windowWidth, AF_FLOAT _windowHeight){


	AF_CTransform3D* cameraTransform = &_ecs->transforms[_cameraID];
	AF_CCamera* camera = &_ecs->cameras[_cameraID];
    af_bool_t hasCameraComponent = AF_Component_GetHas(camera->enabled);
    if(hasCameraComponent == AF_FALSE){
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
    camera->projectionMatrix = AF_Camera_GetPerspectiveProjectionMatrix(camera, _windowWidth, _windowHeight);
    
}

