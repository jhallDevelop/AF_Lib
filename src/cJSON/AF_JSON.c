#include "AF_JSON.h"
#include "cJSON.h"

// JSON to Component conversion functions
void AF_JSON_JsonToTransform(cJSON* _transformJSON, AF_CTransform3D* _transform);
void AF_JSON_JsonToEditorData(cJSON* _editorDataJSON, AF_CEditorData* _editorData);
void AF_JSON_JsonToSprite(cJSON* _spriteJSON, AF_CSprite* _sprite);
void AF_JSON_JsonToRigidbody(cJSON* _rigidbodyJSON, AF_C3DRigidbody* _rigidbody);
void AF_JSON_JsonToCollider(cJSON* _colliderJSON, AF_CCollider* _collider);
void AF_JSON_JsonToAnimation(cJSON* _animationJSON, AF_CAnimation* _animation);
void AF_JSON_JsonToCamera(cJSON* _cameraJSON, AF_CCamera* _camera);
void AF_JSON_JsonToMesh(cJSON* _meshJSON, AF_CMesh* _mesh);
void AF_JSON_JsonToText(cJSON* _textJSON, AF_CText* _text);
void AF_JSON_JsonToAudioSource(cJSON* _audioSourceJSON, AF_CAudioSource* _audioSource);
void AF_JSON_JsonToPlayerData(cJSON* _playerDataJSON, AF_CPlayerData* _playerData);
void AF_JSON_JsonToSkeletalAnimation(cJSON* _skeletalAnimationJSON, AF_CSkeletalAnimation* _skeletalAnimation);
void AF_JSON_JsonToAIBehaviour(cJSON* _aiBehaviourJSON, AF_CAI_Behaviour* _aiBehaviour);
void AF_JSON_JsonToInputController(cJSON* _inputControllerJSON, AF_CInputController* _inputController);
void AF_JSON_JsonToScripts(cJSON* _scriptsJSON, AF_CScript* _scripts);
void AF_JSON_JsonToLight(cJSON* _lightJSON, AF_CLight* _light);

// Component to JSON conversion functions
cJSON* AF_JSON_TransformToJson(AF_CTransform3D* _component);
cJSON* AF_JSON_SpriteToJson(AF_CSprite* _component);
cJSON* AF_JSON_RigidbodyToJson(AF_C3DRigidbody* _component);
cJSON* AF_JSON_ColliderToJson(AF_CCollider* _component);
cJSON* AF_JSON_AnimationToJson(AF_CAnimation* _component);
cJSON* AF_JSON_CameraToJson(AF_CCamera* _component);
cJSON* AF_JSON_MeshToJson(AF_CMesh* _component);
cJSON* AF_JSON_TextToJson(AF_CText* _component);
cJSON* AF_JSON_AudioSourceToJson(AF_CAudioSource* _component);
cJSON* AF_JSON_PlayerDataToJson(AF_CPlayerData* _component);
cJSON* AF_JSON_SkeletalAnimationToJson(AF_CSkeletalAnimation* _component);
cJSON* AF_JSON_AIBehaviourToJson(AF_CAI_Behaviour* _component);
cJSON* AF_JSON_EditorDataToJson(AF_CEditorData* _component);
cJSON* AF_JSON_InputControllerToJson(AF_CInputController* _component);
cJSON* AF_JSON_ScriptsToJson(AF_CScript* _component);
cJSON* AF_JSON_LightToJson(AF_CLight* _component);

cJSON* AF_JSON_Vec2ToJson(const char* _name, Vec2* _vec2, cJSON* _rootJSON);
cJSON* AF_JSON_Vec3ToJson(const char* _name, Vec3* _vec3, cJSON* _rootJSON);
cJSON* AF_JSON_Vec4ToJson(const char* _name, Vec4* _vec4, cJSON* _rootJSON);
cJSON* AF_JSON_Mat4ToJson(const char* _name, Mat4* _vec4, cJSON* _rootJSON);

#define AF_JSON_SCRIPT_JSON_NAME_PREFIX "script_"

af_bool_t AF_JSON_LoadProjectDataJson(AF_ProjectData* _projectData, FILE* _file)
{
	if (_file == NULL) {
		AF_Log_Error("AF_JSON_LoadProjectDataJson: File pointer is NULL.\n");
		return AF_FALSE;
	}

	// Read the file content into a string
	char* fileContent = NULL;
	size_t fileSize;
	fseek(_file, 0, SEEK_END);
	fileSize = ftell(_file);
	fseek(_file, 0, SEEK_SET);
	fileContent = (char*)malloc(fileSize + 1);
	if (fileContent == NULL) {
		AF_Log_Error("AF_JSON_LoadProjectDataJson: Failed to allocate memory for file content.\n");
		fclose(_file);
		return AF_FALSE;
	}

	// use cJSON to read the file content
	size_t bytesRead = fread(fileContent, 1, fileSize, _file);
	if (bytesRead != fileSize) {
		AF_Log_Error("AF_JSON_LoadProjectDataJson: Failed to read the entire file content.\n");
		free(fileContent);
		fclose(_file);
		return AF_FALSE;
	}

	fileContent[fileSize] = '\0'; //
	fclose(_file); // Close the file after reading
	AF_Log("AF_JSON_LoadProjectDataJson: File content read successfully.\n");
	// Parse the JSON content
	cJSON* rootJSON = cJSON_Parse(fileContent);
	free(fileContent); // We can free the file buffer immediately after parsing
	//rootJSON = cJSON_GetArrayItem(rootJSON, 0); // Get the first item in the array, which should be the project data

	if (rootJSON == NULL) {
		AF_Log_Error("AF_JSON_LoadProjectDataJson: Failed to parse JSON content.\n");
		free(fileContent); // Free the file content after parsing
		return AF_FALSE;
	}

	AF_Log("AF_JSON_LoadProjectDataJson: JSON content parsed successfully.\n");


	// Now you can access the JSON data and populate the ECS components
	// For each entity in the JSON

	// Get "name"
	const cJSON* nameJSON = cJSON_GetObjectItem(rootJSON, "name");
	snprintf(_projectData->name, AF_MAX_PATH_CHAR_SIZE, "%s", nameJSON->valuestring);

	// Get "assetPath"
	const cJSON* assetPathJSON = cJSON_GetObjectItem(rootJSON, "assetPath");
	snprintf(_projectData->assetsPath, AF_MAX_PATH_CHAR_SIZE, "%s", assetPathJSON->valuestring);

	// Get "defaultAppDataPath"
	const cJSON* defaultAppDataPathJSON = cJSON_GetObjectItem(rootJSON, "defaultAppDataPath");
	snprintf(_projectData->defaultAppDataPath, AF_MAX_PATH_CHAR_SIZE, "%s", defaultAppDataPathJSON->valuestring);


	// Get "defaultScenePath"
	const cJSON* defaultScenePathJSON = cJSON_GetObjectItem(rootJSON, "defaultScenePath");
	snprintf(_projectData->defaultScenePath, AF_MAX_PATH_CHAR_SIZE, "%s", defaultScenePathJSON->valuestring);


	// Get "projectRoot"
	const cJSON* projectRootPathJSON = cJSON_GetObjectItem(rootJSON, "projectRoot");
	snprintf(_projectData->projectRoot, AF_MAX_PATH_CHAR_SIZE, "%s", projectRootPathJSON->valuestring);

	// Project Data
	const cJSON* projectDataJSON = cJSON_GetObjectItem(rootJSON, "projectData");

	// platform data
	const cJSON* platformDataJSON = cJSON_GetObjectItem(rootJSON, "platformData");
	uint32_t platformInt = platformDataJSON->valueint;
	_projectData->platformData.platformType = (AF_Platform_e)platformInt;

	// (Note: You can add similar checks for buildData and platformData if you need them, using cJSON_IsNull)

	// 4. IMPORTANT: Clean up the cJSON object to prevent memory leaks.
	cJSON_Delete(rootJSON);

	return AF_TRUE;
}



