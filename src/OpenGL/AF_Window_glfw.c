/*
===============================================================================
AF_Window_glfw implementation

Implementation for AF_Window using GLFW
Calls GLFW library to handling window creation and input handling
===============================================================================
*/
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


/*
====================
error_callback
General Error callback used by glfw
====================
*/
void error_callback(int error, const char* description)
{
    AF_Log_Error("%s %s %i\n", glfwWindowFileTitle, description, error);
}


/*
====================
key_callback
Key callback used by glfw
====================
*/
static void key_callback (GLFWwindow* _window, int key, int scancode, int action, int mods)
{
	// TODO: https://www.reddit.com/r/opengl/comments/i8lv8u/how_can_i_optimize_my_key_handling_and_make_it/
    if(scancode){}
    if(mods){}
    AF_AppData* afAppData = (AF_AppData*)glfwGetWindowUserPointer(_window);
    if(afAppData == NULL){
        AF_Log_Error("%s key_callback: afAppData is NULL\n", glfwWindowFileTitle);
        return;
    }
    
    AF_Input* input = &afAppData->input;
    if(input == NULL){
        AF_Log_Error("%s key_callback: input is NULL\n", glfwWindowFileTitle);
        return;
    }

    /*
    if(action == GLFW_PRESS){
	// find the key and set it to pressed
	//for(int i = 0; i < AF_INPUT_KEYS_MAPPED; i++){
    // TODO: work for multiple controllers
    for(int i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; i++){
		if(AF_Input_GetKeyCode(input->keys[0][i].code) == key){
			input->keys[0][i].code = AF_Input_EncodeKey(input->keys[0][i].code, TRUE); 
		}
	}
    }else if(action == GLFW_RELEASE){
	// find the key and set it to release
	for(int i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; i++){
		if(AF_Input_GetKeyCode(input->keys[0][i].code) == key){
			input->keys[0][i].code = AF_Input_EncodeKey(input->keys[0][i].code, FALSE); 
		}
	}

    }*/

    // TODO better match this 
    //AF_Input* input = &editorAppData->appData.input;
    // TODO: add for other controllers
    for(uint32_t i = 0; i < AF_INPUT_KEYBOARD_KEYS_COUNT; i++){
        if(input->keys[0][i].code == key){
            input->keys[0][i].pressed = (action == GLFW_PRESS);
            input->keys[0][i].held = (action == GLFW_REPEAT);
        }
    }
    // Retrieve the pointer to the AF_Input struct from the window user pointer
    // increment the buffer to position the next key, rollover if at the end of the array
     
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }
}

/*
====================
window_pos_callback
Move window callback
====================
*/
void window_pos_callback(GLFWwindow* _window, int _xpos, int _ypos){
    if(_xpos || _ypos){}
    // if the window is moved, update the glviewport
    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)_window, &width, &height);
    glViewport(0, 0, width, height);

    AF_AppData* afAppData = (AF_AppData*)glfwGetWindowUserPointer(_window);
    if(afAppData == NULL){
        AF_Log_Error("%s window_pos_callback: afAppData is NULL\n", glfwWindowFileTitle);
        return;
    }
        afAppData->window.windowXPos = _xpos;
        afAppData->window.windowYPos = _ypos;
}

/*
====================
framebuffer_size_callback
when the framebuffer size changes, update the glViewport
====================
*/
void framebuffer_size_callback(GLFWwindow* _window, int _width, int _height)
{
    if(_width || _height){}
    int32_t width = 0;
    int32_t height = 0;
    glViewport(0, 0, width, height);

    AF_AppData* afAppData = (AF_AppData*)glfwGetWindowUserPointer(_window);
    if(afAppData == NULL){
        AF_Log_Error("%s window_pos_callback: afAppData is NULL\n", glfwWindowFileTitle);
        return;
    }
    glfwGetFramebufferSize((GLFWwindow*)_window, &width, &height);
}

/*
====================
window_size_callback
when the window  size changes, update the glViewport
====================
*/
void window_size_callback(GLFWwindow* _window, int _width, int _height)
{
    if(_width || _height){}
    int width, height;

    glfwGetFramebufferSize((GLFWwindow*)_window, &width, &height);
    glViewport(0, 0, width, height);

    // Update the window size
    AF_AppData* afAppData = (AF_AppData*)glfwGetWindowUserPointer(_window);
    if(afAppData == NULL){
        AF_Log_Error("%s window_size_callback: afAppData is NULL\n", glfwWindowFileTitle);
        return;
    }
    afAppData->window.windowWidth = _width;
    afAppData->window.windowHeight = _height;
}

/*
====================
cursor_position_callback
when the cursor changes, update the input data
====================
*/
static void cursor_position_callback(GLFWwindow* _window, double _xpos, double _ypos) {
    //Editor_AppData* editorAppData = (Editor_AppData*)glfwGetWindowUserPointer(_window);
    AF_AppData* appData = (AF_AppData*)glfwGetWindowUserPointer(_window);
    if(appData == NULL){
        AF_Log_Warning("cursor_position_callback: appData is null\n");
        return;
    }

    // Reset the last mouse position if this is the first movement after pressing the right mouse button
    if (appData->input.firstMouse == TRUE && appData->input.mouse2Down == TRUE) {
        appData->input.lastMouseX = _xpos;
        appData->input.lastMouseY = _ypos;
        appData->input.firstMouse = FALSE; // Prevent resetting on subsequent movements
    }

    // Update current mouse position
    appData->input.mouseX = _xpos;
    appData->input.mouseY = _ypos;
}

