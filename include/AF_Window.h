// ===============================================================================
// AF_WINDOW_H
// 
// Definition of Window helper functions
//
// ===============================================================================

#ifndef AF_WINDOW_H
#define AF_WINDOW_H
#include <stdint.h>
#include <stddef.h>
#include "AF_Lib_Define.h"
#include "AF_Vec2.h"

#ifdef __cplusplus
extern "C" {
#endif


// ====================
// AF_Window
// Window struct
// ====================
#pragma pack(push, 8)  // Save current packing and set to 8-byte alignment
typedef struct {
    uint16_t frameBufferWidth;  // (2 bytes)
    uint16_t frameBufferHeight; // (2 bytes)

    // Pointers first - they need 8-byte alignment naturally
    void* window;           // (8 bytes)
    void* input;            // (8 bytes)
    const char* title;      // (8 bytes)
    
    // Group all uint16_t members together to minimize padding
    AF_FLOAT windowXPos;    // (2 bytes)
    AF_FLOAT windowYPos;    // (2 bytes)
    uint16_t windowWidth;   // (2 bytes)
    uint16_t windowHeight;  // (2 bytes)
    
    // Flags - using af_bool_t (uint8_t) for boolean flags
    af_bool_t isFrameUpdated;   // (1 byte)
    af_bool_t isWindowResized;  // (1 byte)
} AF_Window;
#pragma pack(pop)   // Restore previous packing


// ====================
// AF_Window_ZERO
// Return a zero-initialized AF_Window struct with the given title and dimensions
// ====================
inline static AF_Window AF_Window_ZERO(const char* _title, uint16_t _windowWidth, uint16_t _windowHeight){
    AF_Window window = {
        .frameBufferWidth = 0,
        .frameBufferHeight = 0,
        .window = NULL,
        .input = NULL,
        .title = NULL,
        .windowXPos = 0,
        .windowYPos = 0,
        .windowWidth = 0,
        .windowHeight = 0,
        .isFrameUpdated = AF_FALSE,
        .isWindowResized = AF_FALSE
    };
    // assign the provided values
    window.windowWidth = _windowWidth;
    window.windowHeight = _windowHeight;
    window.frameBufferWidth = _windowWidth;
    window.frameBufferHeight = _windowHeight;
    window.title = _title;
    return window;
}


// ====================
// AF_Window_Create
// Create the window and init all the window things
// Platform/library dependent. Likely using glfw
// ====================
af_bool_t AF_Window_Create(void* _appData);

// ====================
// AF_Window_Update
// Update the window
// Platform/library dependent. Likely using glfw
// ====================
af_bool_t AF_Window_Update(AF_Window* _window);



// ====================
// AF_Window_Render
// Tell windowing to swap buffers
// ====================
void AF_Window_Render(AF_Window* _window);


// ====================
// AF_Window_Terminate
// Close and clean up the window
// Platform/library dependent. Likely using glfw
// ====================
af_bool_t AF_Window_Terminate(AF_Window* _window);


// ====================
// AF_Window_GetFramebufferSize
// Get the current framebuffer size as a Vec2
// ====================
Vec2 AF_Window_GetFramebufferSize(AF_Window* _window);


#ifdef __cplusplus
}
#endif

#endif  // AF_WINDOW_H
