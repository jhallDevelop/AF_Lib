#include "AF_Input.h"

/*
====================
AF_Input_Input
Init definition
====================
*/
void AF_Input_Init(void) {


}

/*
====================
AF_Input_Update
Update definition
====================
*/
void AF_Input_Update(AF_Input* _input) {
    if (_input) {}

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    //glfwPollEvents();
}


/*
====================
AF_Input_Shutdown
Shutdown definition
====================
*/
void AF_Input_Shutdown(void) {

}
