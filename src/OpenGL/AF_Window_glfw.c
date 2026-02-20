// ===============================================================================
// AF_Window_glfw implementation
// Implementation for AF_Window using GLFW
// Calls GLFW library to handling window creation and input handling
// ===============================================================================

#include "AF_Window.h"

#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "AF_Input.h"
#include "AF_Log.h"
#include <GL/glew.h>
#include "AF_AppData.h"
#define GL_SILENCE_DEPRECATION

// ------- Create Platform Independent Window -------
// TODO: make this get passed into create window
const char* glfwWindowFileTitle = "AF_Window_GLFW:";

// Global (internal) pointers to window and input data.
static AF_Window* g_window = NULL;
static AF_Input* g_input = NULL;


// ====================
// AF_Window_ErrorCallback
// General Error callback used by glfw
// ====================
// 
void AF_Window_ErrorCallback(int error, const char* description)
{
    AF_Log_Error("%s %s %i\n", glfwWindowFileTitle, description, error);
}

// ====================
// AF_Window_KeyCallback
// Key callback used by glfw
// ====================
static void AF_Window_KeyCallback (GLFWwindow* _window, int key, int scancode, int action, int mods)
{
	// TODO: https://www.reddit.com/r/opengl/comments/i8lv8u/how_can_i_optimize_my_key_handling_and_make_it/
    (void)scancode;
    (void)mods;
    (void)_window;
  
    if( key < 0 && key >= AF_INPUT_KEYBOARD_KEYS_COUNT) {
        return;
    }

    if (action == GLFW_PRESS) {
        g_input->keys[0][key].pressed = 1;
        g_input->keys[0][key].held = 1;
    } else if (action == GLFW_RELEASE) {
        g_input->keys[0][key].pressed = 0;
        g_input->keys[0][key].held = 0;
    }
    
    // Retrieve the pointer to the AF_Input struct from the window user pointer
    // increment the buffer to position the next key, rollover if at the end of the array
}


// ====================
// AF_Window_Pos_Callback
// Move window callback
// ====================
void AF_Window_Pos_Callback(GLFWwindow* _window, int _xpos, int _ypos){
    (void)_xpos;
    (void)_ypos;
    (void)_window;
    // if the window is moved, update the glviewport
    
    g_window->windowXPos = _xpos;
    g_window->windowYPos = _ypos;
    
    
    g_window->isWindowResized = AF_TRUE; // Set the window resized flag to true
    //AF_Log("window_pos_callback: Window moved to position (%d, %d)\n", _xpos, _ypos);
}


// ====================
// AF_Window_Framebuffer_Size_Callback
// when the framebuffer size changes, update the glViewport
// ====================
void AF_Window_Framebuffer_Size_Callback(GLFWwindow* _window, int _width, int _height)
{
    (void)_window;
    (void)_width;
    (void)_height;
}

// ====================
// AF_Window_Size_Callback
// when the window  size changes, update the glViewport
// ====================
void AF_Window_Size_Callback(GLFWwindow* _window, int _width, int _height)
{
    (void)_width;
    (void)_height;
    (void)_window;
    
    
    g_window->windowWidth = _width;
    g_window->windowHeight = _height;
    
	g_window->isWindowResized = AF_TRUE; // Set the window resized flag to true
}


// ====================
// AF_Window_Cursor_Position_Callback
// when the cursor changes, update the input data
// ====================
static void AF_Window_Cursor_Position_Callback(GLFWwindow* _window, double _xpos, double _ypos) {
    //Editor_AppData* editorAppData = (Editor_AppData*)glfwGetWindowUserPointer(_window);
    (void)_window;

    // Update current mouse position
    g_input->mouseX = _xpos;
    g_input->mouseY = _ypos;
}


// ====================
// AF_Window_Mouse_Button_Callback
// when the mouse button changes, update the input data
// ====================
static void AF_Window_Mouse_Button_Callback(GLFWwindow* _window, int button, int action, int mods) {
    (void)mods;
    (void)_window;
    
    // automatically assign the left and right mouse button states to the mouseDownArray for easier access
    if (button < 0 || button >= AF_INPUT_MOUSE_BUTTON_COUNT) {
        return; // Ignore out-of-bounds button indices
    }
    g_input->mouseDownArray[button] = (action == GLFW_PRESS);

    if(action == GLFW_PRESS){
        g_input->firstMouse = AF_TRUE; // Set firstMouse to false on mouse button press
    }
}