af_bool_t AF_JSON_LoadSceneJson(AF_AppData* _appData, FILE* _file)
{
	if (_file == NULL) {
		AF_Log_Error("AF_JSON_LoadJson: File pointer is NULL.\n");
		return AF_FALSE;
	}

	if (_appData->projectData.projectRoot[0] == '\0') {
		AF_Log_Error("AF_JSON_LoadJson: Project root is not set.\n");
		return AF_FALSE;
	}

	// Read the file content into a string
	char* fileContent = NULL;
	size_t fileSize;
	fseek(_file, 0, SEEK_END);
	fileSize = ftell(_file);
	fseek(_file, 0, SEEK_SET);
	fileContent = (char*)malloc(fileSize + 1);
	if (fileContent == NULL) {
		AF_Log_Error("AF_JSON_LoadJson: Failed to allocate memory for file content.\n");
		fclose(_file);
		return AF_FALSE;
	}

	// use cJSON to read the file content
	size_t bytesRead = fread(fileContent, 1, fileSize, _file);
	if (bytesRead != fileSize) {
		AF_Log_Error("AF_JSON_LoadJson: Failed to read the entire file content.\n");
		free(fileContent);
		fclose(_file);
		return AF_FALSE;
	}

	fileContent[fileSize] = '\0'; //
	fclose(_file); // Close the file after reading
	AF_Log("AF_JSON_LoadJson: File content read successfully.\n");
	// Parse the JSON content
	cJSON* rootJSON = cJSON_Parse(fileContent);
	free(fileContent); // Free the file content after parsing
	if (rootJSON == NULL) {
		AF_Log_Error("AF_JSON_LoadJson: Failed to parse JSON content.\n");
		return AF_FALSE;
	}

	AF_Log("AF_JSON_LoadJson: JSON content parsed successfully.\n");


	// Now you can access the JSON data and populate the ECS components
	// For each entity in the JSON
	cJSON* entityJSON = NULL;
	cJSON_ArrayForEach(entityJSON, rootJSON) {
		if (entityJSON == NULL) {
			AF_Log_Error("AF_JSON_LoadJson: No entities found in JSON.\n");
			cJSON_Delete(rootJSON);
			return AF_FALSE;
		}

		// Create a new entity in the ECS
		AF_ECS* ecs = &_appData->ecs;
		AF_Entity* newEntity = &_appData->ecs.entities[ecs->currentEntity];
		newEntity->id_tag = AF_ECS_AssignID(newEntity->id_tag, cJSON_GetObjectItem(entityJSON, "id")->valueint); // Get ID tag from JSON
		newEntity->id_tag = AF_ECS_AssignTag(newEntity->id_tag, cJSON_GetObjectItem(entityJSON, "tag")->valueint); // Get tag from JSON

		newEntity->flags = AF_Component_SetEnabled(newEntity->flags, cJSON_GetObjectItem(entityJSON, "enabled")->valueint); // Get enabled flag from JSON
		newEntity->flags = AF_Component_SetHas(newEntity->flags, cJSON_GetObjectItem(entityJSON, "has")->valueint); // Get has flag from JSON

		if (AF_Component_GetHas(newEntity->flags) == AF_FALSE) {
			//AF_Log("AF_JSON_LoadJson: Entity %d is not enabled, skipping.\n", ecs->currentEntity);
			continue; // Skip entities that are not enabled
		}

		// Transform Component
		cJSON* transformJSON = cJSON_GetObjectItem(entityJSON, "transform");
		AF_CTransform3D* transform = &ecs->transforms[ecs->currentEntity]; // Initialize with default values
		AF_JSON_JsonToTransform(transformJSON, transform);

		// Sprite Component
		cJSON* spriteJSON = cJSON_GetObjectItem(entityJSON, "sprite");
		AF_JSON_JsonToSprite(spriteJSON, &ecs->sprites[ecs->currentEntity]);

		// Rigidbody Component
		cJSON* rigidbodyJSON = cJSON_GetObjectItem(entityJSON, "rigidbody");
		AF_JSON_JsonToRigidbody(rigidbodyJSON, &ecs->rigidbodies[ecs->currentEntity]);

		// Collider Component
		cJSON* colliderJSON = cJSON_GetObjectItem(entityJSON, "collider");
		AF_JSON_JsonToCollider(colliderJSON, &ecs->colliders[ecs->currentEntity]);

		// Animation Component
		cJSON* animationJSON = cJSON_GetObjectItem(entityJSON, "animation");
		AF_JSON_JsonToAnimation(animationJSON, &ecs->animations[ecs->currentEntity]);

		// Camera Component
		cJSON* cameraJSON = cJSON_GetObjectItem(entityJSON, "camera");
		AF_JSON_JsonToCamera(cameraJSON, &ecs->cameras[ecs->currentEntity]);

		// Mesh Component
		cJSON* meshJSON = cJSON_GetObjectItem(entityJSON, "mesh");
		AF_JSON_JsonToMesh(meshJSON, &ecs->meshes[ecs->currentEntity]);

		// Text Component
		cJSON* textJSON = cJSON_GetObjectItem(entityJSON, "text");
		//AF_JSON_JsonToText(textJSON, &ecs->texts[entityIndex]);

		// Audio Source Component
		cJSON* audioSourceJSON = cJSON_GetObjectItem(entityJSON, "audioSource");
		//AF_JSON_JsonToAudioSource(audioSourceJSON, &ecs->audioSources[entityIndex]);

		// Player Data Component
		cJSON* playerDataJSON = cJSON_GetObjectItem(entityJSON, "playerData");
		//AF_JSON_JsonToPlayerData(playerDataJSON, &ecs->playerDatas[entityIndex]);

		// Skeletal Animation Component
		cJSON* skeletalAnimationJSON = cJSON_GetObjectItem(entityJSON, "skeletalAnimation");
		//AF_JSON_JsonToSkeletalAnimation(skeletalAnimationJSON, &ecs->skeletalAnimations[entityIndex]);

		// AI Behaviour Component
		cJSON* aiBehaviourJSON = cJSON_GetObjectItem(entityJSON, "aiBehaviour");
		//AF_JSON_JsonToAIBehaviour(aiBehaviourJSON, &ecs->aiBehaviours[entityIndex]);

		// Input Controller Component
		cJSON* inputControllerJSON = cJSON_GetObjectItem(entityJSON, "inputController");
		//AF_JSON_JsonToInputController(inputControllerJSON, &ecs->inputControllers[entityIndex]);

		// Editor Data Component
		cJSON* editorDataJSON = cJSON_GetObjectItem(entityJSON, "editorData");
		AF_JSON_JsonToEditorData(editorDataJSON, &ecs->editorData[ecs->currentEntity]);

		// Scripts Component
		
		for(uint32_t i = 0; i < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; i++) {
			char scriptJSONName[AF_MAX_PATH_CHAR_SIZE];
			snprintf(scriptJSONName, AF_MAX_PATH_CHAR_SIZE, "%s%i", AF_JSON_SCRIPT_JSON_NAME_PREFIX, i);
			cJSON* scriptsJSON = cJSON_GetObjectItem(entityJSON, scriptJSONName);
			// We use a flat array so need to stride through to get an index
			uint32_t scriptID = (ecs->currentEntity * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + i;
			AF_JSON_JsonToScripts(scriptsJSON, &ecs->scripts[scriptID]);
		}
		

		// Light Component
		cJSON* lightJSON = cJSON_GetObjectItem(entityJSON, "light");
		AF_JSON_JsonToLight(lightJSON, &ecs->lights[ecs->currentEntity]);


		ecs->currentEntity++; // Increment entity index for each entity found
	}

	return AF_TRUE;

}

af_bool_t AF_JSON_SaveProjectDataToJson(AF_ProjectData* _projectData, char* _charBuffer, uint32_t _charBufferSize)
{
	af_bool_t returnBool = AF_FALSE;
	AF_Log("AF_JSON_SaveProjectDataToJson: SaveProjectDataToJSON\n");

	cJSON* rootJSON = cJSON_CreateObject();

	//char name[MAX_PROJECTDATA_FILE_PATH];
	cJSON* nameJSON = cJSON_AddStringToObject(rootJSON, "name", _projectData->name);

	//char projectRoot[MAX_PROJECTDATA_FILE_PATH];
	cJSON* projectRootJSON = cJSON_AddStringToObject(rootJSON, "projectRoot", _projectData->projectRoot);

	//char assetsPath[MAX_PROJECTDATA_FILE_PATH];
	cJSON* assetPathJSON = cJSON_AddStringToObject(rootJSON, "assetPath", _projectData->assetsPath);

	//char defaultAppDataPath[MAX_PROJECTDATA_FILE_PATH];
	cJSON* defaultAppDataPathJSON = cJSON_AddStringToObject(rootJSON, "defaultAppDataPath", _projectData->defaultAppDataPath);

	//char defaultScenePath[MAX_PROJECTDATA_FILE_PATH];
	cJSON* defaultScenePathJSON = cJSON_AddStringToObject(rootJSON, "defaultScenePath", _projectData->defaultScenePath);

	//AF_PlatformData platformData;
	cJSON* platformNameJSON = cJSON_AddNumberToObject(rootJSON, "platformData", _projectData->platformData.platformType);

	//AF_BuildGameData buildData;
	cJSON* buildData = cJSON_AddNullToObject(rootJSON, "buildData");

	// save to text buffer
	snprintf(_charBuffer, _charBufferSize, "%s", cJSON_Print(rootJSON));
	if (_charBuffer == NULL)
	{
		AF_Log_Error("AF_JSON_SaveProjectDataToJson: Failed to print monitor.\n");
		return AF_FALSE;
	}

	return returnBool;

}

af_bool_t AF_JSON_SaveECSToJson(AF_ECS* _ecs, char* _charBuffer, uint32_t _charBufferSize)
{
	AF_Log("AF_JSON_SaveECSToJson: SaveECSToJSON\n");

	cJSON* rootJSON = cJSON_CreateObject();
	// For each entity
	for (uint32_t i = 0; i < _ecs->entitiesCount; i++) {

		// Entity
		AF_Entity* entity = &_ecs->entities[i];

		// Entity json
		cJSON* entityJSON = cJSON_AddObjectToObject(rootJSON, "entity");
		cJSON_AddNumberToObject(entityJSON, "id", AF_ECS_GetID(entity->id_tag)); // Add ID
		cJSON_AddNumberToObject(entityJSON, "tag", AF_ECS_GetTag(entity->id_tag)); // Add tag
		cJSON_AddBoolToObject(entityJSON, "enabled", AF_Component_GetEnabled(entity->flags)); // Add enabled flag
		cJSON_AddBoolToObject(entityJSON, "has", AF_Component_GetHas(entity->flags)); // Add has flag

		// for each component
		af_bool_t flags = (af_bool_t)entity->flags; // TODO: extract the flags
		af_bool_t id = AF_ECS_GetID(entity->id_tag);	// ID
		af_bool_t tag = AF_ECS_GetTag(entity->id_tag);


		// Transform
		AF_CTransform3D* transform = &_ecs->transforms[i];	// 3d transform component
		char transformTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* transformJSON = AF_JSON_TransformToJson(&_ecs->transforms[i]);
		cJSON_AddItemToObject(entityJSON, "transform", transformJSON);

		// Sprite
		AF_CSprite* sprite = &_ecs->sprites[i];		// sprite cmponent
		char spriteText[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* spriteJSON = AF_JSON_SpriteToJson(&_ecs->sprites[i]);
		cJSON_AddItemToObject(entityJSON, "sprite", spriteJSON);

		// Rigidbody
		AF_C3DRigidbody* rigidbody = &_ecs->rigidbodies[i]; // 3d rigidbody
		char rigidbodyTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* rigidbodyJSON = AF_JSON_RigidbodyToJson(&_ecs->rigidbodies[i]);
		cJSON_AddItemToObject(entityJSON, "rigidbody", rigidbodyJSON);

		// Collider
		AF_CCollider* collider = &_ecs->colliders[i];	// Collider component
		char colliderTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* colliderJSON = AF_JSON_ColliderToJson(&_ecs->colliders[i]);
		cJSON_AddItemToObject(entityJSON, "collider", colliderJSON);

		// Animation
		AF_CAnimation* animation = &_ecs->animations[i];	// animation Component
		char animationTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* animationJSON = AF_JSON_AnimationToJson(&_ecs->animations[i]);
		cJSON_AddItemToObject(entityJSON, "animation", animationJSON);

		// Cameras
		AF_CCamera* camera = &_ecs->cameras[i];		// camera component
		char cameraTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* cameraJSON = AF_JSON_CameraToJson(&_ecs->cameras[i]);
		cJSON_AddItemToObject(entityJSON, "camera", cameraJSON);

		// Mesh
		AF_CMesh* mesh = &_ecs->meshes[i];		// mesh component 	
		char meshTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* meshJSON = AF_JSON_MeshToJson(&_ecs->meshes[i]);
		cJSON_AddItemToObject(entityJSON, "mesh", meshJSON);

		// Text
		AF_CText* text = &_ecs->texts[i];
		char textTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* textJSON = AF_JSON_TextToJson(&_ecs->texts[i]);
		cJSON_AddItemToObject(entityJSON, "text", textJSON);

		// Audio Source
		AF_CAudioSource* audioSource = &_ecs->audioSources[i];
		char audioTextBufferBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* audioSourceJSON = AF_JSON_AudioSourceToJson(&_ecs->audioSources[i]);
		cJSON_AddItemToObject(entityJSON, "audio", audioSourceJSON);

		// Player Data
		AF_CPlayerData* playerData = &_ecs->playerDatas[i];
		char playerDataTextBufferBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* playerDataJSON = AF_JSON_PlayerDataToJson(&_ecs->playerDatas[i]);
		cJSON_AddItemToObject(entityJSON, "playerData", playerDataJSON);

		// Skeletal Animation
		AF_CSkeletalAnimation* skeletalAnimation = &_ecs->skeletalAnimations[i];
		char skeletalTextBufferBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* skeletalJSON = AF_JSON_SkeletalAnimationToJson(&_ecs->skeletalAnimations[i]);
		cJSON_AddItemToObject(entityJSON, "skeletal", skeletalJSON);

		// AI Behaviour
		AF_CAI_Behaviour* aiBehaviour = &_ecs->aiBehaviours[i];
		char aiTextBufferBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* aiBehaviourJSON = AF_JSON_AIBehaviourToJson(&_ecs->aiBehaviours[i]);
		cJSON_AddItemToObject(entityJSON, "aiBehaviour", aiBehaviourJSON);


		// Editor Data
		AF_CEditorData* editorData = &_ecs->editorData[i];
		char editorDataTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* editorDataJSON = AF_JSON_EditorDataToJson(&_ecs->editorData[i]);
		cJSON_AddItemToObject(entityJSON, "editorData", editorDataJSON);

		// Input Controller
		AF_CInputController* inputController = &_ecs->inputControllers[i];
		char inputControllerTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* inputControllerJSON = AF_JSON_InputControllerToJson(&_ecs->inputControllers[i]);
		cJSON_AddItemToObject(entityJSON, "inputController", inputControllerJSON);


		// Scripts

		for(uint32_t x = 0; x < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; x++) {
			// We use a flat array so need to stride through to get an index
			uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + x;
			cJSON* scriptsDataJSON = AF_JSON_ScriptsToJson(&_ecs->scripts[scriptID]);
			char scriptsJSONName[AF_MAX_PATH_CHAR_SIZE] = "\0";
			snprintf(scriptsJSONName, AF_MAX_PATH_CHAR_SIZE, "%s%i", AF_JSON_SCRIPT_JSON_NAME_PREFIX, x);

			cJSON_AddItemToObject(entityJSON, scriptsJSONName, scriptsDataJSON);
		}

		// Lights
		AF_CLight* light = &_ecs->lights[i];
		char lightsTextBuffer[AF_MAX_PATH_CHAR_SIZE] = "\0";
		cJSON* lightDataJSON = AF_JSON_LightToJson(&_ecs->lights[i]);
		cJSON_AddItemToObject(entityJSON, "light", lightDataJSON);

	}

	// save to text buffer
	snprintf(_charBuffer, _charBufferSize, "%s", cJSON_Print(rootJSON));
	if (_charBuffer == NULL)
	{
		AF_Log_Error("AF_JSON_SaveECSToJson: Failed to print monitor.\n");
		return AF_FALSE;
	}

	return AF_TRUE;
}

// JSON TO COMPONENTS CONVERTERS
void AF_JSON_JsonToTransform(cJSON* _transformJSON, AF_CTransform3D* _transform) {
	if (_transformJSON == NULL || _transform == NULL) {
		AF_Log_Error("AF_JSON_JsonToTransform: Invalid JSON or transform pointer.\n");
		return;
	}

	// Has
	_transform->enabled = AF_Component_SetHas(_transform->enabled, cJSON_GetObjectItem(_transformJSON, "has")->valueint);

	// Enabled
	_transform->enabled = AF_Component_SetEnabled(_transform->enabled, cJSON_GetObjectItem(_transformJSON, "enabled")->valueint);

	// Pos
	cJSON* posJSON = cJSON_GetObjectItem(_transformJSON, "pos");
	if (posJSON != NULL) {
		_transform->pos.x = cJSON_GetArrayItem(posJSON, 0)->valuedouble;
		_transform->pos.y = cJSON_GetArrayItem(posJSON, 1)->valuedouble;
		_transform->pos.z = cJSON_GetArrayItem(posJSON, 2)->valuedouble;
	}

	// Local Pos
	cJSON* localPosJSON = cJSON_GetObjectItem(_transformJSON, "localPos");
	if (localPosJSON != NULL) {
		_transform->localPos.x = cJSON_GetArrayItem(localPosJSON, 0)->valuedouble;
		_transform->localPos.y = cJSON_GetArrayItem(localPosJSON, 1)->valuedouble;
		_transform->localPos.z = cJSON_GetArrayItem(localPosJSON, 2)->valuedouble;
	}

	// Rot
	cJSON* rotJSON = cJSON_GetObjectItem(_transformJSON, "rot");
	if (rotJSON != NULL) {
		_transform->rot.x = cJSON_GetArrayItem(rotJSON, 0)->valuedouble;
		_transform->rot.y = cJSON_GetArrayItem(rotJSON, 1)->valuedouble;
		_transform->rot.z = cJSON_GetArrayItem(rotJSON, 2)->valuedouble;
	}

	// Local Rot
	cJSON* localRotJSON = cJSON_GetObjectItem(_transformJSON, "localRot");
	if (localRotJSON != NULL) {
		_transform->localRot.x = cJSON_GetArrayItem(localRotJSON, 0)->valuedouble;
		_transform->localRot.y = cJSON_GetArrayItem(localRotJSON, 1)->valuedouble;
		_transform->localRot.z = cJSON_GetArrayItem(localRotJSON, 2)->valuedouble;
	}
	// Scale
	cJSON* scaleJSON = cJSON_GetObjectItem(_transformJSON, "scale");
	if (scaleJSON != NULL) {
		_transform->scale.x = cJSON_GetArrayItem(scaleJSON, 0)->valuedouble;
		_transform->scale.y = cJSON_GetArrayItem(scaleJSON, 1)->valuedouble;
		_transform->scale.z = cJSON_GetArrayItem(scaleJSON, 2)->valuedouble;
	}
	// Local Scale
	cJSON* localScaleJSON = cJSON_GetObjectItem(_transformJSON, "localScale");
	if (localScaleJSON != NULL) {
		_transform->localScale.x = cJSON_GetArrayItem(localScaleJSON, 0)->valuedouble;
		_transform->localScale.y = cJSON_GetArrayItem(localScaleJSON, 1)->valuedouble;
		_transform->localScale.z = cJSON_GetArrayItem(localScaleJSON, 2)->valuedouble;
	}
	// Orientation
	cJSON* orientationJSON = cJSON_GetObjectItem(_transformJSON, "orientation");
	if (orientationJSON != NULL) {
		_transform->orientation.x = cJSON_GetArrayItem(orientationJSON, 0)->valuedouble;
		_transform->orientation.y = cJSON_GetArrayItem(orientationJSON, 1)->valuedouble;
		_transform->orientation.z = cJSON_GetArrayItem(orientationJSON, 2)->valuedouble;
		_transform->orientation.w = cJSON_GetArrayItem(orientationJSON, 3)->valuedouble;
	}

	// don't worry about the model matrix, it will be updated later by the renderer
}

// sprite
void AF_JSON_JsonToSprite(cJSON* _spriteJSON, AF_CSprite* _sprite) {
	if (_spriteJSON == NULL || _sprite == NULL) {
		AF_Log_Error("AF_JSON_JsonToSprite: Invalid JSON or sprite pointer.\n");
		return;
	}

	// Has
	_sprite->enabled = AF_Component_SetHas(_sprite->enabled, cJSON_GetObjectItem(_spriteJSON, "has")->valueint);

	// Enabled
	_sprite->enabled = AF_Component_SetEnabled(_sprite->enabled, cJSON_GetObjectItem(_spriteJSON, "enabled")->valueint);

	// Loop
	_sprite->loop = cJSON_GetObjectItem(_spriteJSON, "loop")->valueint;

	// Current Frame
	_sprite->currentFrame = (char)cJSON_GetObjectItem(_spriteJSON, "currentFrame")->valueint;

	// Animation Frames
	_sprite->animationFrames = (char)cJSON_GetObjectItem(_spriteJSON, "animationFrames")->valueint;

	// Current Frame Time
	_sprite->currentFrameTime = (AF_FLOAT)cJSON_GetObjectItem(_spriteJSON, "currentFrameTime")->valuedouble;

	// currentFrameTime
	_sprite->currentFrameTime = (AF_FLOAT)cJSON_GetObjectItem(_spriteJSON, "currentFrameTime")->valuedouble;

	// Next Frame Time
	_sprite->nextFrameTime = (AF_FLOAT)cJSON_GetObjectItem(_spriteJSON, "nextFrameTime")->valuedouble;

	// Animation Speed
	_sprite->animationSpeed = (AF_FLOAT)cJSON_GetObjectItem(_spriteJSON, "animationSpeed")->valuedouble;

	// Sprite Position
	cJSON* spritePosJSON = cJSON_GetObjectItem(_spriteJSON, "spritePos");
	if (spritePosJSON != NULL) {
		_sprite->spritePos.x = cJSON_GetArrayItem(spritePosJSON, 0)->valuedouble;
		_sprite->spritePos.y = cJSON_GetArrayItem(spritePosJSON, 1)->valuedouble;
	}

	// Sprite Size
	cJSON* spriteSizeJSON = cJSON_GetObjectItem(_spriteJSON, "spriteSize");
	if (spriteSizeJSON != NULL) {
		_sprite->spriteSize.x = cJSON_GetArrayItem(spriteSizeJSON, 0)->valuedouble;
		_sprite->spriteSize.y = cJSON_GetArrayItem(spriteSizeJSON, 1)->valuedouble;
	}

	// Sprite Scale
	cJSON* spriteScaleJSON = cJSON_GetObjectItem(_spriteJSON, "spriteScale");
	if (spriteScaleJSON != NULL) {
		_sprite->spriteScale.x = cJSON_GetArrayItem(spriteScaleJSON, 0)->valuedouble;
		_sprite->spriteScale.y = cJSON_GetArrayItem(spriteScaleJSON, 1)->valuedouble;
	}

	// Sprite Rotation
	_sprite->spriteRotation = (AF_FLOAT)cJSON_GetObjectItem(_spriteJSON, "spriteRotation")->valuedouble;

	// flipX
	_sprite->flipX = cJSON_GetObjectItem(_spriteJSON, "flipX")->valueint;
	// flipY
	_sprite->flipY = cJSON_GetObjectItem(_spriteJSON, "flipY")->valueint;

	// Sprite Sheet Size
	cJSON* spriteSheetSizeJSON = cJSON_GetObjectItem(_spriteJSON, "spriteSheetSize");
	if (spriteSheetSizeJSON != NULL) {
		_sprite->spriteSheetSize.x = cJSON_GetArrayItem(spriteSheetSizeJSON, 0)->valuedouble;
		_sprite->spriteSheetSize.y = cJSON_GetArrayItem(spriteSheetSizeJSON, 1)->valuedouble;
	}

	// Sprite Color
	cJSON* spriteColorJSON = cJSON_GetObjectItem(_spriteJSON, "spriteColor");
	if (spriteColorJSON != NULL) {
		_sprite->spriteColor[0] = (uint8_t)cJSON_GetArrayItem(spriteColorJSON, 0)->valueint;
		_sprite->spriteColor[1] = (uint8_t)cJSON_GetArrayItem(spriteColorJSON, 1)->valueint;
		_sprite->spriteColor[2] = (uint8_t)cJSON_GetArrayItem(spriteColorJSON, 2)->valueint;
		_sprite->spriteColor[3] = (uint8_t)cJSON_GetArrayItem(spriteColorJSON, 3)->valueint;
	}

	// Sprite Path
	cJSON* spritePathJSON = cJSON_GetObjectItem(_spriteJSON, "spritePath");
	if (spritePathJSON != NULL && cJSON_IsString(spritePathJSON)) {
		snprintf(_sprite->spritePath, AF_MAX_PATH_CHAR_SIZE, "%s", spritePathJSON->valuestring); // Copy the string to the spritePath
	}
	else {
		snprintf(_sprite->spritePath, AF_MAX_PATH_CHAR_SIZE, "%s", "\0"); // Set to NULL if not found or not a string
	}
	// Sprite Data
	// skip sprite data for now, it will be loaded later when the sprite is used

	// Is Playing
	_sprite->isPlaying = cJSON_GetObjectItem(_spriteJSON, "isPlaying")->valueint;

	// Filtering
	_sprite->filtering = cJSON_GetObjectItem(_spriteJSON, "filtering")->valueint;
}

// rigidbody
void AF_JSON_JsonToRigidbody(cJSON* _rigidbodyJSON, AF_C3DRigidbody* _rigidbody) {
	if (_rigidbodyJSON == NULL || _rigidbody == NULL) {
		AF_Log_Error("AF_JSON_JsonToRigidbody: Invalid JSON or rigidbody pointer.\n");
		return;
	}

	// Has
	_rigidbody->enabled = AF_Component_SetHas(_rigidbody->enabled, cJSON_GetObjectItem(_rigidbodyJSON, "has")->valueint);

	// Enabled
	_rigidbody->enabled = AF_Component_SetEnabled(_rigidbody->enabled, cJSON_GetObjectItem(_rigidbodyJSON, "enabled")->valueint);

	// Is Kinematic
	_rigidbody->isKinematic = cJSON_GetObjectItem(_rigidbodyJSON, "isKinematic")->valueint;

	// Gravity
	_rigidbody->gravity = cJSON_GetObjectItem(_rigidbodyJSON, "gravity")->valueint;

	// Velocity
	cJSON* velocityJSON = cJSON_GetObjectItem(_rigidbodyJSON, "velocity");
	if (velocityJSON != NULL) {
		_rigidbody->velocity.x = cJSON_GetArrayItem(velocityJSON, 0)->valuedouble;
		_rigidbody->velocity.y = cJSON_GetArrayItem(velocityJSON, 1)->valuedouble;
		_rigidbody->velocity.z = cJSON_GetArrayItem(velocityJSON, 2)->valuedouble;
	}

	// Angular Velocity
	cJSON* angularVelocityJSON = cJSON_GetObjectItem(_rigidbodyJSON, "angularVelocity");
	if (angularVelocityJSON != NULL) {
		_rigidbody->anglularVelocity.x = cJSON_GetArrayItem(angularVelocityJSON, 0)->valuedouble;
		_rigidbody->anglularVelocity.y = cJSON_GetArrayItem(angularVelocityJSON, 1)->valuedouble;
		_rigidbody->anglularVelocity.z = cJSON_GetArrayItem(angularVelocityJSON, 2)->valuedouble;
	}

	// Inverse Mass
	cJSON* inverseMassJSON = cJSON_GetObjectItem(_rigidbodyJSON, "inverseMass");
	if (inverseMassJSON != NULL) {
		_rigidbody->inverseMass = (AF_FLOAT)cJSON_GetNumberValue(inverseMassJSON);
	}
	else {
		_rigidbody->inverseMass = 1.0f; // Default value if not found
	}

	// Force
	cJSON* forceJSON = cJSON_GetObjectItem(_rigidbodyJSON, "force");
	if (forceJSON != NULL) {
		_rigidbody->force.x = cJSON_GetArrayItem(forceJSON, 0)->valuedouble;
		_rigidbody->force.y = cJSON_GetArrayItem(forceJSON, 1)->valuedouble;
		_rigidbody->force.z = cJSON_GetArrayItem(forceJSON, 2)->valuedouble;
	}

	// Torque
	cJSON* torqueJSON = cJSON_GetObjectItem(_rigidbodyJSON, "torque");
	if (torqueJSON != NULL) {
		_rigidbody->torque.x = cJSON_GetArrayItem(torqueJSON, 0)->valuedouble;
		_rigidbody->torque.y = cJSON_GetArrayItem(torqueJSON, 1)->valuedouble;
		_rigidbody->torque.z = cJSON_GetArrayItem(torqueJSON, 2)->valuedouble;
	}

	// Inertia Tensor
	cJSON* inertiaTensorJSON = cJSON_GetObjectItem(_rigidbodyJSON, "inertiaTensor");
	if (inertiaTensorJSON != NULL) {
		_rigidbody->inertiaTensor.x = cJSON_GetArrayItem(inertiaTensorJSON, 0)->valuedouble;
		_rigidbody->inertiaTensor.y = cJSON_GetArrayItem(inertiaTensorJSON, 1)->valuedouble;
		_rigidbody->inertiaTensor.z = cJSON_GetArrayItem(inertiaTensorJSON, 2)->valuedouble;
	}
	else {
		// Default inertia tensor if not provided
		Vec3 inertiaTensor = { 1.0f, 1.0f, 1.0f }; // Assuming a default value
		_rigidbody->inertiaTensor = inertiaTensor; // Assuming a default value
	}
}

// collider
void AF_JSON_JsonToCollider(cJSON* _colliderJSON, AF_CCollider* _collider) {
	if (_colliderJSON == NULL || _collider == NULL) {
		AF_Log_Error("AF_JSON_JsonToCollider: Invalid JSON or collider pointer.\n");
		return;
	}

	// Has
	_collider->enabled = AF_Component_SetHas(_collider->enabled, cJSON_GetObjectItem(_colliderJSON, "has")->valueint);
	// Enabled
	_collider->enabled = AF_Component_SetEnabled(_collider->enabled, cJSON_GetObjectItem(_colliderJSON, "enabled")->valueint);
	// Type
	_collider->type = (enum CollisionVolumeType)cJSON_GetObjectItem(_colliderJSON, "type")->valueint;
	// Bounding Volume
	cJSON* boundingVolumeJSON = cJSON_GetObjectItem(_colliderJSON, "boundingVolume");
	if (boundingVolumeJSON != NULL) {
		_collider->boundingVolume.x = cJSON_GetArrayItem(boundingVolumeJSON, 0)->valuedouble;
		_collider->boundingVolume.y = cJSON_GetArrayItem(boundingVolumeJSON, 1)->valuedouble;
		_collider->boundingVolume.z = cJSON_GetArrayItem(boundingVolumeJSON, 2)->valuedouble;
	}
	// Position
	cJSON* posJSON = cJSON_GetObjectItem(_colliderJSON, "pos");
	if (posJSON != NULL) {
		_collider->pos.x = cJSON_GetArrayItem(posJSON, 0)->valuedouble;
		_collider->pos.y = cJSON_GetArrayItem(posJSON, 1)->valuedouble;
		_collider->pos.z = cJSON_GetArrayItem(posJSON, 2)->valuedouble;
	}
	// Collision, skipp for now

	// Show Debug
	_collider->showDebug = cJSON_GetObjectItem(_colliderJSON, "showDebug")->valueint;
	// Broadphase AABB
	cJSON* broadphaseAABBJSON = cJSON_GetObjectItem(_colliderJSON, "broadphaseAABB");
	if (broadphaseAABBJSON != NULL) {
		_collider->broadphaseAABB.x = cJSON_GetArrayItem(broadphaseAABBJSON, 0)->valuedouble;
		_collider->broadphaseAABB.y = cJSON_GetArrayItem(broadphaseAABBJSON, 1)->valuedouble;
		_collider->broadphaseAABB.z = cJSON_GetArrayItem(broadphaseAABBJSON, 2)->valuedouble;
	}
	else {
		// Default broadphase AABB if not provided
		Vec3 broadphaseAABB = { 0.0f, 0.0f, 0.0f }; // Assuming a default value
		_collider->broadphaseAABB = broadphaseAABB; // Assuming a default value
	}
}

// animation
void AF_JSON_JsonToAnimation(cJSON* _animationJSON, AF_CAnimation* _animation) {
	if (_animationJSON == NULL || _animation == NULL) {
		AF_Log_Error("AF_JSON_JsonToAnimation: Invalid JSON or animation pointer.\n");
		return;
	}
	// Has
	_animation->enabled = AF_Component_SetHas(_animation->enabled, cJSON_GetObjectItem(_animationJSON, "has")->valueint);
	// Enabled
	_animation->enabled = AF_Component_SetEnabled(_animation->enabled, cJSON_GetObjectItem(_animationJSON, "enabled")->valueint);
	// Animation Speed
	_animation->animationSpeed = (AF_FLOAT)cJSON_GetObjectItem(_animationJSON, "animationSpeed")->valuedouble;
	// Next Frame Time
	_animation->nextFrameTime = (AF_FLOAT)cJSON_GetObjectItem(_animationJSON, "nextFrameTime")->valuedouble;
	// Current Frame
	_animation->currentFrame = (uint8_t)cJSON_GetObjectItem(_animationJSON, "currentFrame")->valueint;
	// Animation Frames
	_animation->animationFrames = (uint8_t)cJSON_GetObjectItem(_animationJSON, "animationFrames")->valueint;
	// Loop
	_animation->loop = cJSON_GetObjectItem(_animationJSON, "loop")->valueint;

}

// camera
void AF_JSON_JsonToCamera(cJSON* _cameraJSON, AF_CCamera* _camera) {
	if (_cameraJSON == NULL || _camera == NULL) {
		AF_Log_Error("AF_JSON_JsonToCamera: Invalid JSON or camera pointer.\n");
		return;
	}

	// Has
	_camera->enabled = AF_Component_SetHas(_camera->enabled, cJSON_GetObjectItem(_cameraJSON, "has")->valueint);

	// Enabled
	_camera->enabled = AF_Component_SetEnabled(_camera->enabled, cJSON_GetObjectItem(_cameraJSON, "enabled")->valueint);

	// yaw
	_camera->yaw = (AF_FLOAT)cJSON_GetObjectItem(_cameraJSON, "yaw")->valuedouble;
	// pitch
	_camera->pitch = (AF_FLOAT)cJSON_GetObjectItem(_cameraJSON, "pitch")->valuedouble;

	// FOV
	cJSON* fovJSON = cJSON_GetObjectItem(_cameraJSON, "fov");
	if (fovJSON != NULL) {
		_camera->fov = (AF_FLOAT)cJSON_GetNumberValue(fovJSON);
	}
	else {
		_camera->fov = 45.0f; // Default value if not found
	}

	// Near Plane
	cJSON* nearPlaneJSON = cJSON_GetObjectItem(_cameraJSON, "nearPlane");
	if (nearPlaneJSON != NULL) {
		_camera->nearPlane = (AF_FLOAT)cJSON_GetNumberValue(nearPlaneJSON);
	}
	else {
		_camera->nearPlane = 0.1f; // Default value if not found
	}

	// Far Plane
	cJSON* farPlaneJSON = cJSON_GetObjectItem(_cameraJSON, "farPlane");
	if (farPlaneJSON != NULL) {
		_camera->farPlane = (AF_FLOAT)cJSON_GetNumberValue(farPlaneJSON);
	}
	else {
		_camera->farPlane = 1000.0f; // Default value if not found
	}
	// Aspect Ratio
	cJSON* aspectRatioJSON = cJSON_GetObjectItem(_cameraJSON, "aspectRatio");
	if (aspectRatioJSON != NULL) {
		_camera->aspectRatio = (AF_FLOAT)cJSON_GetNumberValue(aspectRatioJSON);
	}
	else {
		_camera->aspectRatio = 16.0f / 9.0f; // Default value if not found
	}
	// Orthographic
	_camera->orthographic = cJSON_GetObjectItem(_cameraJSON, "orthographic")->valueint;
	// Projection Matrix skip for now, it will be updated later by the renderer

	// View Matrix skip for now, it will be updated later by the renderer

	// Background Color
	cJSON* backgroundColorJSON = cJSON_GetObjectItem(_cameraJSON, "backgroundColor");
	if (backgroundColorJSON != NULL) {
		_camera->backgroundColor.x = cJSON_GetArrayItem(backgroundColorJSON, 0)->valuedouble;
		_camera->backgroundColor.y = cJSON_GetArrayItem(backgroundColorJSON, 1)->valuedouble;
		_camera->backgroundColor.z = cJSON_GetArrayItem(backgroundColorJSON, 2)->valuedouble;
		_camera->backgroundColor.w = cJSON_GetArrayItem(backgroundColorJSON, 3)->valuedouble;
	}
	else {
		// Default background color if not provided
		Vec4 backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f}; // Assuming black
		_camera->backgroundColor = backgroundColor; // Assuming black with full opacity
	}
}

// mesh
void AF_JSON_JsonToMesh(cJSON* _meshJSON, AF_CMesh* _mesh) {
	if (_meshJSON == NULL || _mesh == NULL) {
		AF_Log_Error("AF_JSON_JsonToMesh: Invalid JSON or mesh pointer.\n");
		return;
	}

	// Has
	_mesh->enabled = AF_Component_SetHas(_mesh->enabled, cJSON_GetObjectItem(_meshJSON, "has")->valueint);

	// Enabled
	_mesh->enabled = AF_Component_SetEnabled(_mesh->enabled, cJSON_GetObjectItem(_meshJSON, "enabled")->valueint);


	// meshCount
	_mesh->meshCount = (uint32_t)cJSON_GetObjectItem(_meshJSON, "meshCount")->valueint;
	// showDebug
	_mesh->showDebug = cJSON_GetObjectItem(_meshJSON, "showDebug")->valueint;
	// meshType
	_mesh->meshType = (enum AF_MESH_TYPE)cJSON_GetObjectItem(_meshJSON, "meshType")->valueint;
	// meshPath
	cJSON* meshPathJSON = cJSON_GetObjectItem(_meshJSON, "meshPath");
	if (meshPathJSON != NULL && cJSON_IsString(meshPathJSON)) {
		snprintf(_mesh->meshPath, sizeof(_mesh->meshPath) - 1, "%s", meshPathJSON->valuestring);
		//strncpy(_mesh->meshPath, meshPathJSON->valuestring, sizeof(_mesh->meshPath) - 1);
		_mesh->meshPath[sizeof(_mesh->meshPath) - 1] = '\0'; // Ensure null termination
	}
	else {
		_mesh->meshPath[0] = '\0'; // Set to empty string if not found or not a string
	}

	// shader skipp for now, it will be loaded later when the mesh is used

	// isImageFlipped
	_mesh->isImageFlipped = cJSON_GetObjectItem(_meshJSON, "isImageFlipped")->valueint;

	// mesh id skipp for now
	cJSON* shaderJSON = cJSON_GetObjectItem(_meshJSON, "shader");

	cJSON* shaderFragPathJSON = cJSON_GetObjectItem(shaderJSON, "fragmentShader");

	cJSON* shaderVertPathJSON = cJSON_GetObjectItem(shaderJSON, "vertexShader");

	// save the shader paths
	snprintf(_mesh->shader.fragPath, sizeof(_mesh->shader.vertPath), "%s", shaderFragPathJSON->valuestring);
	snprintf(_mesh->shader.vertPath, sizeof(_mesh->shader.vertPath), "%s", shaderVertPathJSON->valuestring);

	// Material
	cJSON* materialJson = cJSON_GetObjectItem(_meshJSON, "material");

	//diffuse texture
	cJSON* diffuseTexturePathJson = cJSON_GetObjectItem(materialJson, "diffuseTexture");
	snprintf(_mesh->material.diffuseTexture.path, AF_MAX_PATH_CHAR_SIZE, "%s", diffuseTexturePathJson->valuestring);
	_mesh->material.diffuseTexture.type = AF_TEXTURE_TYPE_DIFFUSE;

	//specular texture
	cJSON* specularTexturePathJson = cJSON_GetObjectItem(materialJson, "specularTexture");
	snprintf(_mesh->material.specularTexture.path, AF_MAX_PATH_CHAR_SIZE, "%s", diffuseTexturePathJson->valuestring);
	_mesh->material.specularTexture.type = AF_TEXTURE_TYPE_SPECULAR;

	//normal texture
	cJSON* normalTexturePathJson = cJSON_GetObjectItem(materialJson, "normalTexture");
	snprintf(_mesh->material.normalTexture.path, AF_MAX_PATH_CHAR_SIZE, "%s", diffuseTexturePathJson->valuestring);
	_mesh->material.normalTexture.type = AF_TEXTURE_TYPE_NORMALS;

	//snprintf(_mesh->material.normalTexture.path, AF_MAX_PATH_CHAR_SIZE, "%s", normalTexturePathJson->valuestring);

	// Color
	cJSON* colorJson = cJSON_GetObjectItem(materialJson, "color");
	_mesh->material.color.r = (uint8_t)cJSON_GetArrayItem(colorJson, 0)->valueint;
	_mesh->material.color.g = (uint8_t)cJSON_GetArrayItem(colorJson, 1)->valueint;
	_mesh->material.color.b = (uint8_t)cJSON_GetArrayItem(colorJson, 2)->valueint;
	_mesh->material.color.a = (uint8_t)cJSON_GetArrayItem(colorJson, 3)->valueint;


	//shininess
	cJSON* shininessJson = cJSON_GetObjectItem(materialJson, "shininess");
	_mesh->material.shininess = shininessJson->valuedouble;


	// isAnimating
	_mesh->isAnimating = cJSON_GetObjectItem(_meshJSON, "isAnimating")->valueint;

	// textured
	_mesh->textured = cJSON_GetObjectItem(_meshJSON, "textured")->valueint;

	// transparent
	_mesh->transparent = cJSON_GetObjectItem(_meshJSON, "transparent")->valueint;
	// recieveLights
	_mesh->recieveLights = cJSON_GetObjectItem(_meshJSON, "recieveLights")->valueint;
	// recieveShadows
	_mesh->recieveShadows = cJSON_GetObjectItem(_meshJSON, "recieveShadows")->valueint;
	// castShadows
	_mesh->castShadows = cJSON_GetObjectItem(_meshJSON, "castShadows")->valueint;
}

// text
void AF_JSON_JsonToText(cJSON* _textJSON, AF_CText* _text) {
	if (_textJSON == NULL || _text == NULL) {
		AF_Log_Error("AF_JSON_JsonToText: Invalid JSON or text pointer.\n");
		return;
	}
	// Has
	_text->enabled = AF_Component_SetHas(_text->enabled, cJSON_GetObjectItem(_textJSON, "has")->valueint);
	// Enabled
	_text->enabled = AF_Component_SetEnabled(_text->enabled, cJSON_GetObjectItem(_textJSON, "enabled")->valueint);
	// isDirty
	_text->isDirty = cJSON_GetObjectItem(_textJSON, "isDirty")->valueint;
	// isShowing
	_text->isShowing = cJSON_GetObjectItem(_textJSON, "isShowing")->valueint;
	// fontID
	_text->fontID = (uint8_t)cJSON_GetObjectItem(_textJSON, "fontID")->valueint;
	// fontPath
	cJSON* fontPathJSON = cJSON_GetObjectItem(_textJSON, "fontPath");
	if (fontPathJSON != NULL && cJSON_IsString(fontPathJSON)) {

		snprintf(_text->fontPath, AF_MAX_PATH_CHAR_SIZE, "%s", fontPathJSON->valuestring); // Copy the string to the fontPath
	}

	// text
	cJSON* textJSON = cJSON_GetObjectItem(_textJSON, "text");
	if (textJSON != NULL && cJSON_IsString(textJSON)) {
		snprintf(_text->text, AF_MAX_PATH_CHAR_SIZE, "%s", textJSON->valuestring); // Copy the string to the text field
	}

	// screenPos
	cJSON* screenPosJSON = cJSON_GetObjectItem(_textJSON, "screenPos");
	if (screenPosJSON != NULL) {
		_text->screenPos.x = cJSON_GetArrayItem(screenPosJSON, 0)->valuedouble;
		_text->screenPos.y = cJSON_GetArrayItem(screenPosJSON, 1)->valuedouble;
	}
	// textBounds
	cJSON* textBoundsJSON = cJSON_GetObjectItem(_textJSON, "textBounds");
	if (textBoundsJSON != NULL) {
		_text->textBounds.x = cJSON_GetArrayItem(textBoundsJSON, 0)->valuedouble;
		_text->textBounds.y = cJSON_GetArrayItem(textBoundsJSON, 1)->valuedouble;
	}
	// textColor
	cJSON* textColorJSON = cJSON_GetObjectItem(_textJSON, "textColor");
	if (textColorJSON != NULL) {
		_text->textColor[0] = (uint8_t)cJSON_GetArrayItem(textColorJSON, 0)->valueint;
		_text->textColor[1] = (uint8_t)cJSON_GetArrayItem(textColorJSON, 1)->valueint;
		_text->textColor[2] = (uint8_t)cJSON_GetArrayItem(textColorJSON, 2)->valueint;
		_text->textColor[3] = (uint8_t)cJSON_GetArrayItem(textColorJSON, 3)->valueint;
	}
	else {
		// Default text color if not provided
		_text->textColor[0] = 255; // White
		_text->textColor[1] = 255; // White
		_text->textColor[2] = 255; // White
		_text->textColor[3] = 255; // Full opacity
	}

}

// audio source
void AF_JSON_JsonToAudioSource(cJSON* _audioSourceJSON, AF_CAudioSource* _audioSource) {
	if (_audioSourceJSON == NULL || _audioSource == NULL) {
		AF_Log_Error("AF_JSON_JsonToAudioSource: Invalid JSON or audio source pointer.\n");
		return;
	}
	// Has
	_audioSource->enabled = AF_Component_SetHas(_audioSource->enabled, cJSON_GetObjectItem(_audioSourceJSON, "has")->valueint);
	// Enabled
	_audioSource->enabled = AF_Component_SetEnabled(_audioSource->enabled, cJSON_GetObjectItem(_audioSourceJSON, "enabled")->valueint);
	// Clip skipp for now
	// Channel
	_audioSource->channel = (uint8_t)cJSON_GetObjectItem(_audioSourceJSON, "channel")->valueint;
	// Loop
	_audioSource->loop = cJSON_GetObjectItem(_audioSourceJSON, "loop")->valueint;
	// Is Playing
	_audioSource->isPlaying = cJSON_GetObjectItem(_audioSourceJSON, "isPlaying")->valueint;

}

// player data
void AF_JSON_JsonToPlayerData(cJSON* _playerDataJSON, AF_CPlayerData* _playerData) {
	if (_playerDataJSON == NULL || _playerData == NULL) {
		AF_Log_Error("AF_JSON_JsonToPlayerData: Invalid JSON or player data pointer.\n");
		return;
	}
	// Has
	_playerData->enabled = AF_Component_SetHas(_playerData->enabled, cJSON_GetObjectItem(_playerDataJSON, "has")->valueint);
	// Enabled
	_playerData->enabled = AF_Component_SetEnabled(_playerData->enabled, cJSON_GetObjectItem(_playerDataJSON, "enabled")->valueint);
	// isHuman
	_playerData->isHuman = cJSON_GetObjectItem(_playerDataJSON, "isHuman")->valueint;
	// isCarrying
	_playerData->isCarrying = cJSON_GetObjectItem(_playerDataJSON, "isCarrying")->valueint;
	// isCarried
	_playerData->isCarried = cJSON_GetObjectItem(_playerDataJSON, "isCarried")->valueint;
	// carryingEntity
	_playerData->carryingEntity = cJSON_GetObjectItem(_playerDataJSON, "carryingEntity")->valueint;
	// health
	_playerData->health = (float)cJSON_GetObjectItem(_playerDataJSON, "health")->valuedouble;
	// isAlive
	_playerData->isAlive = cJSON_GetObjectItem(_playerDataJSON, "isAlive")->valueint;
	// isAttacking
	_playerData->isAttacking = cJSON_GetObjectItem(_playerDataJSON, "isAttacking")->valueint;
	// isJumping
	_playerData->isJumping = cJSON_GetObjectItem(_playerDataJSON, "isJumping")->valueint;
	// movementSpeed
	_playerData->movementSpeed = (float)cJSON_GetObjectItem(_playerDataJSON, "movementSpeed")->valuedouble;
	// score
	_playerData->score = cJSON_GetObjectItem(_playerDataJSON, "score")->valueint;
	// startPosition
	cJSON* startPositionJSON = cJSON_GetObjectItem(_playerDataJSON, "startPosition");
	if (startPositionJSON != NULL) {
		_playerData->startPosition.x = cJSON_GetArrayItem(startPositionJSON, 0)->valuedouble;
		_playerData->startPosition.y = cJSON_GetArrayItem(startPositionJSON, 1)->valuedouble;
		_playerData->startPosition.z = cJSON_GetArrayItem(startPositionJSON, 2)->valuedouble;
	}
	// targetDestination
	cJSON* targetDestinationJSON = cJSON_GetObjectItem(_playerDataJSON, "targetDestination");
	if (targetDestinationJSON != NULL) {
		_playerData->targetDestination.x = cJSON_GetArrayItem(targetDestinationJSON, 0)->valuedouble;
		_playerData->targetDestination.y = cJSON_GetArrayItem(targetDestinationJSON, 1)->valuedouble;
		_playerData->targetDestination.z = cJSON_GetArrayItem(targetDestinationJSON, 2)->valuedouble;
	}
	// spawnTime
	_playerData->spawnTime = (float)cJSON_GetObjectItem(_playerDataJSON, "spawnTime")->valuedouble;
	// faction
	_playerData->faction = (enum PLAYER_FACTION)cJSON_GetObjectItem(_playerDataJSON, "faction")->valueint;
}

// skeletal animation
void AF_JSON_JsonToSkeletalAnimation(cJSON* _skeletalAnimationJSON, AF_CSkeletalAnimation* _skeletalAnimation) {
	// check null pointers
	if (_skeletalAnimationJSON == NULL || _skeletalAnimation == NULL) {
		AF_Log_Error("AF_JSON_JsonToSkeletalAnimation: Invalid JSON or skeletal animation pointer.\n");
		return;
	}

	// Has
	_skeletalAnimation->enabled = AF_Component_SetHas(_skeletalAnimation->enabled, cJSON_GetObjectItem(_skeletalAnimationJSON, "has")->valueint);
	// Enabled
	_skeletalAnimation->enabled = AF_Component_SetEnabled(_skeletalAnimation->enabled, cJSON_GetObjectItem(_skeletalAnimationJSON, "enabled")->valueint);


	// Animation Paths
	cJSON* animIdlePathJSON = cJSON_GetObjectItem(_skeletalAnimationJSON, "animIdlePath");
	if (animIdlePathJSON != NULL && cJSON_IsString(animIdlePathJSON)) {

		snprintf(_skeletalAnimation->animIdlePath, AF_MAX_PATH_CHAR_SIZE, "%s", animIdlePathJSON->valuestring); // Copy the string to the animIdlePath
	}

	cJSON* animWalkPathJSON = cJSON_GetObjectItem(_skeletalAnimationJSON, "animWalkPath");
	if (animWalkPathJSON != NULL && cJSON_IsString(animWalkPathJSON)) {
		snprintf(_skeletalAnimation->animWalkPath, AF_MAX_PATH_CHAR_SIZE, "%s", animWalkPathJSON->valuestring); // Copy the string to the animWalkPath
	}

	cJSON* animAttackPathJSON = cJSON_GetObjectItem(_skeletalAnimationJSON, "animAttackPath");
	if (animAttackPathJSON != NULL && cJSON_IsString(animAttackPathJSON)) {
		snprintf(_skeletalAnimation->animAttackPath, AF_MAX_PATH_CHAR_SIZE, "%s", animAttackPathJSON->valuestring); // Copy the string to the animAttackPath
	}

	// animationSpeed
	_skeletalAnimation->animationSpeed = (AF_FLOAT)cJSON_GetObjectItem(_skeletalAnimationJSON, "animationSpeed")->valuedouble;
	// animationBlend
	_skeletalAnimation->animationBlend = (AF_FLOAT)cJSON_GetObjectItem(_skeletalAnimationJSON, "animationBlend")->valuedouble;
	// animationTypeID
	_skeletalAnimation->animationTypeID = (enum AnimationType)cJSON_GetObjectItem(_skeletalAnimationJSON, "animationTypeID")->valueint;
	// nextFrameTime
	_skeletalAnimation->nextFrameTime = (AF_FLOAT)cJSON_GetObjectItem(_skeletalAnimationJSON, "nextFrameTime")->valuedouble; // 16 or 32 bytes
	// currentFrame
	_skeletalAnimation->currentFrame = (uint8_t)cJSON_GetObjectItem(_skeletalAnimationJSON, "currentFrame")->valueint; // 8 bytes
	// animationFrames
	_skeletalAnimation->animationFrames = (uint8_t)cJSON_GetObjectItem(_skeletalAnimationJSON, "animationFrames")->valueint; // 8 bytes
	// loop
	_skeletalAnimation->loop = cJSON_GetObjectItem(_skeletalAnimationJSON, "loop")->valueint; // 8 bytes
}

// AI Behaviour
void AF_JSON_JsonToAIBehaviour(cJSON* _aiBehaviourJSON, AF_CAI_Behaviour* _aiBehaviour) {
	if (_aiBehaviourJSON == NULL || _aiBehaviour == NULL) {
		AF_Log_Error("AF_JSON_JsonToAIBehaviour: Invalid JSON or AI Behaviour pointer.\n");
		return;
	}
	// Has
	_aiBehaviour->enabled = AF_Component_SetHas(_aiBehaviour->enabled, cJSON_GetObjectItem(_aiBehaviourJSON, "has")->valueint);
	// Enabled
	_aiBehaviour->enabled = AF_Component_SetEnabled(_aiBehaviour->enabled, cJSON_GetObjectItem(_aiBehaviourJSON, "enabled")->valueint);

	// actionsArray skipped

	// nextAvailableActionSlot
	_aiBehaviour->nextAvailableActionSlot = (uint8_t)cJSON_GetObjectItem(_aiBehaviourJSON, "nextAvailableActionSlot")->valueint;
}

// input controller
void AF_JSON_JsonToInputController(cJSON* _inputControllerJSON, AF_CInputController* _inputController) {
	// null pointer check
	if (_inputControllerJSON == NULL || _inputController == NULL) {
		AF_Log_Error("AF_JSON_JsonToInputController: Invalid JSON or input controller pointer.\n");
		return;
	}

	// not implementing as this is depricated
}

// scripts
void AF_JSON_JsonToScripts(cJSON* _scriptsJSON, AF_CScript* _scripts) {
	if (_scriptsJSON == NULL || _scripts == NULL) {
		AF_Log_Error("AF_JSON_JsonToScripts: Invalid JSON or script pointer.\n");
		return;
	}
	// Has
	uint32_t jsonHasValue = cJSON_GetObjectItem(_scriptsJSON, "has")->valueint;
	if(jsonHasValue == 1){
		_scripts->enabled = AF_Component_SetHas(_scripts->enabled, AF_TRUE);
	}
	
	// Enabled
	uint32_t jsonEnabledValue = cJSON_GetObjectItem(_scriptsJSON, "enabled")->valueint;
	if(jsonEnabledValue == 1){
		_scripts->enabled = AF_Component_SetEnabled(_scripts->enabled, AF_TRUE);
	}

	if(AF_Component_GetHasEnabled(_scripts->enabled) == AF_FALSE) {
		//AF_Log_Warn("AF_JSON_JsonToScripts: Script component is not enabled, skipping further processing.\n");
		return; // If the script is not enabled, skip further processing
	}

	// for each script in the array

	// Script Name
	cJSON* scriptNameJSON = cJSON_GetObjectItem(_scriptsJSON, "scriptName");
	if (scriptNameJSON != NULL && cJSON_IsString(scriptNameJSON)) {
		snprintf(_scripts->scriptName, sizeof(_scripts->scriptName) - 1, "%s", scriptNameJSON->valuestring);
		//strncpy(_scripts->scriptName, scriptNameJSON->valuestring, sizeof(_scripts->scriptName) - 1);
		_scripts->scriptName[sizeof(_scripts->scriptName) - 1] = '\0'; // Ensure null termination
	}
	else {
		_scripts->scriptName[0] = '\0'; // Set to empty string if not found or not a string
	}

	// Script Full Path
	cJSON* scriptFullPathJSON = cJSON_GetObjectItem(_scriptsJSON, "scriptFullPath");
	if (scriptFullPathJSON != NULL && cJSON_IsString(scriptFullPathJSON)) {
		snprintf(_scripts->scriptFullPath, sizeof(_scripts->scriptName) - 1, "%s", scriptFullPathJSON->valuestring);
		//strncpy(_scripts->scriptFullPath, scriptFullPathJSON->valuestring, sizeof(_scripts->scriptFullPath) - 1);
		_scripts->scriptFullPath[sizeof(_scripts->scriptFullPath) - 1] = '\0'; // Ensure null termination
	}
	else {
		_scripts->scriptFullPath[0] = '\0'; // Set to empty string if not found or not a string
	}

	// function pointer skipped as we setup this automatically
}

// 
void AF_JSON_JsonToLight(cJSON* _lightJSON, AF_CLight* _light) {
	// null pointer check
	if (_lightJSON == NULL || _light == NULL) {
		AF_Log_Error("AF_JSON_JsonToLight: Invalid JSON or light pointer.\n");
		return;
	}
	// Has
	_light->enabled = AF_Component_SetHas(_light->enabled, cJSON_GetObjectItem(_lightJSON, "has")->valueint);
	// Enabled
	_light->enabled = AF_Component_SetEnabled(_light->enabled, cJSON_GetObjectItem(_lightJSON, "enabled")->valueint);
	// Light Type
	_light->lightType = (enum AF_Light_Type_e)cJSON_GetObjectItem(_lightJSON, "lightType")->valueint;
	// Direction
	cJSON* directionJSON = cJSON_GetObjectItem(_lightJSON, "direction");
	if (directionJSON != NULL) {
		_light->direction.x = cJSON_GetArrayItem(directionJSON, 0)->valuedouble;
		_light->direction.y = cJSON_GetArrayItem(directionJSON, 1)->valuedouble;
		_light->direction.z = cJSON_GetArrayItem(directionJSON, 2)->valuedouble;
	}
	else {
		Vec3 direction = { 0.0f, -1.0f, 0.0f }; // Default direction if not provided
		_light->direction = direction; // Default direction if not provided
	}
	// Ambient Color
	cJSON* ambientColJSON = cJSON_GetObjectItem(_lightJSON, "ambientCol");
	if (ambientColJSON != NULL) {
		_light->ambientCol.x = cJSON_GetArrayItem(ambientColJSON, 0)->valuedouble;
		_light->ambientCol.y = cJSON_GetArrayItem(ambientColJSON, 1)->valuedouble;
		_light->ambientCol.z = cJSON_GetArrayItem(ambientColJSON, 2)->valuedouble;
	}
	else {
		Vec3 ambientCol = { 0.1f, 0.1f, 0.1f }; // Default ambient color if not provided
		_light->ambientCol = ambientCol; // Default ambient color if not provided
	}
	// Diffuse Color
	cJSON* diffuseColJSON = cJSON_GetObjectItem(_lightJSON, "diffuseCol");
	if (diffuseColJSON != NULL) {
		_light->diffuseCol.x = cJSON_GetArrayItem(diffuseColJSON, 0)->valuedouble;
		_light->diffuseCol.y = cJSON_GetArrayItem(diffuseColJSON, 1)->valuedouble;
		_light->diffuseCol.z = cJSON_GetArrayItem(diffuseColJSON, 2)->valuedouble;
	}
	else {
		Vec3 diffuseCol = { 1.0f, 1.0f, 1.0f }; // Default diffuse color if not provided
		_light->diffuseCol = diffuseCol; // Default diffuse color if not provided
	}
	// Specular Color
	cJSON* specularColJSON = cJSON_GetObjectItem(_lightJSON, "specularCol");
	if (specularColJSON != NULL) {
		_light->specularCol.x = cJSON_GetArrayItem(specularColJSON, 0)->valuedouble;
		_light->specularCol.y = cJSON_GetArrayItem(specularColJSON, 1)->valuedouble;
		_light->specularCol.z = cJSON_GetArrayItem(specularColJSON, 2)->valuedouble;
	}
	else {
		Vec3 specularCol = { 1.0f, 1.0f, 1.0f }; // Default specular color if not provided
		_light->specularCol = specularCol; // Default specular color if not provided
	}
	// Constant
	cJSON* constantJSON = cJSON_GetObjectItem(_lightJSON, "constant");
	if (constantJSON != NULL) {
		_light->constant = (AF_FLOAT)cJSON_GetNumberValue(constantJSON);
	}
	else {
		_light->constant = 1.0f; // Default value if not found
	}
	// Linear
	cJSON* linearJSON = cJSON_GetObjectItem(_lightJSON, "linear");
	if (linearJSON != NULL) {
		_light->linear = (AF_FLOAT)cJSON_GetNumberValue(linearJSON);
	}
	else {
		_light->linear = 0.09f; // Default value if not found
	}
	// Quadratic
	cJSON* quadraticJSON = cJSON_GetObjectItem(_lightJSON, "quadratic");
	if (quadraticJSON != NULL) {
		_light->quadratic = (AF_FLOAT)cJSON_GetNumberValue(quadraticJSON);
	}
	else {
		_light->quadratic = 0.032f; // Default value if not found
	}
	// Cut Off
	cJSON* cutOffJSON = cJSON_GetObjectItem(_lightJSON, "cutOff");
	if (cutOffJSON != NULL) {
		_light->cutOff = (AF_FLOAT)cJSON_GetNumberValue(cutOffJSON);
	}
	else {
		_light->cutOff = 0.5f; // Default value if not found
	}
	// Outer Cutoff
	cJSON* outerCutoffJSON = cJSON_GetObjectItem(_lightJSON, "outerCutoff");
	if (outerCutoffJSON != NULL) {
		_light->outerCutoff = (AF_FLOAT)cJSON_GetNumberValue(outerCutoffJSON);
	}
	else {
		_light->outerCutoff = 0.7f; // Default value if not found
	}
}

// editor data component
void AF_JSON_JsonToEditorData(cJSON* _editorDataJSON, AF_CEditorData* _editorData) {

	if (_editorDataJSON != NULL) {
		// Populate editor data from JSON
		_editorData->enabled = AF_Component_SetEnabled(_editorData->enabled, cJSON_GetObjectItem(_editorDataJSON, "enabled")->valueint);
		_editorData->enabled = AF_Component_SetHas(_editorData->enabled, cJSON_GetObjectItem(_editorDataJSON, "has")->valueint);
		cJSON* nameJSON = cJSON_GetObjectItem(_editorDataJSON, "name");
		if (nameJSON != NULL) {
			snprintf(_editorData->name, sizeof(_editorData->name) - 1, "%s", nameJSON->valuestring);
			//strncpy(_editorData->name, nameJSON->valuestring, );
			_editorData->name[sizeof(_editorData->name) - 1] = '\0'; // Ensure null termination
		}
	}
}

// COMPONENTS TO JSON CONVERTERS

cJSON* AF_JSON_TransformToJson(AF_CTransform3D* _transform) {
	cJSON* transformJSON = cJSON_CreateObject();

	// Has
	af_bool_t has = AF_Component_GetHas(_transform->enabled);
	cJSON_AddNumberToObject(transformJSON, "has", has);

	// Enabled
	af_bool_t enabled = AF_Component_GetEnabled(_transform->enabled);
	cJSON_AddNumberToObject(transformJSON, "enabled", enabled);

	// Pos
	Vec3 pos = _transform->pos;
	AF_JSON_Vec3ToJson("pos", &pos, transformJSON);

	// Local Pos
	Vec3 localPos = _transform->localPos;
	AF_JSON_Vec3ToJson("localPos", &localPos, transformJSON);

	// Rot
	Vec3 rot = _transform->rot;
	AF_JSON_Vec3ToJson("rot", &rot, transformJSON);

	// LocalRot
	Vec3 localRot = _transform->localRot;
	AF_JSON_Vec3ToJson("localRot", &localRot, transformJSON);

	// Scale
	Vec3 scale = _transform->scale;
	AF_JSON_Vec3ToJson("scale", &scale, transformJSON);

	// LocalScale
	Vec3 localScale = _transform->localScale;
	AF_JSON_Vec3ToJson("localScale", &localScale, transformJSON);

	// Orientation
	Vec4 orientation = _transform->orientation;
	AF_JSON_Vec4ToJson("orientation", &orientation, transformJSON);

	// Model Mat
	Mat4 modelMat = _transform->modelMat;
	AF_JSON_Mat4ToJson("modelMatrix", &modelMat, transformJSON);

	return transformJSON;
}

cJSON* AF_JSON_SpriteToJson(AF_CSprite* _sprite) {
	// sprite json
	cJSON* spriteJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_sprite->enabled);
	cJSON_AddNumberToObject(spriteJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_sprite->enabled);
	cJSON_AddNumberToObject(spriteJSON, "enabled", enabled);

	// loop
	af_bool_t loop = _sprite->loop;
	cJSON_AddNumberToObject(spriteJSON, "loop", loop);

	// current frame
	char currentFrame = _sprite->currentFrame;
	cJSON_AddNumberToObject(spriteJSON, "currentFrame", currentFrame);

	// animation frames
	char animtionFrames = _sprite->animationFrames;
	cJSON_AddNumberToObject(spriteJSON, "animationFrames", animtionFrames);

	// current frame time
	AF_FLOAT currentFrameTime = _sprite->currentFrameTime;
	cJSON_AddNumberToObject(spriteJSON, "currentFrameTime", currentFrameTime);

	// next frame time
	AF_FLOAT nextFrameTime = _sprite->nextFrameTime;     // 4 bytes
	cJSON_AddNumberToObject(spriteJSON, "nextFrameTime", nextFrameTime);

	// animation speed
	AF_FLOAT animationSpeed = _sprite->animationSpeed;    // 4 bytes
	cJSON_AddNumberToObject(spriteJSON, "animationSpeed", animationSpeed);

	// sprite pos
	Vec2 spritePos = _sprite->spritePos;		    // 8 bytes
	AF_JSON_Vec2ToJson("spritePos", &spritePos, spriteJSON);

	// sprite size
	Vec2 spriteSize = _sprite->spriteSize;    	// size of sprite in pixels
	AF_JSON_Vec2ToJson("spriteSize", &spriteSize, spriteJSON);

	// sprite scale
	Vec2 spriteScale = _sprite->spriteScale;		// transform scale
	AF_JSON_Vec2ToJson("spriteScale", &spriteScale, spriteJSON);

	// sprite rotations
	float spriteRotation = _sprite->spriteRotation;	// rotation
	cJSON_AddNumberToObject(spriteJSON, "spriteRotation", spriteRotation);

	// flip
	af_bool_t flipX = _sprite->flipX;
	cJSON_AddNumberToObject(spriteJSON, "flipX", flipX);
	af_bool_t flipY = _sprite->flipY;
	cJSON_AddNumberToObject(spriteJSON, "flipY", flipY);

	// sprite sheet size
	Vec2 spriteSheetSize = _sprite->spriteSheetSize;    // 8 bytes
	AF_JSON_Vec2ToJson("spriteSheetSize", &spriteSheetSize, spriteJSON);

	// sprite color
	Vec4 spriteColor = { (AF_FLOAT)_sprite->spriteColor[0], (AF_FLOAT)_sprite->spriteColor[1], (AF_FLOAT)_sprite->spriteColor[2], (AF_FLOAT)_sprite->spriteColor[3] };
	AF_JSON_Vec4ToJson("spriteColo", &spriteColor, spriteJSON);

	// sprite path
	const char* spritePath = _sprite->spritePath;
	cJSON_AddStringToObject(spriteJSON, "animationSpeed", spritePath);

	// sprite data
	void* spriteData = _sprite->spriteData; // special ptr for sprite data to be cast when known
	cJSON_AddNullToObject(spriteJSON, "spriteData");

	// is playing
	af_bool_t isPlaying = _sprite->isPlaying;
	cJSON_AddNumberToObject(spriteJSON, "isPlaying", isPlaying);

	// filtering
	af_bool_t filtering = _sprite->filtering;
	cJSON_AddNumberToObject(spriteJSON, "filtering", filtering);

	return spriteJSON;
}

