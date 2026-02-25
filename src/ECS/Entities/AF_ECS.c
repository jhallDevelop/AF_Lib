#include "ECS/Entities/AF_ECS.h"
#include "AF_RendererBuffer.h"

// ====================
// AF_ECS_ReSyncComponents
// Helper function to re-sync pointers loast likely when loading a save
// ====================
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


// ====================
// AF_ECS_Init
// Init helper function to initialise all the entities.
// Entities are all loaded into memory at the start using the compile time define AF_ECS_TOTAL_ENTITIES
// ====================
void AF_ECS_Init(AF_ECS* _ecs){
	assert(_ecs != NULL && "AF_ECS_Init: argument is null");
	// Initialise all entities in the entity pool with default values
	_ecs->entitiesCount = AF_ECS_TOTAL_ENTITIES;
	_ecs->currentEntity = 0; // init to 0
	
	// memset the sparse arrays to zero
	_ecs->meshSparseSet.count = 0;
	memset(_ecs->meshSparseSet.sparseEntityIDs, 0xFF, sizeof(_ecs->meshSparseSet.sparseEntityIDs));
	// Zero enabled bits in densecomponent array
	memset(_ecs->meshSparseSet.denseComponent, 0, sizeof(_ecs->meshSparseSet.denseComponent));
	memset(_ecs->meshSparseSet.denseToSparse, 0xFF, sizeof(_ecs->meshSparseSet.denseToSparse));

	
	
	
	for(uint32_t i = 0; i < AF_ECS_TOTAL_ENTITIES; i++){
		
		AF_Entity* entity = &_ecs->entities[i];
		flag_t* componentState = &entity->flags;
 		//entity->enabled = AF_TRUE;
		entity->flags = AF_Component_SetEnabled(*componentState, AF_FALSE);
		entity->id_tag = AF_ECS_AssignID(entity->id_tag, i);
		entity->id_tag = AF_ECS_AssignTag(entity->id_tag, 0);
		
		// zero the terrain
		_ecs->terrains[i] = AF_CTerrain_ZERO();
	}
	
	
}


// ====================
// AF_ECS_DeleteEntity
// Helper function to delete the entity 
// ====================
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
}


