/*
===============================================================================
AF_Lighting Implementation

Lighting system for managing and updating light data
Handles ambient, point, and spot lights
===============================================================================
*/
#include "AF_Lib_Define.h"
#include "AF_Lighting.h"
#include "AF_Log.h"
#include "AF_Math/AF_Math.h"
#include "ECS/Components/AF_CLight.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Entities/AF_ECS.h"
#include <GL/glew.h>

// =================================================================================================
// AF_Lighting_RenderForwardPointLights
// Update shaders with lighting data for forward rendering
// =================================================================================================
void AF_Lighting_RenderForwardPointLights(uint32_t _shader, AF_ECS* _ecs, AF_LightingData* _lightingData){
	
	// Ambient/Directional light
	if(_lightingData->ambientLightEntityIndex > 0){
		AF_CLight* light = &_ecs->lights[_lightingData->ambientLightEntityIndex];
		glUniform1f(AF_Shader_GetUniformLocation(_shader, "material.shininess"), 32.0f);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "dirLight.direction"), light->direction.x, light->direction.y, light->direction.z);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "dirLight.ambient"),  light->ambientCol.x, light->ambientCol.y, light->ambientCol.z); 
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "dirLight.diffuse"),  light->diffuseCol.x, light->diffuseCol.y, light->diffuseCol.z);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "dirLight.specular"),  light->specularCol.x, light->specularCol.y, light->specularCol.z);
	}
	
	// Spot light
	if(_lightingData->spotLightEntityIndex > 0){
		AF_Entity* spotLightEntity = &_ecs->entities[_lightingData->spotLightEntityIndex];
		AF_CLight* spotLight = &_ecs->lights[_lightingData->spotLightEntityIndex];
		uint32_t spotLightEntityID = AF_ECS_GetID(spotLightEntity->id_tag);
		AF_CTransform3D* spotLightTransform = &_ecs->transforms[spotLightEntityID];	
		Vec3* spotLightPos = &spotLightTransform->pos;
		
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "spotLight.position"), spotLightPos->x, spotLightPos->y, spotLightPos->z);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "spotLight.direction"), spotLight->direction.x, spotLight->direction.y, spotLight->direction.z);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "spotLight.ambient"), spotLight->ambientCol.x, spotLight->ambientCol.y, spotLight->ambientCol.z);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "spotLight.diffuse"), spotLight->diffuseCol.x, spotLight->diffuseCol.y, spotLight->diffuseCol.z);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, "spotLight.specular"), spotLight->specularCol.x, spotLight->specularCol.y, spotLight->specularCol.z);
		glUniform1f(AF_Shader_GetUniformLocation(_shader, "spotLight.constant"), spotLight->constant); 
		glUniform1f(AF_Shader_GetUniformLocation(_shader, "spotLight.linear"), spotLight->linear);
		glUniform1f(AF_Shader_GetUniformLocation(_shader, "spotLight.quadratic"), spotLight->quadratic);
		glUniform1f(AF_Shader_GetUniformLocation(_shader, "spotLight.cutoff"), AF_Math_Cos(AF_Math_Radians(spotLight->cutOff)));
		glUniform1f(AF_Shader_GetUniformLocation(_shader, "spotLight.outerCutOff"), AF_Math_Cos(AF_Math_Radians(spotLight->outerCutoff)));
	}
	
	// Point lights (up to maxLights)
	for(uint8_t i = 0; i < _lightingData->pointLightsFound; i++){
		uint16_t pointLightEntityIndex = _lightingData->pointLightIndexArray[i];
		AF_CLight* light = &_ecs->lights[pointLightEntityIndex];
		uint32_t pointLightEntityID = AF_ECS_GetID(_ecs->entities[pointLightEntityIndex].id_tag);
		Vec3* lightPosition = &_ecs->transforms[pointLightEntityID].pos;
		
		char uniformName[AF_MAX_PATH_CHAR_SIZE];
		
		// Position
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE, "pointLights[%i].position", i);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, uniformName), lightPosition->x, lightPosition->y, lightPosition->z);
		
		// Ambient
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE, "pointLights[%i].ambient", i);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, uniformName), light->ambientCol.x, light->ambientCol.y, light->ambientCol.z);
		
		// Diffuse
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE, "pointLights[%i].diffuse", i);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, uniformName), light->diffuseCol.x, light->diffuseCol.y, light->diffuseCol.z); 
		
		// Specular
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE, "pointLights[%i].specular", i);
		glUniform3f(AF_Shader_GetUniformLocation(_shader, uniformName), light->specularCol.x, light->specularCol.y, light->specularCol.z);
		
		// Constant
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE,"pointLights[%i].constant", i);
		glUniform1f(AF_Shader_GetUniformLocation(_shader, uniformName), light->constant);
	
		// Linear
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE, "pointLights[%i].linear", i);
		glUniform1f(AF_Shader_GetUniformLocation(_shader, uniformName), light->linear);

		// Quadratic
		snprintf(uniformName, AF_MAX_PATH_CHAR_SIZE, "pointLights[%i].quadratic", i);
		glUniform1f(AF_Shader_GetUniformLocation(_shader, uniformName), light->quadratic);
	}
}