cJSON* AF_JSON_RigidbodyToJson(AF_C3DRigidbody* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// Kinematic
	cJSON_AddNumberToObject(returnJSON, "isKinematic", _component->isKinematic);

	// Gravity
	cJSON_AddNumberToObject(returnJSON, "gravity", _component->gravity);

	// velocity
	AF_JSON_Vec3ToJson("velocity", &_component->velocity, returnJSON);

	// angular velocity
	AF_JSON_Vec3ToJson("anglularVelocity", &_component->anglularVelocity, returnJSON);

	// inverse mass
	cJSON_AddNumberToObject(returnJSON, "inverseMass", _component->inverseMass);

	// force
	AF_JSON_Vec3ToJson("force", &_component->force, returnJSON);

	// torque
	AF_JSON_Vec3ToJson("torque", &_component->torque, returnJSON);

	// inertiaTensor
	AF_JSON_Vec3ToJson("inertiaTensor", &_component->inertiaTensor, returnJSON);

	return returnJSON;
}

cJSON* AF_JSON_ColliderToJson(AF_CCollider* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// collision type
	cJSON_AddNumberToObject(returnJSON, "type", _component->type);

	// bounding volume
	AF_JSON_Vec3ToJson("boundingVolume", &_component->boundingVolume, returnJSON);

	// pos
	AF_JSON_Vec3ToJson("pos", &_component->pos, returnJSON);

	// collision
	cJSON_AddNullToObject(returnJSON, "collision");
	//AF_JSON_Vec3ToJson("collision", &_component->collision, returnJSON);

	// showDebug
	cJSON_AddNumberToObject(returnJSON, "showDebug", _component->showDebug);

	// broadphaseAABB
	Vec3 broadphaseAABB;
	AF_JSON_Vec3ToJson("broadphaseAABB", &_component->broadphaseAABB, returnJSON);

	return returnJSON;
}

