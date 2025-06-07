/*
===============================================================================
AF_RENDERINGDATA_H defninitions

Implementation of logging helper functions for the game
Calls vfprintf but adds some colour to text output
===============================================================================
*/

#ifndef AF_RENDERINGDATA_H
#define AF_RENDERINGDATA_H

#include "AF_Lib_Define.h"
#include "AF_Window.h"
#include "AF_LightingData.h"
#include "AF_Vec3.h"
#include "AF_FrameBufferData.h"
typedef void (*WindowFuncPtr)(void*);

#ifdef __cplusplus
extern "C" {
#endif


// =============== Rendering Data =============== 
typedef enum AF_Renderer_e {
    AF_RENDERER_OPENGL = 0,
    AF_RENDERER_DIRECTX = 1,
    AF_RENDERER_VULKAN = 2
} AF_Renderer_e;

typedef struct AF_Renderer_t{ 
    AF_Renderer_e key;
    const char* name;
} AF_Renderer_t;

#define AF_RENDERER_COUNT 3
static const AF_Renderer_t AF_Renderer_Mappings[AF_RENDERER_COUNT] = {
    {AF_RENDERER_OPENGL, "OpenGL"},
    {AF_RENDERER_DIRECTX, "DirectX"},
    {AF_RENDERER_VULKAN, "Vulkan"}
};

// Define the renderer mappings
extern const AF_Renderer_t AF_Renderer_Mappings[];

// =============== Renderer Pipeline Data =============== 
typedef enum AF_RendererPipeline_e {
    AF_RENDERER_FORWARD = 0,
    AF_RENDERER_FORWARD_PLUS = 1,
    AF_RENDERER_DEFERRED = 2
} AF_RendererPipeline_e;

typedef struct AF_RendererPipeline_t{ 
    AF_RendererPipeline_e key;
    const char* name;
} AF_RendererPipeline_t;

#define AF_RENDERER_PIPELINE_COUNT 3
static const AF_RendererPipeline_t AF_RendererPipeline_Mappings[AF_RENDERER_PIPELINE_COUNT] = {
    {AF_RENDERER_FORWARD, "Forward"},
    {AF_RENDERER_FORWARD_PLUS, "Forward+"},
    {AF_RENDERER_DEFERRED, "Deferred"}
};

typedef enum AF_Renderer_PolygonMode_e {
    AF_RENDERER_POLYGON_MODE_FILL = 0,
    AF_RENDERER_POLYGON_MODE_POINT = 1,
    AF_RENDERER_POLYGON_MODE_LINE = 2
} AF_Renderer_PolygonMode_e;

// Define the renderer mappings
extern const AF_RendererPipeline_t AF_RendererPipeline_Mappings[];


// =============== Rendering Data Struct =============== 
typedef struct AF_RenderingData{
    AF_Renderer_e rendererType;
    AF_RendererPipeline_e rendererPipelineType;
    // Frame Buffer
    AF_FrameBufferData screenFrameBufferData;
    //uint32_t screenFBO_ID;
    //uint32_t screenRBO_ID;
    //uint32_t screenFBO_TextureID;
    //uint32_t screenFBO_ShaderID;
    uint32_t screenQUAD_VAO;
    uint32_t screenQUAD_VBO;
    /// Depth Buffer
    AF_FrameBufferData depthFrameBufferData;
    //uint32_t depthFBO_ID;
    //uint32_t depthRBO_ID;
    //uint32_t depthMapTextureID;
    //uint32_t depthRenderShaderID;
    // Debug depth
    AF_FrameBufferData depthDebugFrameBufferData;
    //uint32_t depthDebugFBO_ID;
    //uint32_t depthDebugRBO_ID;
    //uint32_t depthDebugTextureID;
    //uint32_t depthDebugShaderID;
    AF_Renderer_PolygonMode_e polygonMode;
    WindowFuncPtr frameResizeFnctPtr;
    AF_Window* windowPtr;
    // In AF_RenderingData
    uint16_t viewportTextureWidth;  // The width the FBO texture should be / currently is
    uint16_t viewportTextureHeight; // The height the FBO texture should be / currently is
    af_bool_t     viewportSizeDirty;     // Flag if it needs to be recreated
} AF_RenderingData;

// ================ Screen FBO Data ================
#define SCREEN_VERT_SHADER_PATH "screenFrameBuffer.vert"
#define SCREEN_FRAG_SHADER_PATH "screenFrameBuffer.frag"

// ================ Depth Data ================
#define DEPTH_VERT_SHADER_PATH "depth.vert"
#define DEPTH_FRAG_SHADER_PATH "depth.frag"
#define DEPTH_DEBUG_VERT_SHADER_PATH "debugDepth.vert"
#define DEPTH_DEBUG_FRAG_SHADER_PATH "debugDepth.frag"


#define AF_RENDERINGDATA_SHADOW_WIDTH 1024
#define AF_RENDERINGDATA_SHADOW_HEIGHT 1024

/*
================
AF_AF_RenderingData_ZERO
// Construct and return the rendering data
================
*/
static inline AF_RenderingData AF_RenderingData_ZERO(void){
	AF_RenderingData returnRenderingData = {
		.rendererType = (AF_Renderer_e)0,
        .rendererPipelineType = (AF_RendererPipeline_e)0,
        .screenFrameBufferData = AF_FrameBufferData_ZERO(),
        .screenQUAD_VAO = 0,
        .screenQUAD_VBO = 0,
        .depthFrameBufferData = AF_FrameBufferData_ZERO(),
        .depthDebugFrameBufferData = AF_FrameBufferData_ZERO(),
        .polygonMode = AF_RENDERER_POLYGON_MODE_FILL,
        .frameResizeFnctPtr = NULL,
        .windowPtr = NULL,
        // In AF_RenderingData
        .viewportTextureWidth = 0,  // The width the FBO texture should be / currently is
        .viewportTextureHeight = 0, // The height the FBO texture should be / currently is
        .viewportSizeDirty = AF_FALSE    // Flag if it needs to be recreated
	};

	return returnRenderingData;
}

/*
================
AF_Renderer_ConvertToCharArray
// Construct and return the renderer as an array of chars
================
*/
inline static void AF_Renderer_ConvertToCharArray(const AF_Renderer_t* _mappings, const char** _charArray, uint32_t _size) {
    if (!_mappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _mappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}

/*
================
AF_RendererPipeline_ConvertToCharArray
// Construct and return the renderer pipelines as an array of chars
================
*/
inline static void AF_RendererPipeline_ConvertToCharArray(const AF_RendererPipeline_t* _mappings, const char** _charArray, uint32_t _size) {
    if (!_mappings || !_charArray) return;  // Null pointer check

    for (uint32_t i = 0; i < _size; i++) {
        _charArray[i] = _mappings[i].name;
    }

    _charArray[_size] = NULL;  // Null-terminate the array
}
#ifdef __cplusplus
}
#endif

#endif  // AF_RENDERINGDATA_H
