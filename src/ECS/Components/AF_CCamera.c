#include "ECS/Components/AF_CCamera.h"
#include "ECS/Components/AF_Component.h"
/*
====================
AF_CCamera_ZERO
Initialisation constructor function
====================
*/
AF_CCamera AF_CCamera_ZERO(void){
	PACKED_CHAR component = FALSE;
	component = AF_Component_SetHas(component, FALSE);
	component = AF_Component_SetEnabled(component, FALSE);
	AF_CCamera returnCamera = {
		.enabled = component,
		.cameraFront = {0,0,0},
		.cameraUp = {0,0,0},
		.cameraRight = {0,0,0},
		.cameraWorldUp = {0,1,0},
		.yaw = 0,
		.pitch = 0,
		.fov = 0,
		.nearPlane = 0,
		.aspectRatio = 0,
		//.windowWidth = 0,
		//.windowHeight = 0,
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
AF_CCamera AF_CCamera_ADD(BOOL _isOrthographic){
		
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
		//.windowWidth = 0,
		//.windowHeight = 0,
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

Mat4 Mat4_Ortho(float _left, float _right, float _bottom, float _top, float _nearPlane, float _farPlane) {
    // Set the elements of the orthographic projection matrix
    /*
    Mat4 orthMatrix = {{
        {2.0f / (_right - _left), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (_top - _bottom), 0.0f, 0.0f},
        {0.0f, 0.0f, -2.0f / (_farPlane - _nearPlane), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    */
    //Calculate scaling factors
    float scaleX = 2.0f / (_right - _left);
    float scaleY = 2.0f / (_top - _bottom);
    float scaleZ = -2.0f / (_farPlane - _nearPlane);

    // Calculate translation factors
    float transX = -(_right + _left) / (_right - _left);
    float transY = -(_top + _bottom) / (_top - _bottom);
    float transZ = -(_farPlane + _nearPlane) / (_farPlane - _nearPlane);

    // Set the elements of the orthographic projection matrix
    // Assuming column-major layout (like GLM, OpenGL expects): Column[Row]
    // Mat4 orthMatrix = { Column0, Column1, Column2, Column3 }
    // Or if row-major {{R0C0,R0C1,R0C2,R0C3}, {R1C0,...}, ...} - adjust below accordingly
   /*
    Mat4 orthMatrix = {{
        {scaleX,    0.0f,      0.0f,     0.0f}, // Column 0
        {0.0f,      scaleY,    0.0f,     0.0f}, // Column 1
        {0.0f,      0.0f,      scaleZ,   0.0f}, // Column 2
        {transX,    transY,    transZ,   1.0f}  // Column 3 (Translation)
    }};
     */
    // --- OR --- If your Mat4 initializer is row-major {{Row0}, {Row1}, ...}:
    
    Mat4 orthMatrix = {{
        {scaleX,    0.0f,    0.0f,      transX},   // Row 0
        {0.0f,      scaleY,  0.0f,      transY},   // Row 1
        {0.0f,      0.0f,    scaleZ,    transZ},   // Row 2
        {0.0f,      0.0f,    0.0f,      1.0f}     // Row 3
    }};
   

    return orthMatrix;
}

/*
====================
AF_Ccamera_GetPerspectiveProjectionMatrix
Setup a camera component struct with settings for a perspective camera
====================
*/
Mat4 AF_Camera_GetPerspectiveProjectionMatrix(AF_CCamera* _camera, AF_FLOAT _windowWidth, AF_FLOAT _windowHeight) {
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix.html
    // Get the framebuffer width and height as we work in pixels
    // Set perspective camera settings
    //_camera->aspectRatio = (float)_camera->windowWidth / (float)_camera->windowHeight;
    _camera->aspectRatio = (float)_windowWidth / (float)_windowHeight;
    
    // Convert FOV to radians and calculate tanHalfFov
    _camera->tanHalfFov = AF_Math_Tan(AF_Math_Radians(_camera->fov) / 2);
    
    // Calculate other elements for the projection matrix
    AF_FLOAT right = _camera->nearPlane * _camera->tanHalfFov;
    AF_FLOAT left = -right;
    AF_FLOAT top = right / _camera->aspectRatio;
    AF_FLOAT bottom = -top;
    
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
Vec3 AF_Camera_CalculateFront(AF_FLOAT _yaw, AF_FLOAT _pitch){ //
    // calculate the new Front vector
    AF_FLOAT x = AF_Math_Cos(AF_Math_Radians(_yaw)) * AF_Math_Cos(AF_Math_Radians(_pitch));
    AF_FLOAT y = AF_Math_Sin(AF_Math_Radians(_pitch));
    AF_FLOAT z = AF_Math_Sin(AF_Math_Radians(_yaw)) * AF_Math_Cos(AF_Math_Radians(_pitch));
    Vec3 normalisedVec3 = {x, y, z};
    
    Vec3 newFront= {0,0,0}; 
    newFront = Vec3_NORMALIZE(normalisedVec3);

    return newFront;
}