cJSON* AF_JSON_AnimationToJson(AF_CAnimation* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// animation speed
	cJSON_AddNumberToObject(returnJSON, "animationSpeed", _component->animationSpeed);

	// next frame time
	cJSON_AddNumberToObject(returnJSON, "nextFrameTime", _component->nextFrameTime);


	// current frame
	cJSON_AddNumberToObject(returnJSON, "currentFrame", _component->currentFrame);

	// animation frames
	cJSON_AddNumberToObject(returnJSON, "animationFrames", _component->animationFrames);

	// loop
	cJSON_AddNumberToObject(returnJSON, "loop", _component->loop);

	return returnJSON;
}

cJSON* AF_JSON_CameraToJson(AF_CCamera* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// camera front
	AF_JSON_Vec3ToJson("cameraFront", &_component->cameraFront, returnJSON);

	// camera up
	AF_JSON_Vec3ToJson("cameraUp", &_component->cameraUp, returnJSON);

	// camera right
	AF_JSON_Vec3ToJson("cameraRight", &_component->cameraRight, returnJSON);

	// camera world up
	AF_JSON_Vec3ToJson("cameraWorldUp", &_component->cameraWorldUp, returnJSON);

	// yaw
	cJSON_AddNumberToObject(returnJSON, "yaw", _component->yaw);

	// pitch
	cJSON_AddNumberToObject(returnJSON, "pitch", _component->pitch);

	// pov
	cJSON_AddNumberToObject(returnJSON, "fov", _component->fov);

	// near plane
	cJSON_AddNumberToObject(returnJSON, "nearPlane", _component->nearPlane);

	// far plane
	cJSON_AddNumberToObject(returnJSON, "farPlane", _component->farPlane);

	// aspect ratio
	cJSON_AddNumberToObject(returnJSON, "aspectRatio", _component->aspectRatio);

	// tan half fov
	cJSON_AddNumberToObject(returnJSON, "tanHalfFov", _component->tanHalfFov);

	// randge inv
	cJSON_AddNumberToObject(returnJSON, "rangeInv", _component->rangeInv);

	// orthographic
	cJSON_AddNumberToObject(returnJSON, "orthographic", _component->orthographic);

	// projection matrix
	AF_JSON_Mat4ToJson("projectionMatrix", &_component->projectionMatrix, returnJSON);

	// view matrix
	AF_JSON_Mat4ToJson("viewMatrix", &_component->viewMatrix, returnJSON);

	// background color
	AF_JSON_Vec4ToJson("backgroundColor", &_component->backgroundColor, returnJSON);

	return returnJSON;
}