// =================================================================================================
// AF_Lighting_UpdateLighting
// Update lighting data by searching and storing the indices of active lights
// Used in later render passes
// =================================================================================================
void AF_Lighting_UpdateLighting(AF_ECS *_ecs, AF_LightingData *_lightingData){
	// Clear the lighting data so we can re-count fresh
	_lightingData->ambientLightEntityIndex = 0;
	_lightingData->spotLightEntityIndex = 0;
	for(uint16_t x = 0; x < _lightingData->maxLights; x++){
		_lightingData->pointLightIndexArray[x] = 0;
	}
	_lightingData->pointLightsFound = 0;

	// Search all lights in the entities and store point lights, ambient, and spot light
	af_bool_t ambientLightFound = AF_FALSE;
	af_bool_t spotLightfound = AF_FALSE;
	af_bool_t allPointLightsFound = AF_FALSE;
	
	for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
		// Early exit if we have found all available lights
		if(ambientLightFound == AF_TRUE && spotLightfound == AF_TRUE && allPointLightsFound == AF_TRUE){
			break;
		}

		AF_Entity* entity = &_ecs->entities[i];
		if(!AF_Component_GetEnabled(entity->flags)){
			continue;
		}

		AF_CLight* light = &_ecs->lights[i];
		if(!AF_Component_GetHasEnabled(light->enabled)){
			continue;
		}

		// Handle different light types
		if(light->lightType == AF_LIGHT_TYPE_AMBIENT){
			if(_lightingData->ambientLightEntityIndex > 0){
				AF_Log_Warning("AF_Lighting_UpdateLighting: Ambient Light already set (entityIndex %i). Can't set ambient light (entityIndex: %i). You are only allowed 1. Disable the others\n", _lightingData->ambientLightEntityIndex, i);
				continue;
			}
			_lightingData->ambientLightEntityIndex = i;
			ambientLightFound = AF_TRUE;
		}
		else if(light->lightType == AF_LIGHT_TYPE_POINT){
			if(_lightingData->pointLightsFound >= 4){
				AF_Log_Warning("AF_Lighting_UpdateLighting: Point Lights already maxed out (entityIndex %i, %i, %i, %i). Can't set point light (entityIndex: %i). You are only allowed 4. Disable the others\n", 
					_lightingData->pointLightIndexArray[0], 
					_lightingData->pointLightIndexArray[1], 
					_lightingData->pointLightIndexArray[2], 
					_lightingData->pointLightIndexArray[3], i);
				allPointLightsFound = AF_TRUE;
				continue;
			}
			_lightingData->pointLightIndexArray[_lightingData->pointLightsFound] = i;
			_lightingData->pointLightsFound++;
		}
		else if (light->lightType == AF_LIGHT_TYPE_SPOT){
			if(_lightingData->spotLightEntityIndex > 0){
				AF_Log_Warning("AF_Lighting_UpdateLighting: Spot Light already set (entityIndex %i). Can't set spot light (entityIndex: %i). You are only allowed 1. Disable the others\n", _lightingData->spotLightEntityIndex, i);
				continue;
			}
			_lightingData->spotLightEntityIndex = i;
			spotLightfound = AF_TRUE;
		}
	}
}
