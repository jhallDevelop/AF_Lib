#ifndef AF_JSON_H
#define AF_JSON_H

#include <stdint.h>
#include <stdio.h>
#include "AF_Lib_Define.h"
#include "AF_ProjectData.h"
#include "AF_AppData.h"
#include "ECS/Entities/AF_ECS.h"

#ifdef __cplusplus
extern "C" {
#endif

AF_LIB_API af_bool_t AF_JSON_LoadProjectDataJson(AF_ProjectData* _projectData, FILE* _file);
AF_LIB_API af_bool_t AF_JSON_LoadSceneJson(AF_AppData* _appData, FILE* _file);
af_bool_t AF_JSON_SaveProjectDataToJson(AF_ProjectData* _projectData, char* _charBuffer, uint32_t _charBufferSize);
af_bool_t AF_JSON_SaveECSToJson(AF_ECS* _ecs, char* _charBuffer, uint32_t _charBufferSize);

#ifdef __cplusplus
}
#endif

#endif// AF_JSON_H