cJSON* AF_JSON_MeshToJson(AF_CMesh* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// mesh
	AF_MeshData meshes[MAX_MESH_COUNT];
	// Assuming meshes is an array of AF_MeshData, we will add a placeholder for now
	cJSON_AddNullToObject(returnJSON, "meshes");

	// meshCount
	cJSON_AddNumberToObject(returnJSON, "meshCount", _component->meshCount);

	// showDebug
	cJSON_AddNumberToObject(returnJSON, "showDebug", _component->showDebug);

	// meshType
	enum AF_MESH_TYPE meshType;
	cJSON_AddNumberToObject(returnJSON, "meshType", _component->meshType);

	// meshPath
	cJSON_AddStringToObject(returnJSON, "meshPath", _component->meshPath);

	// Shader
	cJSON* shaderJSON = cJSON_AddObjectToObject(returnJSON, "shader");
	cJSON_AddStringToObject(shaderJSON, "vertexShader", _component->shader.vertPath);
	cJSON_AddStringToObject(shaderJSON, "fragmentShader", _component->shader.fragPath);

	// Material
	cJSON* materialJson = cJSON_AddObjectToObject(returnJSON, "material");

	//diffuse texture
	cJSON* diffuseTexturePathJson = cJSON_AddStringToObject(materialJson, "diffuseTexture", _component->material.diffuseTexture.path);

	//specular texture
	cJSON* specularTexturePathJson = cJSON_AddStringToObject(materialJson, "specularTexture", _component->material.specularTexture.path);

	//normal texture
	cJSON* normalTexturePathJson = cJSON_AddStringToObject(materialJson, "normalTexture", _component->material.normalTexture.path);

	// Color
	uint8_t color[4];
	cJSON* colorJSONArray = cJSON_AddArrayToObject(materialJson, "color");
	cJSON* rObject = cJSON_AddNumberToObject(colorJSONArray, "r", _component->material.color.r);
	cJSON* gObject = cJSON_AddNumberToObject(colorJSONArray, "g", _component->material.color.g);
	cJSON* bObject = cJSON_AddNumberToObject(colorJSONArray, "b", _component->material.color.b);
	cJSON* aObject = cJSON_AddNumberToObject(colorJSONArray, "a", _component->material.color.a);

	// shininess
	cJSON* shininessJson = cJSON_AddNumberToObject(materialJson, "shininess", _component->material.shininess);




	// isImageFlipped
	cJSON_AddNumberToObject(returnJSON, "isImageFlipped", _component->isImageFlipped);

	// meshID
	cJSON_AddNumberToObject(returnJSON, "meshID", _component->meshID);

	// isAnimating
	cJSON_AddNumberToObject(returnJSON, "isAnimating", _component->isAnimating);

	// textured
	cJSON_AddNumberToObject(returnJSON, "textured", _component->textured);

	// transparent
	cJSON_AddNumberToObject(returnJSON, "transparent", _component->transparent);

	// recieveLights
	cJSON_AddNumberToObject(returnJSON, "recieveLights", _component->recieveLights);

	// recieveShadows
	cJSON_AddNumberToObject(returnJSON, "recieveShadows", _component->recieveShadows);

	// castShadows
	cJSON_AddNumberToObject(returnJSON, "castShadows", _component->castShadows);

	// modelMatrix
	cJSON_AddNullToObject(returnJSON, "modelMatrix");

	// displayListBuffer
	void* displayListBuffer;
	cJSON_AddNullToObject(returnJSON, "modelMatrix");

	return returnJSON;
}

