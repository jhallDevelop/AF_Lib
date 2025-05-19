/*
===============================================================================
AF_RENDERER_H

Definition for functions for rendering 
===============================================================================
*/
#ifndef AF_RENDERER_H
#define AF_RENDERER_H
#include "AF_Window.h"
#include "ECS/Components/AF_CCamera.h"
#include "AF_MeshData.h"
#include "ECS/Entities/AF_ECS.h"
#include "AF_LightingData.h"
#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_CSprite.h"
#include "AF_Debug.h"
#include "AF_Time.h"
#include "AF_Assets.h"
#include "AF_RenderingData.h"
#include "AF_FrameBufferData.h"

#ifdef __cplusplus
extern "C" {
#endif



// ============================ DEFAULT ================================ 
uint32_t AF_Renderer_Awake(void);
void AF_Renderer_Start(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight);
void AF_Renderer_Update(AF_ECS* _ecs, AF_Time* _time);
void AF_Renderer_Render(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_Entity* _cameraEntity);
void AF_Renderer_Finish(void);
void AF_Renderer_Shutdown(AF_ECS* _ecs);


// ============================ TEXTURES ================================ 
// Textures
void AF_Renderer_SetFlipImage(BOOL _flipImage);
uint32_t AF_Renderer_LoadTexture(char const * path);
AF_Texture AF_Renderer_ReLoadTexture(AF_Assets* _assets, const char* _texturePath);
void AF_Renderer_SetTexture(const uint32_t _shaderID, const char* _shaderVarName, uint32_t _textureID);

// ============================  Forward Rendering ================================ 
void AF_Renderer_StartForwardRendering(AF_ECS* _ecs, AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_Entity* _cameraEntity);
void AF_Renderer_EndForwardRendering(void);

// ============================  DRAW ================================ 
void AF_Renderer_StartRendering(Vec4 _backgroundColor);
void AF_Renderer_DrawMeshes(Mat4* _viewMat, Mat4* _projMat, AF_ECS* _ecs, Vec3* _camera, AF_LightingData* _lightingData, uint32_t _shaderOverride);
void AF_Renderer_DrawMesh(Mat4* _modelMat, Mat4* _viewMat, Mat4* _projMat, AF_CMesh* _mesh, AF_ECS* _ecs, Vec3* _camera, AF_LightingData* _lightingData, uint32_t _shaderOverride);
void AF_Render_DrawMeshElements(AF_ECS* _ecs, Mat4* _projection, Vec3* _cameraPos, uint32_t _shaderID);
void AF_Renderer_RenderScreenFBOQuad(AF_RenderingData* _renderingData);


// ============================  MESH BUFFERS ================================ 
void AF_Renderer_CreateScreenFBOQuadMeshBuffer(AF_RenderingData* _renderingData);
void AF_Renderer_InitMeshBuffers(AF_CMesh* _mesh, uint32_t _entityCount);
void AF_Renderer_CreateMeshBuffer(AF_MeshData* _meshData);  


// ============================  FRAME BUFFERS ================================ 
//void AF_Renderer_Start_ScreenFrameBuffers(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight);
void AF_Renderer_Start_ScreenFrameBuffers(uint32_t* _fbo, uint32_t* _rbo, uint32_t* _shaderID, uint32_t* _textureID,  uint16_t* _screenWidth, uint16_t* _screenHeight, const char* _vertPath, const char* _fragPath, const char* _shaderTextureName);

uint32_t AF_Renderer_CreateFBO(void);
void AF_Renderer_CreateFramebuffer(AF_FrameBufferData* _framebufferData);
uint32_t AF_Renderer_CreateRBO(void);
uint32_t AF_Renderer_CreateFBOTexture(AF_FrameBufferData* _framebufferData);
void AF_Renderer_BindFrameBuffer(uint32_t _fBOID);
void AF_Renderer_UnBindFrameBuffer(void);
void AF_Renderer_BindFrameBufferToTexture(uint32_t _fBOID, uint32_t _textureID, uint32_t _textureAttatchmentType);
void AF_Renderer_BindRenderBuffer(uint32_t _rbo, uint32_t _screenWidth, uint32_t _screenHeight);
void AF_Renderer_FrameResized(void* _renderingData);


// ============================  DEPTH ================================ 
void AF_Renderer_StartDepthPass(AF_RenderingData* _renderingData, AF_LightingData* _lightingData, AF_ECS* _ecs, AF_CCamera* _camera);
void AF_Renderer_Start_DepthFrameBuffers(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight);
void AF_Renderer_Start_DepthDebugFrameBuffers(AF_RenderingData* _renderingData, uint16_t* _screenWidth, uint16_t* _screenHeight);



// ============================  LIGHTING ================================ 
void AF_Renderer_RenderForwardPointLights(uint32_t _shader, AF_ECS* _ecs, AF_LightingData* _lightingData);
void AF_Renderer_UpdateLighting(AF_ECS* _ecs, AF_LightingData* _lightingData);

// ============================  ANIMATION ================================ 

void AF_Renderer_PlayAnimation(AF_CSkeletalAnimation* _animation);


// ============================  DESTROY / CLEANUP ================================ 
// Destroy
void AF_Renderer_DestroyMeshBuffers(AF_CMesh* _mesh);
void AF_Renderer_Destroy_Material_Textures(AF_Material* _material);
void AF_Renderer_DestroyRenderer(AF_RenderingData* _renderingData, AF_ECS* _ecs);
void AF_Renderer_DeleteFBO(uint32_t* _fboID);
void AF_Renderer_DeleteRBO(uint32_t* _rboID);
void AF_Renderer_DeleteTexture(uint32_t* _textureID);



// ====================================== HELPER FUNCTIONS =====================================
void AF_Renderer_CheckFrameBufferStatus(const char* _message);
void AF_Renderer_CheckError(const char* _message);
void AF_Renderer_SetPolygonMode(AF_Renderer_PolygonMode_e _polygonMode);






#ifdef __cplusplus
}
#endif