// ====================
// AF_ECS_DuplicateEntity
// Helper function to delete the entity and reset all the components to zero
// ====================
void AF_ECS_DuplicateEntity(AF_ECS* _ecs, AF_Entity* _entity){
	if(_ecs->currentEntity+1 >= AF_ECS_TOTAL_ENTITIES){
		AF_Log_Warning("AF_ECS_DuplicateEntity: Run out of entities, can't duplicate\n");
		return;
	}

	// Create the entity
	AF_Entity* newEntity = AF_ECS_CreateEntity(_ecs);
    uint32_t srcID = AF_ECS_GetID(_entity->id_tag);
    uint32_t dstID = AF_ECS_GetID(newEntity->id_tag);


	// copy the name
	

	// copy all the components across
	// Transform
	_ecs->transforms[dstID] = _ecs->transforms[srcID];

	// Sprite
	_ecs->sprites[dstID] = _ecs->sprites[srcID];

	// Rigidbody
	_ecs->rigidbodies[dstID] = _ecs->rigidbodies[srcID];

	// Collider
	_ecs->colliders[dstID] = _ecs->colliders[srcID];

	// Camera
	_ecs->cameras[dstID] = _ecs->cameras[srcID];

	// Animation
	_ecs->animations[dstID] = _ecs->animations[srcID];

	// Mesh
	AF_CMesh* srcMeshComponent = AF_ECS_GetMeshComponent(_ecs, srcID);
	AF_CMesh* dstMeshComponent = AF_ECS_GetMeshComponent(_ecs, dstID);

	if(srcMeshComponent != NULL){
		if(dstMeshComponent == NULL){
			dstMeshComponent = AF_ECS_AddMeshComponent(_ecs, dstID);
		}
	}
	else{
		AF_Log_Warning("AF_ECS_DuplicateEntity: Source entity does not have a mesh component\n");
	}
	*dstMeshComponent = *srcMeshComponent;	// copy the mesh component data across

	// deep copy the mesh data
	// Deep copy mesh data to prevent double-free on destruction
    for (uint32_t i = 0; i < srcMeshComponent->meshCount; ++i) {
        AF_MeshData* srcData = &srcMeshComponent->meshes[i];
        AF_MeshData* dstData = &dstMeshComponent->meshes[i];

        if (srcData->vertices != NULL && srcData->vertexCount > 0) {
            dstData->vertices = (AF_Vertex*)malloc(sizeof(AF_Vertex) * srcData->vertexCount);
            if (dstData->vertices) {
                memcpy(dstData->vertices, srcData->vertices, sizeof(AF_Vertex) * srcData->vertexCount);
            }
        }

        if (srcData->indices != NULL && srcData->indexCount > 0) {
            dstData->indices = (uint32_t*)malloc(sizeof(uint32_t) * srcData->indexCount);
            if (dstData->indices) {
                memcpy(dstData->indices, srcData->indices, sizeof(uint32_t) * srcData->indexCount);
            }
        }

        // Reset GPU buffer IDs so we generate new independent buffers
        dstData->vao = 0;
        dstData->vbo = 0;
        dstData->ibo = 0;

        // Generate new GPU buffers for the duplicate
        AF_RendererBuffer_CreateMeshBuffer(dstData);
    }

	// Text
	_ecs->texts[dstID] = _ecs->texts[srcID];

	// Audio Source
	_ecs->audioSources[dstID] = _ecs->audioSources[srcID];

	// Player data
	_ecs->playerDatas[dstID] = _ecs->playerDatas[srcID];

	// Skeletal Animation
	_ecs->skeletalAnimations[dstID] = _ecs->skeletalAnimations[srcID];

	// ai behaviours
	_ecs->aiBehaviours[dstID] = _ecs->aiBehaviours[srcID];

	// Editor Data
	_ecs->editorData[dstID] = _ecs->editorData[srcID];

	// terrain
	_ecs->terrains[dstID] = _ecs->terrains[srcID];
	
	// give it a unique name
	snprintf(_ecs->editorData[srcID].name, AF_MAX_PATH_CHAR_SIZE, "%s_copy", _ecs->editorData[srcID].name);

	// Input controller
	_ecs->inputControllers[dstID] = _ecs->inputControllers[srcID];

	// Scripts
	for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
		uint32_t entityID = AF_ECS_GetID(_entity->id_tag);
		uint32_t scriptIndex = (entityID * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + j;
		
		// check range of id
		_ecs->scripts[scriptIndex] = AF_CScript_ZERO();
	}

	// lights
	_ecs->lights[dstID] = _ecs->lights[srcID];
}


// ====================
// AF_ECS_CreateEntity
// Helper function to enable the entity and pass on a pointer reference to it
// All entities already exist in memory so this just enables it.
// ====================
AF_Entity* AF_ECS_CreateEntity(AF_ECS* _ecs){
	assert(_ecs != NULL && "AF_ECS_CreateEntity: argument is null");
	assert(_ecs->currentEntity <= _ecs->entitiesCount && "AF_ECS_CreateEntity: ECS: Ran out of entities !!!\n");
    // increment the entity count and return the reference to the next available entity
    AF_Entity* entity = &_ecs->entities[_ecs->currentEntity];
    entity->id_tag = AF_ECS_AssignID(entity->id_tag, _ecs->currentEntity);
    PACKED_CHAR* componentState = &entity->flags;
	entity->flags = AF_Component_SetHas(*componentState, AF_TRUE);
	entity->flags = AF_Component_SetEnabled(*componentState, AF_TRUE);


	uint32_t entityID = AF_ECS_GetID(entity->id_tag);
	_ecs->transforms[entityID] = AF_CTransform3D_ZERO();	

	// Set the transform to be enabled
	_ecs->transforms[entityID].enabled = AF_Component_SetHas(_ecs->transforms[entityID].enabled, AF_TRUE);
	_ecs->transforms[entityID].enabled = AF_Component_SetEnabled(_ecs->transforms[entityID].enabled, AF_TRUE);
	
	// increment the entity counter
	_ecs->currentEntity++;
    return entity;
	
}

