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


struct CubemapParams_
{
	VkPipeline pipeline;
	VkShaderModule vertexShader, fragmentShader;
	VkImage img, depthImg;
	VkImageView imgView;
	VkImageView depthView;
	VkBuffer stagingBuffer;
	VkDeviceSize stagingBufferSize, layerSize;
	void* stagingData;
	std::vector<TextureData*> images;
	VkDeviceMemory imageMemory, depthMemory, stagingMemory;
	VkDescriptorSetLayout vertexLayout, fragmentLayout;
	VkPipelineLayout pipelineLayout;
};


class VkCubemap
{
public:
	~VkCubemap();
	void Setup();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateRenderpass();
	void CreateFramebuffers();
	void CreateViewportState();
	void GenerateCubeMap(std::vector<TextureData*> textures);
	void CopyCubeMapToBuffer(VkBool32 frameIndex);
	void CreateCubemapShaders();
	void AddVertexInputBindings();
	void CreateCubemapPipelines();
	void DrawCubemap();

private:
	int width, height;
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