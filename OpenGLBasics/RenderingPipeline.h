#pragma once
#include "glPCH.h"
#include <unordered_map>
#include "Shader.h"
#include "VulkanShaders.h"
#include "TextureData.h"
#include "Light.h"
#include "VulkanPipelineBuilder.h"

class EntityComponent;
class Camera;
class GLSetup;
class Material;
class Mesh;
struct CubemapData;

#define MAX_LIGHT_COUNT 10

struct MVPBuffer
{
	DMat4x4 model;
	DMat4x4 view;
	DMat4x4 projection;
};

struct NormalBuffer
{
	DMat3x3 normalMat;
};


struct LightProperties {
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	DVector3 ambientColor;
	DVector3 mainColor;
	DVector3 position;
	DVector3 halfVector;
	DVector3 coneDirection;
	float spotCosineCutoff;
	float spotExponent;
	float attenuationConstant;
	float linearAttenuation;
	float quadraticAttenuation;
};
struct LightBuffer
{
	LightProperties lights[MAX_LIGHT_COUNT];
	int lightCount = MAX_LIGHT_COUNT;
	float shininess;
	float strength;	
};

struct ViewBuffer
{
	DVector3 lookDir;
};

struct ObjectPropertyBuffer
{
	DVector4 color;
};

struct UniformBufferParams
{		

	~UniformBufferParams()
	{
		auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();		
		vkFreeMemory(vkSettings->device, deviceMemory, vkSettings->allocationCallback);
		vkDestroyBuffer(vkSettings->device, buffer, vkSettings->allocationCallback);
	}

	VkMemoryRequirements memoryReqs;
	VkDeviceSize bufferSize = 0;
	VkBool32 instanceCount = 1;
	VkDeviceMemory deviceMemory;
	VkBuffer buffer;
	std::vector<void*> data;
};

struct CubemapParams
{
	VkPipeline pipeline;
	VkShaderModule shaderModule;
	VkImage img;
	VkImageView imgView;
	VkBuffer stagingBuffer;
	VkDeviceSize stagingBufferSize, layerSize;
	void* stagingData;
	std::vector<TextureData*> images;
	VkDeviceMemory imageMemory, stagingMemory;
};

struct RenderBufferData
{
	~RenderBufferData();
	bool hasTexCoords = false;
	Color color;
	GLuint vao, vbo, ebo;
	std::vector<DVertex> vertices;
	std::vector<VkDeviceSize> vertexOffsets{ 0 };
	std::vector<uint16_t> indices;
	DMat4x4 transform;
	std::shared_ptr<Shader> shader;
	VkShaderStageConfigs shaderConfigurations;
	VkPipelineBuilderParams pipelineBuilderParams;

	std::vector<VkVertexInputBindingDescription>inputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions;
	std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::vector<VkDescriptorSet> descriptorSets;
	std::unordered_map<VkShaderStageFlags, std::vector<VkDescriptorSetLayoutBinding>> bindingsPerShaderStage;
	std::unordered_map<uint16_t, uint16_t> uniformBufferMemoryOffsets;
	std::vector<UniformBufferParams> uniformBufferParamsForShader;
	std::vector<VkDeviceSize> uniformBuffersSize;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	UniformBufferParams vertDataBufParams;
	UniformBufferParams vertexBufParams;
	UniformBufferParams indexBufParams;

	MVPBuffer mvpBuffer;
	NormalBuffer normalBuffer;
	LightBuffer lightBuffer;
	ViewBuffer viewBuffer;
	ObjectPropertyBuffer objectBuffer;

	UniformBufferParams mvpParams;
	UniformBufferParams normalParams;
	UniformBufferParams lightParams;
	UniformBufferParams viewParams;	
	UniformBufferParams objParams;

	VkPipeline graphicsPipeline;
	VkPipelineCache pipelineCache;
};

static VkDescriptorSetLayoutBinding defaultVertexMVPDescriptorLayout = {
	.binding = 0,
	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	.descriptorCount = 1,
	.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
	.pImmutableSamplers = nullptr
};

