/*
===============================================================================
AF_CCAMERA_H definitions

Definition for the camera component struct
and camera helper functions
===============================================================================
*/
#ifndef AF_CCAMERA_H
#define AF_CCAMERA_H
#include "AF_Math.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "AF_Mat4.h"
#include "AF_Window.h"
#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
====================
AF_CCamera
Camera struct
====================
*/
typedef struct  {
    // TODO pack this
	PACKED_CHAR enabled;
    Vec3 cameraFront;
    Vec3 cameraUp;
    Vec3 cameraRight;
    Vec3 cameraWorldUp;
    AF_FLOAT yaw;
    AF_FLOAT pitch;
    AF_FLOAT fov;
    AF_FLOAT nearPlane;
    AF_FLOAT farPlane;
    AF_FLOAT aspectRatio;
    AF_FLOAT windowWidth;
    AF_FLOAT windowHeight;
    AF_FLOAT tanHalfFov;
    AF_FLOAT rangeInv;
    BOOL orthographic;
    Mat4 projectionMatrix;
    Mat4 viewMatrix;
    Vec4 backgroundColor;
} AF_CCamera;

/*
====================
AF_CCamera_ZERO
Initialisation constructor function
====================
*/
inline static AF_CCamera AF_CCamera_ZERO(void){
	PACKED_CHAR component = FALSE;
	component = AF_Component_SetHas(component, FALSE);
	component = AF_Component_SetEnabled(component, FALSE);
	AF_CCamera returnCamera = {
		.enabled = component,
		.cameraFront = {0,0,0},
		.cameraUp = {0,0,0},
		.cameraRight = {0,0,0},
		.cameraWorldUp = {0,0,0},
		.yaw = 0,
		.pitch = 0,
		.fov = 0,
		.nearPlane = 0,
		.aspectRatio = 0,
		.windowWidth = 0,
		.windowHeight = 0,
		.tanHalfFov = 0,
		.rangeInv = 0,
		.orthographic = FALSE,
		.projectionMatrix =  {{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
	}},
		.viewMatrix = {{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
	}},
		.backgroundColor = {0,0,0,0} 
	};

	return returnCamera;
}
/*
====================
AF_CCamera_ADD
Add the camera component
Initialise with enable and has set to true
====================
*/
inline static AF_CCamera AF_CCamera_ADD(BOOL _isOrthographic){
		
	PACKED_CHAR component = FALSE;
	component = AF_Component_SetHas(component, TRUE);
	component = AF_Component_SetEnabled(component, TRUE);
	AF_CCamera returnCamera = {
		.enabled = component,
		.cameraFront = {0, 0, -1},
		.cameraUp = {0, 1, 0},
		.cameraRight =  {0,0,0},
		.cameraWorldUp = {0,1,0},
		.yaw = 90,
		.pitch = 0,
		.fov = 45,
		.nearPlane = 0.1f,
		.farPlane = 100.0f,
		.aspectRatio = 0,
		.windowWidth = 0,
		.windowHeight = 0,
		.tanHalfFov = AF_Math_Tan(45 / 2), //AF_Math_Tan(halfFov)
		.rangeInv = 1 / 100 - 0, // 1/farPlane - nearPlane
		.orthographic = _isOrthographic,
		.projectionMatrix = {{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
		}},
		.viewMatrix = {{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
		}},
		.backgroundColor = {0,0,0,0} 
	};
	return returnCamera;
}

/*
====================
AF_Ccamera_GetOrthographicProjectMatrix
Setup a camera component struct with settings for orthographic camera
====================
*/

