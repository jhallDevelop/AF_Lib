/*
===============================================================================
AF_CAPPDATA_H definitions

Definition for the app data that will be shared around
and helper functions
===============================================================================
*/
#ifndef AF_APPDATA_H
#define AF_APPDATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AF_Lib_Define.h"
#include "AF_Vec4.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_Time.h"
#include "AF_Window.h"
//#include "AF_ViewportData.h"
#include "AF_Input.h"
#include "AF_Assets.h"
#include "AF_ProjectData.h"
#include "AF_LightingData.h"
#include "AF_RenderingData.h"

#pragma pack(push, 8)  // Set 8-byte alignment
typedef struct AF_AppData {
    // Start with Vec4 since it likely contains floating point values
    // that benefit from strict alignment
    Vec4 backgrounColor;    // Likely 16 bytes (4 floats)

    // Place larger structs next
    AF_ECS ecs;            // Struct size depends on definition
    AF_Time time;          // Struct size depends on definition
    AF_Window window;      // We know this is 40 bytes
    AF_Input input;        // Struct size depends on definition
    AF_Assets assets;      // Struct size depends on definition
    AF_ProjectData projectData;  // Struct size depends on definition
    AF_LightingData lightingData;   // struct to hold important cached lighting data
    AF_RenderingData rendererData;   // struct to hold important rendering data
    // Group boolean values together at the end to minimize padding
    bool isRunning;        // 1 byte
    bool isFullscreen;     // 1 byte
    
    // Add padding to ensure total struct size is multiple of 8
    uint8_t _padding[6];   // 6 bytes of padding
} AF_AppData;
#pragma pack(pop)

static inline AF_AppData AF_AppData_ZERO(void){
    Vec4 blackColor = {0,0,0,0};
    AF_AppData returnAppData ={
        // Initialise the app data
        .backgrounColor = blackColor,
        .time = AF_Time_ZERO(0),
        .window = AF_Window_ZERO("", 0, 0),
        .input = AF_Input_ZERO(),
        .assets = AF_Assets_ZERO(),
        .projectData = Editor_Project_Data_ZERO(),
        .rendererData = AF_RenderingData_ZERO(),
        .isRunning = AF_FALSE,
        .isFullscreen = AF_FALSE
    };
    return returnAppData;
}


#ifdef __cplusplus
}
#endif

#endif //AF_ASSETS_H