// ====================
// AF_ECS_GetMeshComponent
// Helper function to get a pointer reference to the mesh component for an entity
// ====================
AF_CMesh* AF_ECS_GetMeshComponent(AF_ECS* _ecs, uint32_t entityID){
	assert(_ecs != NULL && "AF_CMesh_GetMeshComponent: argument is null");
	assert(entityID < _ecs->entitiesCount && "AF_CMesh_GetMeshComponent: entityID out of range");
	AF_Entity* entity = &_ecs->entities[entityID];
	uint32_t denseIndexID = _ecs->meshSparseSet.sparseEntityIDs[entityID];
	if(denseIndexID == AF_ECS_INVALID_INDEX){
		return NULL;	// entity does not have a mesh component
	}

	AF_CMesh* meshComponent = &_ecs->meshSparseSet.denseComponent[_ecs->meshSparseSet.sparseEntityIDs[entityID]];
	if(!AF_Component_GetHas(meshComponent->enabled)){
		AF_Log_Error("AF_CMesh_GetMeshComponent: Entity does not have a mesh component\n");
		return NULL;
	}
	return meshComponent;
}

// ====================
// AF_ECS_AddMeshComponent
// Helper function to add a mesh component to an entity
// ====================
AF_CMesh* AF_ECS_AddMeshComponent(AF_ECS* _ecs, uint32_t entityID){
	assert(_ecs != NULL && "AF_CMesh_AddMeshComponent: argument is null");
	assert(entityID < _ecs->entitiesCount && "AF_CMesh_AddMeshComponent: entityID out of range");
	AF_Entity* entity = &_ecs->entities[entityID];
	AF_CMesh_SparseSet* meshSparseSet = &_ecs->meshSparseSet;
	// mesh component to be added at the end of the dense array
	AF_CMesh* meshComponent = &_ecs->meshSparseSet.denseComponent[meshSparseSet->count];

	*meshComponent = AF_CMesh_ZERO();	// set the default values for the mesh component
	// just check if its already enabled, if it is then we have a problem
	if(AF_Component_GetHas(meshComponent->enabled)){
		AF_Log_Error("AF_CMesh_AddMeshComponent: Mesh component already exists for this entity\n");
		return meshComponent;
	}
	meshComponent->enabled = AF_Component_SetHas(meshComponent->enabled, AF_TRUE);
	meshComponent->enabled = AF_Component_SetEnabled(meshComponent->enabled, AF_TRUE);
	meshSparseSet->sparseEntityIDs[entityID] = meshSparseSet->count;
	meshSparseSet->denseToSparse[meshSparseSet->count] = entityID;

	// set the default mesh data
	meshComponent->recieveLights = AF_TRUE;
	meshComponent->recieveShadows = AF_TRUE;
	meshComponent->castShadows = AF_TRUE;
	meshComponent->textured = AF_TRUE;

	//AF_Log("AF_ECS_AddMeshComponent: Added mesh component to entity %u, dense index %u\n", entityID, meshSparseSet->count);
	meshSparseSet->count++;
	return meshComponent;
}