cJSON* AF_JSON_TextToJson(AF_CText* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// Is dirty
	cJSON_AddNumberToObject(returnJSON, "has", _component->isDirty);

	// isShowing
	cJSON_AddNumberToObject(returnJSON, "isShowing", _component->isShowing);

	// fontID
	cJSON_AddNumberToObject(returnJSON, "fontID", _component->fontID);

	// fontPath
	cJSON_AddStringToObject(returnJSON, "fontPath", _component->fontPath);

	// text
	cJSON_AddStringToObject(returnJSON, "text", _component->text);

	// screenPos
	AF_JSON_Vec2ToJson("screenPos", &_component->screenPos, returnJSON);

	// textBounds
	Vec2 textBounds;
	AF_JSON_Vec2ToJson("screenPos", &_component->screenPos, returnJSON);

	// textColor
	uint8_t textColor[4];
	cJSON* textJSONArray = cJSON_AddArrayToObject(returnJSON, "textColor");
	cJSON* xObject = cJSON_AddNumberToObject(textJSONArray, "x", _component->textColor[0]);
	cJSON* yObject = cJSON_AddNumberToObject(textJSONArray, "y", _component->textColor[1]);
	cJSON* zObject = cJSON_AddNumberToObject(textJSONArray, "z", _component->textColor[2]);
	cJSON* wObject = cJSON_AddNumberToObject(textJSONArray, "w", _component->textColor[3]);


	// textData
	cJSON_AddNullToObject(returnJSON, "textData");

	return returnJSON;
}