/*
====================
mouse_button_callback
when the mouse button changes, update the input data
====================
*/
static void mouse_button_callback(GLFWwindow* _window, int button, int action, int mods) {
    if(mods){}
    AF_AppData* appData = (AF_AppData*)glfwGetWindowUserPointer(_window);
    if(appData == NULL){
        AF_Log_Warning("mouse_button_callback: editorAppData is null\n");
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // Update left mouse button state
        appData->input.mouse1Down = (action == GLFW_PRESS);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        // Update right mouse button state and reset firstMouse flag
        if (action == GLFW_PRESS) {
            appData->input.mouse2Down = TRUE;
            appData->input.firstMouse = TRUE; // Ensure reset for next cursor movement
        } else if (action == GLFW_RELEASE) {
            appData->input.mouse2Down = FALSE;
            appData->input.firstMouse = TRUE; // Prepare for future presses
        }
    }
}


/*
====================
AF_Window_CreateWindow
Create a window using GLFW
====================
*/
BOOL AF_Window_Create(void* _appData) {
    
    if(!_appData){
        AF_Log("%s CreateWindow: _appData is NULL\n", glfwWindowFileTitle);
        AF_Log_Error("%s CreateWindow: failed to create window\n", glfwWindowFileTitle);
        return FALSE;
    }

    AF_Log("%s Create Window\n", glfwWindowFileTitle);
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        // Initialization failed
        AF_Log_Error("%sCreateWindow: Failed to init glfw\n", glfwWindowFileTitle);
        return FALSE;
    }
 
    // If using openGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    AF_AppData* appData = (AF_AppData*)_appData;
    AF_Window* _window = &appData->window;
    AF_Log("AF_Lib_CreateWindow: appData %p, window %p \n",appData, &appData->window);
   
    GLFWwindow* glfwWindow = glfwCreateWindow(_window->windowWidth, _window->windowHeight, _window->title, NULL, NULL);
   
    if (!glfwWindow)
    {
        // Window or context creation failed
         AF_Log_Error("%s CreateWindow: Failed to create a window\n", glfwWindowFileTitle);
         return FALSE;
    }
    // assign the glfw window ptr to the struct passed in
    _window->window = glfwWindow;

    // Set the framebuffer sies
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);
    glViewport(0, 0, width, height);

     _window->frameBufferWidth = width; 
     _window->frameBufferHeight = height;

    // make current context
    glfwMakeContextCurrent(glfwWindow);

    /**/
    // Set the user ptr to that of type AF_Window struct.
    //_app data is a void* so care should be used and ensure its always cast to AF_AppData checked before used a
    glfwSetWindowUserPointer(glfwWindow, _appData);

    // Set window size callback 
    glfwSetWindowSizeCallback(glfwWindow, window_size_callback);

    // Set callback
    glfwSetKeyCallback(glfwWindow, key_callback);

     // cursor callback
    glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);

    // set window move callpack
    glfwSetWindowPosCallback(glfwWindow, window_pos_callback);

    // Set the window size correctly, needed for OSX retina displays
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);

    // Set the user ptr of the window to 
    glfwSetWindowTitle((GLFWwindow*) appData->window.window, appData->projectData.name);

    return TRUE;
}


/*
====================
AF_Window_UpdateWindow
Update the window
====================
*/
BOOL AF_Window_Update(AF_Window* _window){
    // while
    if (glfwWindowShouldClose(_window->window))
    {
        // Close the window
        return FALSE;
    }

    
    // return true (we are still running)
    return TRUE;
}

/*
====================
AF_Window_RenderWindow
Render the window by calling glfw swap buffers
====================
*/
void AF_Window_Render(AF_Window* _window){
    // Set the framebuffer sies
    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)_window->window, &width, &height);
    glViewport(0, 0, width, height);

    _window->frameBufferWidth = width; 
    _window->frameBufferHeight = height;


    /* Swap front and back buffers */
    glfwSwapBuffers(_window->window);

    /* Poll for and process events */
    glfwPollEvents();
}

/*====================
AF_Window_TerminateWindow
Destroy the window
====================
*/
void AF_Window_Terminate(AF_Window* _window){
    // null check the struct
    if(!_window){
        AF_Log_Error("%s TerminateWindow: failed to destroy window, argment passed in a null AF_Window struct\n", glfwWindowFileTitle);
        return;
    }

    // Null check the window pointer
    if(!_window->window){
        AF_Log_Error("%s TerminateWindow: failed to destroy window, argment passed in a null window ptr\n", glfwWindowFileTitle);
        return;
    }

    AF_Log("%s TerminateWindow:\n", glfwWindowFileTitle);

     // Destory the wndow
    glfwDestroyWindow(_window->window);
    glfwTerminate();
}



