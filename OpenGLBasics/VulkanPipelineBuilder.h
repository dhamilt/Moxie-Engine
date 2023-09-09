#pragma once
#include<vulkan/vulkan.h>
#include "VulkanPipelineSetup.h"
#include "VulkanShaders.h"
class VkPipelineBuilder
{
public:
	VkPipelineBuilder() { pipelineSetup = new VkPipelineSetup(); };
	void SetInputAssembly(VkPrimitiveTopology* _topology);
	void LoadShaderModule(VkShaderStageConfigs& shaderConfig, VkShaderModule& shaderModule);
	void GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline);
	
private:
	VkPipelineSetup* pipelineSetup = nullptr;
	VkShaderModule* fragModule, * vertModule;
};