cJSON* AF_JSON_AudioSourceToJson(AF_CAudioSource* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// clip
	cJSON_AddNullToObject(returnJSON, "clip");

	// channel
	cJSON_AddNumberToObject(returnJSON, "channel", _component->channel);

	// loop
	cJSON_AddNumberToObject(returnJSON, "channel", _component->channel);

	// isPlaying
	cJSON_AddNumberToObject(returnJSON, "isPlaying", _component->isPlaying);

	// clipData
	cJSON_AddNullToObject(returnJSON, "clipData");

	return returnJSON;
}

cJSON* AF_JSON_PlayerDataToJson(AF_CPlayerData* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	// isHuman;
	cJSON_AddNumberToObject(returnJSON, "isHuman", _component->isHuman);

	// isCarrying;
	cJSON_AddNumberToObject(returnJSON, "isCarrying", _component->isCarrying);

	// isCarried;
	cJSON_AddNumberToObject(returnJSON, "isCarried", _component->isCarried);

	// carryingEntity;
	cJSON_AddNumberToObject(returnJSON, "carryingEntity", _component->carryingEntity);

	// health;
	cJSON_AddNumberToObject(returnJSON, "health", _component->health);

	// isAlive;
	cJSON_AddNumberToObject(returnJSON, "isAlive", _component->isAlive);

	// isAttacking;
	cJSON_AddNumberToObject(returnJSON, "isAttacking", _component->isAttacking);

	// isJumping;
	cJSON_AddNumberToObject(returnJSON, "isJumping", _component->isJumping);

	// movementSpeed;
	cJSON_AddNumberToObject(returnJSON, "movementSpeed", _component->movementSpeed);

	// score;
	cJSON_AddNumberToObject(returnJSON, "score", _component->score);

	// startPosition;
	AF_JSON_Vec3ToJson("startPosition", &_component->startPosition, returnJSON);

	// targetDestination;
	AF_JSON_Vec3ToJson("targetDestination", &_component->targetDestination, returnJSON);

	// spawnTime;
	cJSON_AddNumberToObject(returnJSON, "spawnTime", _component->spawnTime);

	// PLAYER_FACTION faction;
	cJSON_AddNumberToObject(returnJSON, "faction", _component->faction);

	return returnJSON;
}