static VkDescriptorSetLayoutBinding defaultVertexNormalsDescriptorLayout = {
	.binding = 1,
	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	.descriptorCount = 1,
	.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
	.pImmutableSamplers = nullptr
};

static VkDescriptorSetLayoutBinding defaultFragmentLightDescriptorLayout = {
	.binding = 2,
	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	.descriptorCount = 1,
	.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
	.pImmutableSamplers = nullptr
};

static VkDescriptorSetLayoutBinding defaultFragmentViewDescriptorLayout = {
	.binding = 3,
	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	.descriptorCount = 1,
	.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
	.pImmutableSamplers = nullptr
};

static VkDescriptorSetLayoutBinding defaultFragmentObjectDescriptorLayout = {
	.binding = 4,
	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	.descriptorCount = 1,
	.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
	.pImmutableSamplers = nullptr
};

static std::vector<VkDescriptorSetLayoutBinding> defaultDescriptorLayoutBindings = {
	defaultVertexMVPDescriptorLayout,
	defaultVertexNormalsDescriptorLayout,
	defaultFragmentLightDescriptorLayout,
	defaultFragmentViewDescriptorLayout,
	defaultFragmentObjectDescriptorLayout
};

// Rendering pipeline to carry out rendering tasks requested on multiple platforms
class BRenderingPipeline final
{
	BRenderingPipeline();
	~BRenderingPipeline();
	void CleanupRenderingPipeline();
	// Initialization function for setting up default framebuffer and renderbuffer
	void Init();
	/** Vertex specification Functions*/
	// Imports mesh data for pipeline to render 
	void Import(std::string primitiveName, std::vector<DVertex>_vertices, std::vector<uint16_t>_indices);
	void Import(std::string primitiveName, Mesh* mesh);
	// Instructs the vulkan graphics pipeline on how to read the vertex buffer data
	void LoadVertexReadingFormatToVkPipeline(std::string primitiveName);
	// Allocates memory on application to allow mesh vertex data to fed to pipeline
	void CreateVkVertexBuffer(std::string primitiveName);
	// Allocates memory on application to allow mesh index buffer data to fed to pipeline
	void CreateVkIndexBuffer(std::string primitiveName);
	// Allocates memory for uniform buffers
	void CreateVkUniformBuffers(std::string primitiveName);
	// Fills vertex buffer with vertex data
	void FillVkVertexBuffer(std::string primitiveName);
	// Fills index buffer with index data for mesh
	void FillVkIndexBuffer(std::string primitiveName);
	// Fills uniform buffers with binding data
	void FillVkUniformBuffers(std::string primitiveName);
	// Sets the descriptor layouts for the uniform buffers on shaders
	void SetVkDescriptorForUniformBuffers(std::string primitiveName, std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings = defaultDescriptorLayoutBindings);
	// Adds shader stage file(s) to be used and read by the graphics pipeline
	void LoadVkShaderStages(std::string primitiveName, VkShaderStageConfigs& shaderStages);
	// Sets the depth info for the depth/stencil state of the current graphics pipeline
	void SetVkPipelineDepthState(std::string primitiveName, VkCompareOp comparisonOperation, bool isDepthBoundsEnabled, float minDepthBounds = 0.0f, float maxDepthBounds = 1.0f);
	// Sets the stencil info for depth/stencil state of the current graphics pipeline
	void SetVkPipelineStencilState(std::string primitiveName, VkStencilOpState frontStencilState, VkStencilOpState backStencilState);
	// Creates pipeline layout from descriptor set layout(s)
	void CreatePipelineLayout(std::string primitiveName);
	// Creates a vulkan rendering pipeline from start to finish
	void CreateDefaultGraphicsPipeline(std::string primitiveName);
	// Creates a cubemap from texture data
	void GenerateCubemap(std::vector<TextureData*>cubemapTextureData);
	// Create pipeline for cubemap
	void CreateCubemapPipeline();
	// Feeds mesh data to rendering platform
	void RequestForMeshVertexData(std::string primitiveName);
	// Feeds vertex data from skybox to rendering platform
	void RequestForDefaultSkyboxVerts();
	void RequestForSkyboxVerts(std::string skyboxName, CubemapData* vertices);
	// Toggles projection from perspective to othrographic
	void SetOrthoView(bool isOrtho);
	// Updates the transform of the primitive
	void UpdateModelMatrix(DMat4x4 _transform, std::string primitiveName);
	// Updates the world space matrix
	void UpdateProjectionMatrix(float fieldOfView, float width, float height, float nearClippingPlane, float farClippingPlane);
	// Updates the view matrix (matrix respresenting the camera and its view)
	void UpdateViewMatrix(DMat4x4 _view);
	// Updates MVP matrix of primitive
	void UpdateTransformMatrix(std::string primitiveName);
	// Updates the MVP matrices of all primitives in the scene
	void UpdateTransforms();
	// Send mvp data to shader(s) tied to primitive
	void UpdateTransformOnGpu(std::string primitive);
	// Send mvp data to shader(s) tied to all primitives in scene
	void UpdateAllTransformsOnGpu();
	/** Vertex Shading Functions */
	// Create default mesh shader
	void CreateDefaultShader();
	// Adds or updates shader to be used by primitive
	void LoadShader(std::string primitiveName, Material* mat);
	// Getter function for default shader
	void GetDefaultShader(Shader* defaultShader);
	// Adds shader to cubemap
	void LoadDefaultCubemapShader();
	// Adds shader to the cache
	void UpdateShaderCache(Shader* shaderToAdd);
	void UpdateShaderCache(Material* shaderToAdd);
	void UpdateDefaultCubemapShader();
	// Request to add a light to the scene
	bool RequestForLight(Light& light);
	// Passes in light data for shader
	void UpdateLightDataForShader(RenderBufferData* renderData);
	/** Vertex Post Processing Functions */

