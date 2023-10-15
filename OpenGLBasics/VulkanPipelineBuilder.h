#pragma once
#include<vulkan/vulkan.h>
#include "VulkanPipelineSetup.h"
#include "VulkanShaders.h"

struct VkPipelineBuilderParams {
	VkVertexInputInfo vertexInputInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	std::vector<VkShaderStageConfigs> shaderStagingConfigs;
};

class VkPipelineBuilder
{
public:
	VkPipelineBuilder() { pipelineSetup = new VkPipelineSetup(); };
	void SetInputAssembly(VkPrimitiveTopology* _topology);
	void LoadShaderModule(VkShaderStageConfigs& shaderConfig, VkShaderModule& shaderModule);
	void GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline);
	void CreateMeshShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline, const VkPipelineBuilderParams& params);
	
private:
	VkPipelineSetup* pipelineSetup = nullptr;
	VkShaderModule* fragModule, * vertModule;
};

