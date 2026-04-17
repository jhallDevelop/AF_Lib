// ===============================================================================
// AF_PROJECT_H
// This file defines the interface for project management, including loading project data and scenes.
// ===============================================================================

#ifndef AF_PROJECT_H
#define AF_PROJECT_H
#include "AF_AppData.h"
#include "AF_Lib_API.h"
#ifdef __cplusplus
extern "C" {
#endif

AF_LIB_API void AF_Project_SyncEntities(AF_AppData* _appData);
AF_LIB_API af_bool_t AF_Project_Load(AF_AppData* _appData, const char* _filePath);
AF_LIB_API af_bool_t AF_Project_LoadScene(AF_AppData* _appData, const char* _sceneFilePath);
AF_LIB_API af_bool_t AF_Project_RelativizePath(const char* _inPath, const char* _projectRoot, char* _outPath, uint32_t _outPathSize);
AF_LIB_API void AF_Project_RequestSceneChange(AF_AppData* _appData, const char* _sceneFilePath);

#ifdef __cplusplus
}
#endif


#endif // AF_PROJECT_H
