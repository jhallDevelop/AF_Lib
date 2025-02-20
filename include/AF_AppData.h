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

typedef struct AF_AppData {                            // 8 bytes (64 bit)
    bool isRunning;
    bool isFullscreen;
    Vec4 backgrounColor;
    AF_ECS ecs;    
    AF_Time time;  
    AF_Window window;   
    //AF_ViewportData viewport;     
    AF_Input input; 
    AF_Assets assets; // AppData      
    AF_ProjectData projectData;       
    // AppData
} AF_AppData;

static inline AF_AppData AF_AppData_ZERO(void){
    Vec4 blackColor = {0,0,0,0};
    AF_AppData returnAppData ={
        // Initialise the app data
       .isRunning = FALSE,
        .isFullscreen = FALSE,
        .backgrounColor = blackColor,
        //.ecs = NULL,
        .time = AF_Time_ZERO(0),
        .window = AF_Window_ZERO("", 0, 0),
        //_editorAppData->viewport = AF_ViewportData_ZERO();    
        .input = AF_Input_ZERO(),
        .assets = AF_Assets_ZERO(),
        .projectData = Editor_Project_Data_ZERO()
    };
    return returnAppData;
}


#ifdef __cplusplus
}
#endif

#endif //AF_ASSETS_H