#endif // AF_RENDERER_H

// ========== JUNK / OLD FUNCTIONS FROM VULKAN ===========

//static unsigned int LoadTexture(char const * path);
//static void SetDiffuseTexture(const unsigned int _shaderID);
//static void SetSpecularTexture(const unsigned int _shaderID);
//static void SetEmissionTexture(const unsigned int _shaderID);
//static void SetEmissionMaskTexture(const unsigned int _shaderID);
// Cleanup
//static void CleanUpMesh(const unsigned int _shaderID);
//void CloseWindow();
//void CleanUp();
//void CleanupSwapChain();


// Error checking
// Util
//static void CheckGLError(std::string _message);
//bool CheckValidationLayerSupport();
//void SetupDebugMessager();
//void PickPhysicalDevice();

// Load Models
//void LoadModel(CModel& _model, std::vector<AF_Vertex>& _vertices, std::vector<uint32_t>& _indices);


// Getters and setters
//VkSampler& GetTextureSampler();
//std::vector<VkImageView>& GetImageViews();
//VkImageView& GetCurrentImageView();
//VkImageView& GetCurrentEditorImageView();
//void SetEditorMode(const bool _state);
//void SetViewportEditor(const bool _state);
//uint32_t GetDrawCalls() const;
//VkExtent2D& GetSwapChainExtent() const;
//bool GetFramebufferResized() const;
//void SetFramebufferResized(const bool _state);
//static glm::vec3 CalculateFront(const CTransform3D& _transform);
//std::vector<AF_Mesh>& getMeshes() const override;

//void DrawFrame(GLFWwindow* _window, Entity& _cameraEntity, std::vector<Entity*>& _entities);
//static void RenderMesh(const AF_Mesh& _mesh, const AF_Camera& _camera);
//uint32_t AF_LoadTexture(const char* _texturePath, BOOL _flipTexture);

//void Init(GLFWwindow* _window, std::vector<Entity*>& _entities);
//void InitRenderingData(std::vector<Entity*>& _entities);
//static AF_MeshBuffers InitBuffers(const AF_MeshBuffers& _bufferObject);
// vulkan things
//void CreateSurface(GLFWwindow* _window);
//void RecreateSwapChain(GLFWwindow* _window);
//VkDevice& GetDevice();
//VkPhysicalDevice& GetPhysicalDevice();
//VkInstance& GetInstance();
//VkQueue& GetQueue();
//VkRenderPass& GetRenderPass();
//VkSampleCountFlagBits& GetMSAASamples();
////void InitWindow(AF_AppData& _appData);
//void InitVulkan(GLFWwindow* _window, std::vector<Entity*>& _entities);
//void CreateInstance();
        
//void CreateLogicalDevice();
//void CreateSwapChain(GLFWwindow* _window);
//void CreateSurface();
//void CreateImageViews();
// EditorImageViews
//void CreateOffscreenImageResources();
//void CreateRenderPass();
//void CreateFrameBuffers();
//void CreateCommandPool();
//void CreateDepthResources();


//void CreateTextureImage(const char* _texturePath, std::vector<VkImage>& _textureImages, std::vector<VkDeviceMemory>& _textureImagesMemory, const uint32_t _levels, VkImageCreateFlagBits _flags, VkImageLayout _imageLayout);
//void CreateTextureImage(const char* _texturePath, VkImage& _textureImage, VkDeviceMemory& _textureImageMemory);
//void CreateTextureImageView(VkImage& _textureImage, VkImageView& _textureImageView, uint32_t _mipLevels);
//void CreateTextureImageView(std::vector<VkImage>& _textureImages, std::vector<VkImageView>& _textureImageViews, uint32_t _mipLevels, uint32_t _layers, VkImageViewType _imageViewType);
//void CreateTextureSampler(VkDevice& _device, VkPhysicalDevice& _physicalDevice, VkSampler& _textureSampler);
//void CreateTextureSampler(VkDevice& _device, VkPhysicalDevice& _physicalDevice, std::vector<VkImage>& _textureImages, std::vector<VkSampler>& _textureSamplers);


//void CreateVertexBuffer(VkDevice& _device, std::vector<AF_Vertex>& _vertices);
//void CreateIndexBuffer(VkDevice& _device, std::vector<uint32_t>& indices);
//void CreateUniformBuffers(VkDevice& _device, std::vector<VkBuffer>& _uniformBuffers, std::vector<VkDeviceMemory>& _uniformBuffersMemory, std::vector<void*>& _uniformBuffersMapped, const VkDeviceSize _uniformBufferSize);


//void CreateDescriptorPool();
//void CreateTextureDescriptorSets(VkDevice& _device, std::vector<VkImageView>& _modelTextureImagesView, std::vector<VkSampler>& _modelTextureSamplers );

//void CreateDescriptorSets();
//void CreateDescriptorSetLayout();
//void CreateGraphicsPipeline(std::vector<Entity*>& _entities);
//void CreateCommandBuffers();
//void CreateSyncObjects();
//void CreateColorResources();
//void CreateIMGUIRenderPass();
//void CreateOffscreenFramebuffer();

// Create Material
//AF_Material& CreateMaterial(const AF_Vec2 _screenDimensions, const std::string& _vertShaderPath, const std::string& _fragShaderPath, const std::string& _diffuseTexturePath) override;
//AF_Mesh& CreateMesh(const AF_Vec2& _cameraSize, const std::string& _vertShaderPath, const std::string& _fragShaderPath, const std::string& _texturePath)  override;


