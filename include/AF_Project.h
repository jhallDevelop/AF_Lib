/*
===============================================================================
AF_PROJECT_H


===============================================================================
*/
#ifndef AF_PROJECT_H
#define AF_PROJECT_H
#include "AF_AppData.h"
#include "AF_Lib_API.h"
#ifdef __cplusplus
extern "C" {
#endif



/*
================
AF_Project_SyncEntities
Sync the entities loaded
================
*/
AF_LIB_API void AF_Project_SyncEntities(AF_AppData* _appData);

/*
================
AF_Project_Load
Take a file path and open the game.proj file if it can be found.
================
*/
AF_LIB_API af_bool_t AF_Project_Load(AF_AppData* _appData, const char* _filePath);

#ifdef __cplusplus
}
#endif



#endif // AF_PROJECT_H
