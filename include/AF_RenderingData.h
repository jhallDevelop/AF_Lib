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
#include "AF_LightingData.h"
#include "AF_Vec3.h"

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

// Define the renderer mappings
extern const AF_RendererPipeline_t AF_RendererPipeline_Mappings[];


// =============== Rendering Data Struct =============== 
typedef struct AF_RenderingData{
	// TODO pack this
    AF_Renderer_e rendererType;
    AF_RendererPipeline_e rendererPipelineType;
    AF_LightingData lightingData;
} AF_RenderingData;

/*
================
AF_AF_RenderingData_ZERO
// Construct and return the rendering data
================
*/
static inline AF_RenderingData AF_AF_RenderingData_ZERO(void){
	AF_RenderingData returnRenderingData = {
		.rendererType = (AF_Renderer_e)0,
        .rendererPipelineType = (AF_RendererPipeline_e)0,
        .lightingData = AF_AF_LightingData_ZERO()
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
