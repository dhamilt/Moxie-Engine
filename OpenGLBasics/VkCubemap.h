#pragma once
#include "glPCH.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanShaders.h"

struct TextureData;


struct CubemapVertConstants
{
	DVector3 position;
	DVector3 texCoord;
};

struct CubemapFragConstants
{
	DVector4 fragmentColor;
	DVector3 texCoord;
};

struct CubemapObjectProperties
{
	DMat4x4 projection;
	DMat4x4 view;
};


// vertices
static std::vector<DVector3> cubeMapVertexBuffer = 
{
	DVector3(1.0f, 1.0f, 1.0f),		// 0
	DVector3(1.0f, 1.0f, -1.0f),	// 1
	DVector3(1.0f, -1.0f, 1.0f),	// 2
	DVector3(1.0f, -1.0f, -1.0f),	// 3
	DVector3(-1.0f, 1.0f, 1.0f),	// 4
	DVector3(-1.0f, 1.0f, -1.0f),	// 5
	DVector3(-1.0f, -1.0f, 1.0f),	// 6
	DVector3(-1.0f, -1.0f, -1.0f)	// 7
};
// indices
static std::vector<uint16_t> cubeMapIndexBuffer =
{
		5, 7, 3,
		3, 1, 5,
		6, 7, 5,
		5, 4, 6,
		3, 2, 0,
		0, 1, 3,
		6, 4, 0,
		0, 2, 6,
		5, 1, 0,
		0, 4, 5,
		7, 6, 3,
		3, 6, 2
};

struct CubemapParams_
{
	VkPipeline pipeline;
	VkShaderModule vertexShader, fragmentShader;
	VkImage img, depthImg;
	VkImageView imgView;
	VkImageView depthView;
	VkSampler sampler;
	VkBuffer stagingBuffer;
	VkDeviceSize stagingBufferSize, layerSize;
	std::vector<DVector3> normals;
	void* stagingData;
	std::vector<TextureData*> images;
	VkDeviceMemory imageMemory, depthMemory, stagingMemory, vertexMemory, fragmentMemory, vertexBufMemory, indexBufMemory;
	VkBuffer objectPropertyBuffer, vertexBuffer, indexBuffer;
	VkDescriptorSetLayout vertexLayout, fragmentLayout;
	VkDescriptorSet vertexDescriptor, fragmentDescriptor;
	VkPipelineLayout pipelineLayout;
};


class VkCubemap
{
public:
	~VkCubemap();
	void Setup();
	void Setup(VkRenderPass* renderpass_);
	void CalculateNormals();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateRenderpass();
	void LoadRenderpass(VkRenderPass* renderpass_);
	void CreateFramebuffers();
	void CreateSampler();
	void CreateViewportState();
	void GenerateCubeMap(std::vector<TextureData*> textures);
	void CopyCubeMapToBuffer(VkBool32 frameIndex);
	void CreateCubemapShaders();
	void AddVertexInputBindings();
	void CreateIndexBuffer();
	void CreateCubemapPipelines();
	void UpdateViewProjectionMatricies(DMat4x4 view, DMat4x4 projection);
	void DrawCubemap(VkBool32 frameIndex);
	void DrawCubemap(VkCommandBuffer& cmdBuffer);

private:
	int width, height;
	DMat4x4 viewMatrix, projectionMatrix;
	CubemapObjectProperties objectProperties;
	std::vector<VkCommandBuffer>cmdBuffers;
	std::vector<VkFramebuffer> framebuffers;	
	VkCommandPool cmdPool;
	VkRenderPass renderpass;
	std::vector<VkBufferImageCopy> cubemapImgCopyInfo;
	std::vector<TextureData*> cubemapData;
	CubemapParams_ cubemapParams;
	VkShaderStageConfigs shaderConfigs;
	VkPipelineBuilder pipelineBuilder;
	VkPipelineBuilderParams pipelineBuilderParams;

};