/*
===============================================================================
AF_CCAMERA_H definitions

Definition for the camera component struct
and camera helper functions
===============================================================================
*/
#ifndef AF_CCAMERA_H
#define AF_CCAMERA_H
#include "AF_Math/AF_Math.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "AF_Math/AF_Mat4.h"
#include "AF_Window.h"
#include "AF_Lib_Define.h"
#include "AF_FrameBufferData.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AF_CCAMERA_DEFAULT_RENDER_TEXTURE_WIDTH 128
#define AF_CCAMERA_DEFAULT_RENDER_TEXTURE_HEIGHT 128

/*
====================
AF_CCamera
Camera struct
====================
*/
typedef struct  {
    // TODO pack this
	// Largest members first (matrices, vectors)
    Mat4 projectionMatrix;
    Mat4 viewMatrix;
    Vec3 cameraFront;
    Vec3 cameraUp;
    Vec3 cameraRight;
    Vec3 cameraWorldUp;
    Vec4 backgroundColor;
    AF_FrameBufferData renderTextureData;

    // Floats next
    AF_FLOAT yaw;   // horizontal rotation in degrees
    AF_FLOAT pitch; // vertical rotation in degrees
    AF_FLOAT fov;
    AF_FLOAT nearPlane;
    AF_FLOAT farPlane;
    AF_FLOAT aspectRatio;
    AF_FLOAT tanHalfFov;
    AF_FLOAT rangeInv;

    // Shorts
    uint16_t renderTextureWidth;
    uint16_t renderTextureHeight;

    // Smallest members last (chars/bools)
    PACKED_CHAR enabled;
    af_bool_t orthographic;
    af_bool_t enableRenderToTexture;
    uint32_t renderToTextureID;
} AF_CCamera;

AF_CCamera AF_CCamera_ZERO(void);
AF_CCamera AF_CCamera_ADD(af_bool_t _isOrthographic);
Mat4 Mat4_Ortho(float _left, float _right, float _bottom, float _top, float _nearPlane, float _farPlane);
Mat4 AF_Camera_GetPerspectiveProjectionMatrix(AF_CCamera* _camera, AF_FLOAT _windowWidth, AF_FLOAT _windowHeight);
Vec3 AF_Camera_CalculateFront(AF_FLOAT _yaw, AF_FLOAT _pitch);

#ifdef __cplusplus
}
#endif

#endif //AF_CCAMERA_H