// ====================
// AF_Window_CreateWindow
// Create a window using GLFW
// ====================
af_bool_t AF_Window_Create(void* _appData) {
    assert(_appData != NULL && "AF_Window_Create: _appData is NULL");
    AF_AppData*appData = (AF_AppData*)_appData;
    g_input = &appData->input;
    g_window = &appData->window;
    
    AF_Log("%s AF_Window_Create\n", glfwWindowFileTitle);
    glfwSetErrorCallback(AF_Window_ErrorCallback);

    if (!glfwInit())
    {
        // Initialization failed
        AF_Log_Error("%s AF_Window_Create: Failed to init glfw\n", glfwWindowFileTitle);
        return AF_FALSE;
    }
 
     // --- Set Platform-Specific Window Hints ---
    #ifdef AF_WEB_BUILD
        // We are on the web, so we must request an OpenGL ES context for WebGL 2.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #else
        // We are on desktop, request a standard OpenGL 3.3 Core context
        // This is your original code
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    // ===================================================================
    // THIS IS THE FIX FOR THE BLACK SCREEN
    // Request an 8-bit alpha channel for the main canvas.
    // This solves strange clearing and transparency behavior in browsers.
    //glfwWindowHint(GLFW_ALPHA_BITS, 8);
    // ===================================================================

    
    //AF_Log("AF_Window_Create: appData %p, window %p width: %i height: %i \n",appData, &appData->window, _window->windowWidth, _window->windowHeight);
   
    GLFWwindow* glfwWindow = glfwCreateWindow(g_window->windowWidth, g_window->windowHeight, g_window->title, NULL, NULL);
    assert(glfwWindow != NULL && "AF_Window_Create: Failed to create GLFW window");
    
    // assign the glfw window ptr to the struct passed in
    g_window->window = glfwWindow;
    g_window->isFrameUpdated = AF_TRUE; // Set the frame updated flag to true to ensure the first frame renders correctly


    // make current context
    glfwMakeContextCurrent(glfwWindow);

    // This would disable the 120 FPS cap 
    glfwSwapInterval(0); 

    /**/
    // Set the user ptr to that of type AF_Window struct.
    //_app data is a void* so care should be used and ensure its always cast to AF_AppData checked before used a
    glfwSetWindowUserPointer(glfwWindow, _appData);

    // Set window size callback 
    glfwSetWindowSizeCallback(glfwWindow, AF_Window_Size_Callback);

    // Set callback
    glfwSetKeyCallback(glfwWindow, AF_Window_KeyCallback);

     // cursor callback
    glfwSetCursorPosCallback(glfwWindow, AF_Window_Cursor_Position_Callback);
    glfwSetMouseButtonCallback(glfwWindow, AF_Window_Mouse_Button_Callback);

    // set window move callpack
    glfwSetWindowPosCallback(glfwWindow, AF_Window_Pos_Callback);

    // Set the window size correctly, needed for OSX retina displays
    glfwSetFramebufferSizeCallback(glfwWindow, AF_Window_Framebuffer_Size_Callback);

    // Set the user ptr of the window to 
    glfwSetWindowTitle((GLFWwindow*) appData->window.window, appData->projectData.name);

    return AF_TRUE;
}


// ====================
// AF_Window_UpdateWindow
// Update the window
// ====================
af_bool_t AF_Window_Update(AF_Window* _window){
    // while
    if (glfwWindowShouldClose(_window->window))
    {
        // Close the window
        return AF_FALSE;
    }

    
    // return true (we are still running)
    return AF_TRUE;
}


// ====================
// AF_Window_RenderWindow
// Render the window by calling glfw swap buffers
// ====================
void AF_Window_Render(AF_Window* _window){

    if(g_window->isWindowResized == AF_TRUE || g_window->isFrameUpdated == AF_TRUE){
        // Set the framebuffer sies
        int width, height;
        glfwGetFramebufferSize((GLFWwindow*)_window->window, &width, &height);
        glViewport(0, 0, width, height);

        //_window->frameBufferWidth = width; 
        //_window->frameBufferHeight = height;
        _window->windowWidth = width;
        _window->windowHeight = height;

        g_window->isWindowResized = AF_FALSE; // Reset the window resized flag
        g_window->isFrameUpdated = AF_FALSE; // Reset the frame updated flag
    }

    /* Swap front and back buffers */
    glfwSwapBuffers(_window->window);

    /* Poll for and process events */
    
}

// ====================
// AF_Window_GetFramebufferSize
// Window size callback
// Get the framebuffer size of the window
// Returns a Vec2 with the width and height of the framebuffer
// ====================
Vec2 AF_Window_GetFramebufferSize(AF_Window* _window) {
    if (_window == NULL || _window->window == NULL) {
        AF_Log_Error("%s GetFramebufferSize: _window or _window->window is NULL\n", glfwWindowFileTitle);
    }
    int width = 0, height = 0;
    glfwGetFramebufferSize((GLFWwindow*)_window->window, &width, &height);
    Vec2 size  = {width, height};
    return size;
}

// ====================
// AF_Window_TerminateWindow
// Destroy the window
// ====================
af_bool_t AF_Window_Terminate(AF_Window* _window){
    // null check the struct
    if(_window == NULL){
        AF_Log_Error("%s TerminateWindow: failed to destroy window, argment passed in a null AF_Window struct\n", glfwWindowFileTitle);
        return AF_FALSE;
    }

    // Null check the window pointer
    if(_window->window == NULL){
        AF_Log_Error("%s TerminateWindow: failed to destroy window, argment passed in a null window ptr\n", glfwWindowFileTitle);
        return AF_FALSE;
    }

    AF_Log("%s TerminateWindow:\n", glfwWindowFileTitle);

     // Destory the wndow
    glfwDestroyWindow((GLFWwindow*)_window->window);
    _window->window = NULL;
    glfwTerminate();

    g_input = NULL;
    g_window = NULL;
    return AF_TRUE;
}



