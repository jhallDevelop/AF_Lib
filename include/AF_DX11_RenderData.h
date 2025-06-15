#ifndef AF_DX11_RENDERDATA_H
#define AF_DX11_RENDERDATA_H
#include "d3d11_3.h" // DirectX 11.3 header for device context
#include "AF_Math/AF_Mat4.h"
//#include "DirectXMath.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct MVPBuffer
{
	Mat4 model;
	Mat4 view;
	Mat4 projection;
	//DirectX::XMFLOAT4X4 model;
	//DirectX::XMFLOAT4X4 view;
	//DirectX::XMFLOAT4X4 projection;
} MVPBuffer;

// Used to send per-vertex data to the vertex shader.
typedef struct DX11Vertex
{
	Vec3 pos;
	Vec3 color;
} DX11Vertex;

typedef struct AF_DX11_RenderData {
	// Create a struct to hold this data
	ID3D11DeviceContext3* context;
	ID3D11Buffer* constantBuffer;
	MVPBuffer* constantBufferData;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	uint32_t indexCount;
	D3D11_VIEWPORT* screenViewport;
	ID3D11RenderTargetView1* renderTargetView; // Added for render target view
	ID3D11DepthStencilView* depthStencilView; // Added for depth stencil view, if needed

} AF_DX11_RenderData;

static inline void AF_DX11_RenderData_Init(AF_DX11_RenderData* _renderData) {
	_renderData->context = NULL;
	_renderData->constantBuffer = NULL;
	_renderData->constantBufferData = NULL;
	_renderData->vertexBuffer = NULL;
	_renderData->indexBuffer = NULL;
	_renderData->inputLayout = NULL;
	_renderData->vertexShader = NULL;
	_renderData->pixelShader = NULL;
	_renderData->indexCount = 0;
	_renderData->screenViewport = NULL;
	_renderData->renderTargetView = NULL; // Initialize render target view to NULL
	_renderData->depthStencilView = NULL; // Initialize depth stencil view to NULL
}

#ifdef __cplusplus
}
#endif

#endif 