// ====================
// AF_ECS_LoadEntities
// Helper function to take in a file that hopefully is json format
// And re-construct the ECS structure
// ====================
void AF_ECS_LoadECSFromBinary(FILE* _filePtr, AF_ECS* _ecs){
	// start reading the file contents.
	// does it look like json format
	// Start creating the entityx
	// Start enabling/setting the component

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



// ====================
// AF_ECS_SaveEntities
// Helper function that saves the ECS structure to json format
// ====================
void AF_ECS_SaveECS(FILE* _file, AF_ECS* _ecs){
	if( _file || _ecs){}
	// start reading the file contents.
	// does it look like json format
	// Start creating the entity
	// Start enabling/setting the components
}

// ====================
// AF_ECS_GetCamera
// Helper function that saves the ECS structure to json format
// ====================
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

void AF_ECS_CreateCamera(AF_ECS* _ecs, Vec3 _pos){
    AF_Log_Warning("Editor_App_Start: Can't find a scene camera: creating new camera\n");
	// Setup camera
	// Create a new camera
	AF_Entity* cameraEntity = AF_ECS_CreateEntity(_ecs);//_editorAppData->viewport.cameraEntityPtr;
	uint32_t cameraEntityID = AF_ECS_GetID(cameraEntity->id_tag);
	AF_CCamera* camera = &_ecs->cameras[cameraEntityID];
	AF_CTransform3D* cameraTransform = &_ecs->transforms[cameraEntityID];
	AF_CEditorData* cameraEditorData = &_ecs->editorData[cameraEntityID];
	// let the editor viewport know about the camera
	
	// start the camera looking down the negative z=

	cameraTransform->pos = _pos;
	*camera = AF_CCamera_ADD(AF_FALSE);
	*cameraEditorData = AF_CEditorData_ADD();
    
}


// ====================
// AF_ECS_UpdateCameraVectors
// Helper function updates the camera vectors
// ====================
void AF_ECS_UpdateCameraVectors(AF_ECS* _ecs, uint32_t _cameraID, AF_FLOAT _windowWidth, AF_FLOAT _windowHeight){


	AF_CTransform3D* cameraTransform = &_ecs->transforms[_cameraID];
	AF_CCamera* camera = &_ecs->cameras[_cameraID];
    af_bool_t hasCameraComponent = AF_Component_GetHas(camera->enabled);
    if(hasCameraComponent == AF_FALSE){
        AF_Log_Error("AF_Camera_UpdateCameraVectors: Can't update camera vectors, passed entity has no camera component\n");
        return;
    }

	// Derive the front from the transformms quaternion
	Vec3 forwardVec = {0, 0, -1};
	Vec3 front = AF_Vec4_Quat_RotateVec3(cameraTransform->rot, forwardVec);
	camera->cameraFront = front;


    // calculate Right
    Vec3 right = Vec3_NORMALIZE(Vec3_CROSS(front, camera->cameraWorldUp));
	
    // calculate up
    Vec3 up = Vec3_NORMALIZE(Vec3_CROSS(right, camera->cameraFront ));
    // Calculate view matrix:vs
    Mat4 viewMatrix = Mat4_Lookat(cameraTransform->pos, Vec3_ADD(cameraTransform->pos, camera->cameraFront ), up);
    camera->viewMatrix = viewMatrix;

    // Calculate projection matrix
	// TODO: disbaled setting projection matrix
    //camera->projectionMatrix = AF_Camera_GetPerspectiveProjectionMatrix(camera);

    // update the cameras model matrix to be used in shaders later
    cameraTransform->modelMat = Mat4_ToModelMat4(cameraTransform->pos, cameraTransform->rot, cameraTransform->scale);
    camera->projectionMatrix = AF_Camera_GetPerspectiveProjectionMatrix(camera, _windowWidth, _windowHeight);
}

// =====================
// AF_Entity_FindEntityOfTag
// This function searches for an entity with a specific tag in the ECS and returns its ID.
// If no entity with the specified tag is found, it returns 0.
// =====================
uint32_t AF_ECS_FindEntityOfTag(AF_ECS* _ecs, AF_Entity_Tag_e _tag) {
    if (_ecs == NULL) {
        AF_Log_Error("GetEntityOfTag: ecs is NULL\n");
        return 0; // Return an invalid ID
    }
    
    uint32_t entityID = 0; // Default to 0 if no entity found
    // Get the entity ID from the ECS
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        if(AF_ECS_GetTag(_ecs->entities[i].id_tag) == _tag){
            entityID = i; // Return the ID of the entity with the specified tag
            return entityID;
        }
    }   
    
    if (entityID == 0) {
        AF_Log_Error("GetEntityOfTag: No entity with tag %i found\n", _tag);
    }
    
    return entityID;
}



