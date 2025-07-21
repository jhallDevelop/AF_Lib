/*
===============================================================================
AF_WINDOW_H

Definition of Window helper functions

===============================================================================
*/
#ifndef AF_WINDOW_H
#define AF_WINDOW_H
#include <stdint.h>
#include "AF_Lib_Define.h"
#include "AF_Vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_Window
Window struct
====================
*/
#pragma pack(push, 8)  // Save current packing and set to 8-byte alignment
typedef struct {
    // Pointers first - they need 8-byte alignment naturally
    void* window;           // Offset 0  (8 bytes)
    void* input;            // Offset 8  (8 bytes)
    const char* title;      // Offset 16 (8 bytes)
    
    // Group all uint16_t members together to minimize padding
    AF_FLOAT windowXPos;    // Offset 24 (2 bytes)
    AF_FLOAT windowYPos;    // Offset 26 (2 bytes)
    uint16_t windowWidth;   // Offset 28 (2 bytes)
    uint16_t windowHeight;  // Offset 30 (2 bytes)
    uint16_t frameBufferWidth;  // Offset 32 (2 bytes)
    uint16_t frameBufferHeight; // Offset 34 (2 bytes)
    af_bool_t isFrameUpdated;
    af_bool_t isWindowResized;
} AF_Window;
#pragma pack(pop)   // Restore previous packing


inline static AF_Window AF_Window_ZERO(const char* _title, uint16_t _windowWidth, uint16_t _windowHeight){
    AF_Window window;
    window.window = 0;
    window.input = 0;
    window.title = _title;
    window.windowXPos = 0;
    window.windowYPos = 0;
    window.windowWidth = _windowWidth;
    window.windowHeight = _windowHeight;
    window.frameBufferWidth = _windowWidth;
    window.frameBufferHeight = _windowHeight;
    window.isFrameUpdated = AF_FALSE;
    window.isWindowResized = AF_FALSE;
    return window;
}

/*
====================
AF_Window_Create
Create the window and init all the window things
Platform/library dependent. Likely using glfw
====================
*/
af_bool_t AF_Window_Create(void* _appData);

/*
====================
AF_Window_Update
Update the window
Platform/library dependent. Likely using glfw

====================
*/
af_bool_t AF_Window_Update(AF_Window* _window);


/*
====================
AF_Window_Render
Tell windowing to swap buffers

====================
*/
void AF_Window_Render(AF_Window* _window);

/*
====================
AF_Window_Terminate
Close and clean up the window
Platform/library dependent. Likely using glfw

====================
*/
void AF_Window_Terminate(AF_Window* _window);

Vec2 AF_Window_GetFramebufferSize(AF_Window* _window);


#ifdef __cplusplus
}
#endif

#endif  // AF_WINDOW_H
