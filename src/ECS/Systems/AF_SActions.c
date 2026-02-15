#include "ECS/Systems/AF_SActions.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_InputAction.h"

void AF_SActions_MouseLook(AF_AppData* _appData, uint32_t _cameraID, AF_FLOAT _mouseLookSensitivity){
    AF_Input* input =  &_appData->input;
	AF_ECS* ecs = &_appData->ecs;
	AF_CCamera* _camera = &ecs->cameras[_cameraID];
    // update the camera based on the pitch and yaw of the mouse;
    float xPos = input->mouseX;
    float yPos = input->mouseY;
    
    // Check if this is the first frame of holding down the right mouse button
    if (input->firstMouse == AF_TRUE) {
        input->lastMouseX = xPos;
        input->lastMouseY = yPos;
        input->firstMouse = AF_FALSE; // Mark as initialized to avoid resetting on subsequent frames
        return; // Skip this frame to prevent any snapping
    }

    float xoffset = xPos - input->lastMouseX;
    float yoffset = input->lastMouseY - yPos;

    input->lastMouseX = xPos;
    input->lastMouseY = yPos;

    xoffset *= _mouseLookSensitivity;
    yoffset *= _mouseLookSensitivity;
    AF_CTransform3D* cameraTransform = &ecs->transforms[_cameraID];
    
    _camera->yaw -= xoffset;
    _camera->pitch += yoffset;

    if(_camera->yaw > 180.0f){
        _camera->yaw -= 360.0f;
    }
    if(_camera->yaw < -180.0f){
        _camera->yaw += 360.0f;
    }

    // clamp pitch to avoid gimbal lock
    if(_camera->pitch > 89.0f){
        _camera->pitch = 89.0f; 
    }

    if(_camera->pitch < -89.0f){
        _camera->pitch = -89.0f; 
    }

    // convert yaw/pitch to quaternion
    Vec3 rotRadians = {
        AF_Math_Radians(_camera->pitch),  // pitch
        AF_Math_Radians(_camera->yaw),  // yaw
        0.0f   // roll (always 0 for FPS camera)
    };

    cameraTransform->rot = AF_Vec4_EulerToQuaternion(rotRadians);

    
    
}
