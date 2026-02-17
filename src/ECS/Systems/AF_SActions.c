#include "ECS/Systems/AF_SActions.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_InputAction.h"

void AF_SActions_MouseLook(AF_AppData* _appData, uint32_t _cameraID, AF_FLOAT _mouseLookSensitivity){
    // Get the variables setup ready for the mouse look system
    AF_Input* input =  &_appData->input;
	AF_ECS* ecs = &_appData->ecs;
	AF_CCamera* _camera = &ecs->cameras[_cameraID];
    float xPos = input->mouseX;
    float yPos = input->mouseY;
    AF_CTransform3D* cameraTransform = &ecs->transforms[_cameraID];


    // Check if this is the first frame of holding down the right mouse button
    if (input->firstMouse == AF_TRUE) {
        input->lastMouseX = xPos;
        input->lastMouseY = yPos;
        input->firstMouse = AF_FALSE; // Mark as initialised to avoid resetting on subsequent frames

        AF_Log("AF_Project: Yaw: %f Pitch: %f\n", _camera->yaw, _camera->pitch);
        return; // Skip this frame to prevent any snapping
    }

    // Calculate the offset of the mouse movement since the last frame
    float xoffset = xPos - input->lastMouseX;
    float yoffset = input->lastMouseY - yPos;

    // Update last mouse position for the next frame
    input->lastMouseX = xPos;
    input->lastMouseY = yPos;

    // Apply sensitivity
    xoffset *= _mouseLookSensitivity;
    yoffset *= _mouseLookSensitivity;
    
    // Update the camera's yaw and pitch based on mouse movement
    _camera->yaw -= xoffset;
    _camera->pitch += yoffset;

    // Wrap yaw between -180 and 180 degrees to prevent overflow issues 
    if(_camera->yaw > 180.0f){
        _camera->yaw -= 360.0f;
    }
    
    // Wrap yaw between -180 and 180 degrees to prevent overflow issues   if(_camera->yaw < -180.0f){
    if(_camera->yaw < -180.0f){
        _camera->yaw += 360.0f;
    }

    // clamp pitch to avoid gimbal lock
    if(_camera->pitch > 89.0f){
        _camera->pitch = 89.0f; 
    }

    // clamp pitch to avoid gimbal lock
    if(_camera->pitch < -89.0f){
        _camera->pitch = -89.0f; 
    }
    
    // Build quaternion as Yaw * Pitch * Roll
    AF_FLOAT yawRad = AF_Math_Radians(_camera->yaw);
    AF_FLOAT pitchRad = AF_Math_Radians(_camera->pitch);
    
    // convert the yaw and pitch to a quaternion
    Vec4 qYaw = {0, sinf(yawRad * 0.5f), 0, cosf(yawRad * 0.5f)};
    Vec4 qPitch = {sinf(pitchRad * 0.5f), 0, 0, cosf(pitchRad * 0.5f)};

    // combine the yaw and pitch quaternions. 
    // Note that the order of multiplication matters. We want to apply yaw first, then pitch, so we multiply in reverse order.
    cameraTransform->rot = AF_Vec4_Quat_MULT(qYaw, qPitch);
}
