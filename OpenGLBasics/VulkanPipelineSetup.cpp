#include "glPCH.h"
#include "VulkanPipelineSetup.h"
#include "VulkanPlatformInit.h"

void VkPipelineSetup::AddShaderStageInfo(VkShaderModule shaderModule, VkShaderStageFlagBits shaderStage)
{
	VkPipelineShaderStageCreateInfo _shaderStageInfo = {};
	_shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	_shaderStageInfo.pNext = VK_NULL_HANDLE;
	// dictate what shader stage is being created on the graphics pipeline
	_shaderStageInfo.stage = shaderStage;
	// add the shader module associated with this shader stage
	_shaderStageInfo.module = shaderModule;
	// the entry point of the shader
	_shaderStageInfo.pName = "main";

	pipelineParams.shaderStageInfo.push_back(_shaderStageInfo);
}

void VkPipelineSetup::AddVertexInputInfo(VkVertexInputInfo viInfo)
{
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo = { };
	_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	_vertexInputInfo.pNext = VK_NULL_HANDLE;

	// Use vertex input attributes and bindings if available
	_vertexInputInfo.vertexAttributeDescriptionCount = (VkBool32)viInfo.attributes.size();
	if (viInfo.attributes.size() > 0)
		_vertexInputInfo.pVertexAttributeDescriptions = &viInfo.attributes[0];
	_vertexInputInfo.vertexBindingDescriptionCount = (VkBool32)viInfo.bindings.size();
	if (viInfo.bindings.size() > 0)
		_vertexInputInfo.pVertexBindingDescriptions = &viInfo.bindings[0];
	

	pipelineParams.vertexInputInfo = _vertexInputInfo;
}

void VkPipelineSetup::AddInputAssemblyInfo(VkPrimitiveTopology topology)
{
	VkPipelineInputAssemblyStateCreateInfo _inputAssemblyInfo = {};
	_inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	_inputAssemblyInfo.pNext = VK_NULL_HANDLE;
	_inputAssemblyInfo.flags = 0;
	_inputAssemblyInfo.topology = topology;
	// Currently set to false
	// TODO: look to see if this needs to be set dynamically
	_inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	pipelineParams.inputAssemblyInfo = _inputAssemblyInfo;
}

void VkPipelineSetup::AddRasterizationInfo(VkPipelineRasterizationStateCreateInfo rasterizationInfo)
{
	pipelineParams.rasterizationInfo = rasterizationInfo;

}

void VkPipelineSetup::AddMultiSamplingInfo(VkPipelineMultisampleStateCreateInfo multisamplingInfo)
{
	pipelineParams.multisamplingInfo = multisamplingInfo;
}

void VkPipelineSetup::AddDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo depthStencilInfo)
{
	pipelineParams.depthStencilInfo = depthStencilInfo;
}

void VkPipelineSetup::AddColorBlendAttachmentInfo(VkPipelineColorBlendAttachmentState colorBlendAttachmentState)
{
	pipelineParams.colorBlendAttachmentStates.push_back(colorBlendAttachmentState);
}

void VkPipelineSetup::AddColorBlendStateInfo(VkPipelineColorBlendStateCreateInfo colorBlendState)
{
	pipelineParams.colorBlendInfo = colorBlendState;
}

void VkPipelineSetup::AddViewportInfo(VkViewport* viewports, VkRect2D* scissors, const VkBool32& viewportCount, const VkBool32& scissorCount)
{
	// Make a viewport state from viewport and scissor parameters
	// by default the application won't support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo _viewportInfo = {};
	_viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	_viewportInfo.flags = 0;
	_viewportInfo.pNext = VK_NULL_HANDLE;
	// TODO: Have these values dynamically assigned
	_viewportInfo.viewportCount = viewportCount;
	_viewportInfo.pViewports = viewports;
	_viewportInfo.scissorCount = scissorCount;
	_viewportInfo.pScissors = scissors;



	pipelineParams.viewportInfo = _viewportInfo;
}

void VkPipelineSetup::AddPipelineLayout()
{
	VkPipelineLayoutCreateInfo _pipelineLayoutInfo = {};
	_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	_pipelineLayoutInfo.pNext = VK_NULL_HANDLE;
	// TODO: Have these values dynamically assigned
	_pipelineLayoutInfo.flags = 0;
	_pipelineLayoutInfo.pSetLayouts = VK_NULL_HANDLE;
	_pipelineLayoutInfo.pushConstantRangeCount = 0;
	_pipelineLayoutInfo.setLayoutCount = 0;
	_pipelineLayoutInfo.pPushConstantRanges = VK_NULL_HANDLE;
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result = vkCreatePipelineLayout(vkSettings->device, &_pipelineLayoutInfo, vkSettings->allocationCallback, &pipelineParams.pipelineLayout);
	assert(result == VK_SUCCESS);
	
}

void VkPipelineSetup::BuildPipelines(VkDevice device, VkRenderPass renderPass, VkPipeline* pipeline)
{
	// Build the pipeline with the pipeline params created
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = VK_NULL_HANDLE;
	// TODO: feed in shader stages to pipeline
	pipelineInfo.stageCount = (VkBool32)pipelineParams.shaderStageInfo.size();
	pipelineInfo.pStages = pipelineParams.shaderStageInfo.data();
	pipelineInfo.pVertexInputState = &pipelineParams.vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &pipelineParams.inputAssemblyInfo;
	pipelineInfo.pColorBlendState = &pipelineParams.colorBlendInfo;
	pipelineInfo.pRasterizationState = &pipelineParams.rasterizationInfo;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &pipelineParams.depthStencilInfo;
	pipelineInfo.pViewportState = &pipelineParams.viewportInfo;
	pipelineInfo.layout = pipelineParams.pipelineLayout;
	pipelineInfo.pMultisampleState = &pipelineParams.multisamplingInfo;
	pipelineInfo.renderPass = renderPass;
	// TODO: Have these values dynamically assigned
	pipelineInfo.subpass = 0;
	
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, vkSettings->allocationCallback, &*pipeline);
	if (result != VK_SUCCESS)
	{
		perror("Failed to create graphics pipeline");
		return;
	}
}