cJSON* AF_JSON_SkeletalAnimationToJson(AF_CSkeletalAnimation* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	//* model; 
	cJSON_AddNullToObject(returnJSON, "model");

	//* skeleton;
	cJSON_AddNullToObject(returnJSON, "skeleton");

	//* skeletonBlend;
	cJSON_AddNullToObject(returnJSON, "skeletonBlend");

	// char* animIdlePath;
	cJSON_AddStringToObject(returnJSON, "animIdlePath", _component->animIdlePath);


	// TODO: make this an array with defined indexs
	//* idleAnimationData;
	cJSON_AddNullToObject(returnJSON, "idleAnimationData");

	// char* animWalkPath;
	cJSON_AddStringToObject(returnJSON, "animWalkPath", _component->animWalkPath);

	//* walkAnimationData;
	cJSON_AddNullToObject(returnJSON, "walkAnimationData");

	// char* animAttackPath;
	cJSON_AddStringToObject(returnJSON, "animAttackPath", _component->animAttackPath);

	//* attackAnimationData;
	cJSON_AddNullToObject(returnJSON, "attackAnimationData");

	// animationSpeed;
	cJSON_AddNumberToObject(returnJSON, "animationSpeed", _component->animationSpeed);

	// animationBlend;
	cJSON_AddNumberToObject(returnJSON, "animationBlend", _component->animationBlend);

	// AnimationType animationTypeID;
	cJSON_AddNumberToObject(returnJSON, "animationTypeID", _component->animationTypeID);

	// nextFrameTime;		// 16 or 32 bytes
	cJSON_AddNumberToObject(returnJSON, "nextFrameTime", _component->nextFrameTime);

	// currentFrame;		// 8 bytes
	cJSON_AddNumberToObject(returnJSON, "currentFrame", _component->currentFrame);

	// animationFrames;	// 8 bytes
	cJSON_AddNumberToObject(returnJSON, "animationFrames", _component->animationFrames);

	// loop;			// 8 bytes
	cJSON_AddNumberToObject(returnJSON, "loop", _component->loop);

	return returnJSON;
}

cJSON* AF_JSON_AIBehaviourToJson(AF_CAI_Behaviour* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	//AF_AI_Action actionsArray[AF_AI_ACTION_ARRAY_SIZE];	// store up to 8 actions to perform
	cJSON_AddNullToObject(returnJSON, "actionsArray");

	//uint8_t nextAvailableActionSlot;
	cJSON_AddNumberToObject(returnJSON, "nextAvailableActionSlot", _component->nextAvailableActionSlot);

	return returnJSON;
}

cJSON* AF_JSON_EditorDataToJson(AF_CEditorData* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	//char name[MAX_EDITORDATA_NAME_CHAR_LENGTH];	 
	cJSON_AddStringToObject(returnJSON, "name", _component->name);

	//bool isSelected;	
	cJSON_AddNumberToObject(returnJSON, "isSelected", _component->isSelected);

	return returnJSON;
}

cJSON* AF_JSON_InputControllerToJson(AF_CInputController* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	//uint8_t inputActionCount;
	cJSON_AddNumberToObject(returnJSON, "inputActionCount", _component->inputActionCount);

	//AF_InputAction inputActions[MAX_CINPUTCONTROLLER_ACTIONS];
	cJSON_AddNullToObject(returnJSON, "inputActions");

	return returnJSON;
}

cJSON* AF_JSON_ScriptsToJson(AF_CScript* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	if(has == AF_TRUE){
		cJSON_AddNumberToObject(returnJSON, "has", 1);
	} else{
		cJSON_AddNumberToObject(returnJSON, "has", 0);
	}
	

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	if(enabled == AF_TRUE){
		cJSON_AddNumberToObject(returnJSON, "enabled", 1);
	} else{
		cJSON_AddNumberToObject(returnJSON, "enabled", 0);
	}
	
	cJSON_AddStringToObject(returnJSON, "scriptName", _component->scriptName);

	//char scriptFullPath[MAX_CSCRIPT_PATH];
	cJSON_AddStringToObject(returnJSON, "scriptFullPath", _component->scriptFullPath);

	//ScriptFuncPtr startFuncPtr;
	//cJSON_AddNullToObject(returnJSON, "startFuncPtr");

	//ScriptFuncPtr updateFuncPtr;
	//cJSON_AddNullToObject(returnJSON, "updateFuncPtr");

	//ScriptFuncPtr destroyFuncPtr;
	//cJSON_AddNullToObject(returnJSON, "destroyFuncPtr");

	//void* loadedScriptPtr;
	//cJSON_AddNullToObject(returnJSON, "loadedScriptPtr");

	return returnJSON;
}

cJSON* AF_JSON_LightToJson(AF_CLight* _component) {
	cJSON* returnJSON = cJSON_CreateObject();

	// has
	af_bool_t has = AF_Component_GetHas(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "has", has);

	// enabled
	af_bool_t enabled = AF_Component_GetEnabled(_component->enabled);
	cJSON_AddNumberToObject(returnJSON, "enabled", enabled);

	//enum AF_Light_Type_e lightType;
	cJSON_AddNumberToObject(returnJSON, "lightType", _component->lightType);

	//Vec3 direction;
	AF_JSON_Vec3ToJson("direction", &_component->direction, returnJSON);

	//Vec3 ambientCol;
	AF_JSON_Vec3ToJson("ambientCol", &_component->ambientCol, returnJSON);

	//Vec3 diffuseCol;
	AF_JSON_Vec3ToJson("diffuseCol", &_component->diffuseCol, returnJSON);

	//Vec3 specularCol;
	AF_JSON_Vec3ToJson("specularCol", &_component->specularCol, returnJSON);

	//AF_FLOAT constant;
	cJSON_AddNumberToObject(returnJSON, "constant", _component->constant);

	//AF_FLOAT linear;
	cJSON_AddNumberToObject(returnJSON, "linear", _component->linear);

	//AF_FLOAT quadratic;
	cJSON_AddNumberToObject(returnJSON, "quadratic", _component->quadratic);

	//AF_FLOAT cutOff;
	cJSON_AddNumberToObject(returnJSON, "cutOff", _component->cutOff);

	//AF_FLOAT outerCutoff;
	cJSON_AddNumberToObject(returnJSON, "outerCutoff", _component->outerCutoff);

	return returnJSON;
}

// VECTOR TO JSON CONVERTERS

cJSON* AF_JSON_Vec2ToJson(const char* _name, Vec2* _vec2, cJSON* _rootJSON) {
	cJSON* vec2 = cJSON_AddArrayToObject(_rootJSON, _name);
	cJSON* xObject = cJSON_AddNumberToObject(vec2, "x", _vec2->x);
	cJSON* yObject = cJSON_AddNumberToObject(vec2, "y", _vec2->y);
	return vec2;
}

cJSON* AF_JSON_Vec3ToJson(const char* _name, Vec3* _vec3, cJSON* _rootJSON) {
	cJSON* vec3 = cJSON_AddArrayToObject(_rootJSON, _name);
	cJSON* xObject = cJSON_AddNumberToObject(vec3, "x", _vec3->x);
	cJSON* yObject = cJSON_AddNumberToObject(vec3, "y", _vec3->y);
	cJSON* zObject = cJSON_AddNumberToObject(vec3, "z", _vec3->z);
	return vec3;
}

cJSON* AF_JSON_Vec4ToJson(const char* _name, Vec4* _vec4, cJSON* _rootJSON) {
	if (_vec4 == NULL || _rootJSON == NULL || _name == NULL) {
		AF_Log_Error("AF_JSON_Vec4ToJson: Invalid parameters.\n");
		return NULL;
	}
	if (strlen(_name) == 0) {
		AF_Log_Error("AF_JSON_Vec4ToJson: Name cannot be empty.\n");
		return NULL;
	}
	cJSON* vec4 = cJSON_AddArrayToObject(_rootJSON, _name);
	cJSON* xObject = cJSON_AddNumberToObject(vec4, "x", _vec4->x);
	cJSON* yObject = cJSON_AddNumberToObject(vec4, "y", _vec4->y);
	cJSON* zObject = cJSON_AddNumberToObject(vec4, "z", _vec4->z);
	cJSON* wObject = cJSON_AddNumberToObject(vec4, "w", _vec4->w);
	return vec4;
}

cJSON* AF_JSON_Mat4ToJson(const char* _name, Mat4* _mat4, cJSON* _rootJSON) {
	if (_mat4 == NULL || _rootJSON == NULL || _name == NULL) {
		AF_Log_Error("AF_JSON_Mat4ToJson: Invalid parameters.\n");
		return NULL;
	}
	cJSON* mat4 = cJSON_AddArrayToObject(_rootJSON, "modelMatrix");
	for (uint32_t x = 0; x < 4; x++) {
		cJSON* vec4_row1 = cJSON_AddArrayToObject(mat4, "vec4");
		// x is potentially NAN. and will cause cJSON to fail to create the object
		if (isnan(_mat4->rows[x].x) || isnan(_mat4->rows[x].y) || isnan(_mat4->rows[x].z) || isnan(_mat4->rows[x].w)) {
			AF_Log_Error("AF_JSON_Mat4ToJson: Mat4 contains NAN values at row %d.\n", x);
			return NULL;
		}
		cJSON* xObject = cJSON_AddNumberToObject(vec4_row1, "x", _mat4->rows[x].x);
		cJSON* yObject = cJSON_AddNumberToObject(vec4_row1, "y", _mat4->rows[x].y);
		cJSON* zObject = cJSON_AddNumberToObject(vec4_row1, "z", _mat4->rows[x].z);
		cJSON* wObject = cJSON_AddNumberToObject(vec4_row1, "w", _mat4->rows[x].w);
	}
	return mat4;
}
