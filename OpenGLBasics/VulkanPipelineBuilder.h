#pragma once
#include<vulkan/vulkan.h>
#include "VulkanPipelineSetup.h"
#include "VulkanShaders.h"


struct VkPipelineBuilderParams {

	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineMultisampleStateCreateInfo multisamplingInfo;
	VkPipelineLayoutCreateInfo layoutInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	std::vector<VkViewport> viewports;
	std::vector<VkRect2D>scissors;
	std::vector<VkShaderStageConfigs> shaderStagingConfigs;
	VkPipelineLayout pipelineLayout;
};

class VkPipelineBuilder
{
public:
	VkPipelineBuilder() { pipelineSetup = new VkPipelineSetup(); };
	void SetInputAssembly(VkPrimitiveTopology* _topology);
	void LoadShaderModule(VkShaderStageConfigs& shaderConfig, VkShaderModule& shaderModule);
	void GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline);
	void CreateMeshShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline, const VkPipelineBuilderParams& params);
	static void BuildVertexInputDescriptions(VkPipelineBuilderParams& params, VkVertexInputAttributeDescription* vertexInputAttributes, VkBool32 attributeCount);
	static void BuildVertexInputBindings(VkPipelineBuilderParams& params, VkVertexInputBindingDescription* vertexInputBindings, VkBool32 vertexInputBindingCount);
	
private:
	VkPipelineSetup* pipelineSetup = nullptr;
	VkShaderModule* fragModule, * vertModule;
};