inline static Mat4 AF_Camera_GetOrthographicProjectionMatrix(AF_Window* _window, AF_CCamera* _camera) {
    if(_window){}
    // Get the framebuffer width and height as we work in pixels
    //_camera->windowWidth = _window->frameBufferWidth;
    //_camera->windowHeight = _window->frameBufferHeight;
    _camera->fov = 45;
    _camera->nearPlane = 0.1f;  // Set to a small positive value
    _camera->farPlane = 100.0f;
    _camera->aspectRatio = _camera->windowWidth / _camera->windowHeight;
    _camera->tanHalfFov = AF_Math_Tan(_camera->fov / 2);
    _camera->rangeInv = 1 / (_camera->farPlane - _camera->nearPlane);

    AF_FLOAT orthoWidth = 10.0f;
    AF_FLOAT orthoHeight = orthoWidth / _camera->aspectRatio;
    AF_FLOAT right = orthoWidth / 2;
    AF_FLOAT left = -orthoWidth / 2;
    AF_FLOAT top = orthoHeight / 2;
    AF_FLOAT bottom = -orthoHeight / 2;

    // Set the elements of the orthographic projection matrix
    Mat4 orthMatrix = {{
        {2.0f / (right - left), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (top - bottom), 0.0f, 0.0f},
        {0.0f, 0.0f, -2.0f / (_camera->farPlane - _camera->nearPlane), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};

    return orthMatrix;
}

/*
====================
AF_Ccamera_GetPerspectiveProjectionMatrix
Setup a camera component struct with settings for a perspective camera
====================
*/

inline static Mat4 AF_Camera_GetPerspectiveProjectionMatrix(AF_CCamera* _camera) {
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix.html
    // Get the framebuffer width and height as we work in pixels
    // Set perspective camera settings
    _camera->aspectRatio = (float)_camera->windowWidth / (float)_camera->windowHeight;
    
    // Convert FOV to radians and calculate tanHalfFov
    _camera->tanHalfFov = AF_Math_Tan(AF_Math_Radians(_camera->fov) / 2);
    
    // Calculate other elements for the projection matrix
    AF_FLOAT right = _camera->nearPlane * _camera->tanHalfFov;
    AF_FLOAT left = -right;
    AF_FLOAT top = right / _camera->aspectRatio;
    AF_FLOAT bottom = -top;
    
    // Set elements of the perspective projection matrix
    /*
    Mat4 perspectiveMatrix = {{
        {2.0f * _camera->nearPlane / (right - left), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f * _camera->nearPlane / (top - bottom), 0.0f, 0.0f},
        {(right + left) / (right - left), (top + bottom) / (top - bottom), -( _camera->farPlane + _camera->nearPlane ) / (_camera->farPlane - _camera->nearPlane), -1.0f},
        {0.0f, 0.0f, -2.0f * _camera->farPlane * _camera->nearPlane / (_camera->farPlane - _camera->nearPlane), 0.0f}
    }};
    */

   // TODO: Make this switch depending if its orthographic or perspective
   /*
   Mat4 perspectiveMatrix = {{
        {2.0f * _camera->nearPlane / (right - left), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f * _camera->nearPlane / (top - bottom), 0.0f, 0.0f},
        {0.0f, 0.0f, -( _camera->farPlane + _camera->nearPlane ) / (_camera->farPlane - _camera->nearPlane), -2.0f * _camera->farPlane * _camera->nearPlane / (_camera->farPlane - _camera->nearPlane)},
        {0.0f, 0.0f, -1.0f, 0.0f}
    }};*/


    // Frustum boundaries (in view space)
    float l = left;      // Left clipping plane (e.g., -screen aspect ratio)
    float r = right;     // Right clipping plane (e.g., screen aspect ratio)
    float b = bottom;    // Bottom clipping plane (e.g., -1.0f)
    float t = top;       // Top clipping plane (e.g., 1.0f)
    float n = _camera->nearPlane;      // Near clipping plane (positive value, e.g., 0.1f)
    float f =  _camera->farPlane;       // Far clipping plane (positive value, e.g., 100.0f)
    Mat4 perspectiveMatrix = {{
        // Column 0                   Column 1                   Column 2          Column 3
        {2*n/(r-l),    0.0f,          0.0f,                      0.0f},           // Row 0
        {0.0f,         2*n/(t-b),     0.0f,                      0.0f},           // Row 1
        {0.0f,         0.0f,          -(f+n)/(f-n),              -2*f*n/(f-n)},           // Row 2
        {0.0f,         0.0f,          -1.0f,                     0.0f}             // Row 3
    }};

    
    return perspectiveMatrix;
}





/*
====================
AF_Camera_CalculateFront
Function to calculate the camera from based on input of yaw and pitch
likely set from the mouse x, y coords
====================
*/
static inline Vec3 AF_Camera_CalculateFront(AF_FLOAT _yaw, AF_FLOAT _pitch){
    // calculate the new Front vector
    AF_FLOAT x = AF_Math_Cos(AF_Math_Radians(_yaw)) * AF_Math_Cos(AF_Math_Radians(_pitch));
    AF_FLOAT y = AF_Math_Sin(AF_Math_Radians(_pitch));
    AF_FLOAT z = AF_Math_Sin(AF_Math_Radians(_yaw)) * AF_Math_Cos(AF_Math_Radians(_pitch));
    Vec3 normalisedVec3 = {x, y, z};
    
    Vec3 newFront= {0,0,0}; 
    newFront = Vec3_NORMALIZE(normalisedVec3);

    return newFront;
}



#ifdef __cplusplus
}
#endif

#endif //AF_CCAMERA_H