	/** DRAWING FUNCTIONS */
	// Generates the default framebuffer
	void GenerateDefaultFramebuffer();
	// Generates a Vulkan framebuffer
	void GenerateVkFrameBuffers();
	// Sends viewport info to graphics pipeline based on screen resolution
	void SetViewportInfo(VkCommandBuffer cmdBuffer);
	// Adds vertex buffers to command buffer 
	void DrawVk(VkCommandBuffer cmdBuffer);
	// Adds draw commands to command buffer with indexed vertex buffer data
	void DrawVkIndexed(VkCommandBuffer cmdBuffer);
	// Grabs the Vulkan framebuffer at the requested index
	void GetVkFramebuffer(VkFramebuffer& framebuf, VkBool32 frameBufIndex);
	void ResizeScreen(int width, int height);
	// Loads in framebuffer
	// set to 0 for default framebuffer
	void LoadGLFramebuffer(GLuint fbID);
	// Loads current framebuffer
	void LoadCurrentFramebuffer();
	//Unloads framebuffer
	void UnloadFramebuffer();
	// Draws all primitives to the screen for one frame
	void RenderPrimitives();
	// Draw function for rendering meshes
	void DrawMesh(RenderBufferData* renderData);
	// Draw function for rendering cubemaps
	void DrawCubeMap();

private:
	// TODO: Create a class/struct that instructs the pipeline 
	// what the intended usage of the framebuffer is
	GLuint defaultFramebuffer, currentFramebuffer, renderbuffer;
	std::vector<VkViewport> viewports;
	std::vector<VkRect2D> scissors;
	CubemapParams vk_cubemapParams;
	CubemapData* cubemapParams;
	VkImageView cubemapView;
	VkImage cubemap;
	Shader* cubemapShader;
	GLuint defaultFrameBufferTextureID;
	VkExtent2D screenResolution;
	std::vector<GLuint> glFramebuffers;
	std::vector<VkFramebuffer> vkFramebuffers;
	VkPipelineBuilder* vulkanPipelineBuilder;
	std::unordered_map<std::string, RenderBufferData*>primitives;
	std::vector<Light*> lightCache;
	DMat4x4 projectionMatrix, viewMatrix;
	bool perspectiveView = true;
	std::vector<std::shared_ptr<Shader>> shaderCache;
	friend class EntityComponent;
	friend class Camera;
	friend class GLSetup;
};

