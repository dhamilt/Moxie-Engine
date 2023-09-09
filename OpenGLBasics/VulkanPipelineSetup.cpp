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

void VkPipelineSetup::AddRasterizationInfo(VkRasterizationInfo rasterizationInfo)
{
	VkPipelineRasterizationStateCreateInfo _rasterizationStateInfo = {};
	_rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	_rasterizationStateInfo.pNext = VK_NULL_HANDLE;
	_rasterizationStateInfo.flags = 0;

	// Determines the draw mode 
	_rasterizationStateInfo.polygonMode = rasterizationInfo.polygonMode;
	// Width of the lines drawn between vertex points
	_rasterizationStateInfo.lineWidth = rasterizationInfo.lineWidth;
	// Determines which triangles are discarded in rendering process
	_rasterizationStateInfo.cullMode = rasterizationInfo.cullMode;
	// Determines if polygon is front or back facing
	_rasterizationStateInfo.frontFace = rasterizationInfo.frontFace;

	// Determines if depth bias is used
	_rasterizationStateInfo.depthBiasEnable = rasterizationInfo.rasterizationFlags 
		& VkRasterizationFlagBits::VK_PIPELINE_RASTERIZATION_DEPTH_BIAS_BIT;
	// How much to scale the depth by
	_rasterizationStateInfo.depthBiasConstantFactor = rasterizationInfo.depthBiasConstantFactor;
	// How much to scale the slope depth by
	_rasterizationStateInfo.depthBiasSlopeFactor = rasterizationInfo.depthBiasSlopeFactor;
	// Determines if depth clamp is used
	_rasterizationStateInfo.depthClampEnable = rasterizationInfo.rasterizationFlags
		& VkRasterizationFlagBits::VK_PIPELINE_RASTERIZATION_DEPTH_CLAMP_BIT;
	// The range for the depth bias
	_rasterizationStateInfo.depthBiasClamp = rasterizationInfo.depthBiasClamp;
	// If ENABLED, discards all primitives before starting the rasterization stage
	_rasterizationStateInfo.rasterizerDiscardEnable = rasterizationInfo.rasterizationFlags
		& VkRasterizationFlagBits::VK_PIPELINE_RASTERIZATION_DISCARD_BIT;

	pipelineParams.rasterizationInfo = _rasterizationStateInfo;

}

void VkPipelineSetup::AddMultiSamplingInfo(VkMultisamplingInfo multisamplingInfo)
{
	VkPipelineMultisampleStateCreateInfo _multisamplingInfo = {};
	_multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	_multisamplingInfo.pNext = VK_NULL_HANDLE;
	_multisamplingInfo.flags = 0;
	// Determines if sample shading is used
	_multisamplingInfo.sampleShadingEnable = multisamplingInfo.multisamplingFlags 
		& VkMultisamplingFlagBits::VK_PIPELINE_MULTISAMPLING_SAMPLE_SHADING_BIT;
	// Determines how many samples are used in rasterization
	_multisamplingInfo.rasterizationSamples = (VkSampleCountFlagBits)multisamplingInfo.rasterizationSamples;
	// Determines the minimum fraction to be used in sample shading
	_multisamplingInfo.minSampleShading = multisamplingInfo.minSampleShading;
	// Assigns the set of masks to use when sampling
	_multisamplingInfo.pSampleMask = multisamplingInfo.sampleMasks.size() > 0 ? & multisamplingInfo.sampleMasks[0] : VK_NULL_HANDLE;
	// Determines if the alpha component for the first color value of the fragment is to be transferred between samples
	_multisamplingInfo.alphaToCoverageEnable = multisamplingInfo.multisamplingFlags
		& VkMultisamplingFlagBits::VK_PIPELINE_MULTISAMPLING_ALPHA_TO_COVERAGE_BIT;
	// Determines of the alpha component for the first color value of the fragment is to be replaced by subsequent samples' alpha values
	_multisamplingInfo.alphaToOneEnable = multisamplingInfo.multisamplingFlags
		& VkMultisamplingFlagBits::VK_PIPELINE_MULTISAMPLING_ALPHA_TO_ONE_BIT;

	pipelineParams.multisamplingInfo = _multisamplingInfo;
}

void VkPipelineSetup::AddDepthStencilInfo(VkDepthStencilInfo depthStencilInfo)
{
	VkPipelineDepthStencilStateCreateInfo _depthStencilInfo = { };
	_depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	_depthStencilInfo.flags = 0;
	_depthStencilInfo.depthTestEnable = depthStencilInfo.depthStencilFlags
		& VK_PIPELINE_DEPTH_BIT ? VK_TRUE : VK_FALSE;
	_depthStencilInfo.depthWriteEnable = depthStencilInfo.depthStencilFlags
		& VK_PIPELINE_DEPTH_WRITE_BIT ? VK_TRUE : VK_FALSE;
	_depthStencilInfo.depthBoundsTestEnable = depthStencilInfo.depthStencilFlags
		& VK_PIPELINE_DEPTH_BOUNDS_BIT ? VK_TRUE : VK_FALSE;
	_depthStencilInfo.stencilTestEnable = depthStencilInfo.depthStencilFlags
		& VK_PIPELINE_STENCIL_BIT ? VK_TRUE : VK_FALSE;
	_depthStencilInfo.minDepthBounds = depthStencilInfo.minDepthBounds;
	_depthStencilInfo.maxDepthBounds = depthStencilInfo.maxDepthBounds;
	_depthStencilInfo.front = depthStencilInfo.frontStencilState;
	_depthStencilInfo.back = depthStencilInfo.backStencilState;

	pipelineParams.depthStencilInfo = _depthStencilInfo;
}

void VkPipelineSetup::AddColorBlendAttachmentInfo()
{
	VkPipelineColorBlendAttachmentState _colorBlendAttachmentInfo = {};
	// TODO: Have these values dynamically assigned
	_colorBlendAttachmentInfo.blendEnable = VK_FALSE;
	_colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	pipelineParams.colorBlendAttachmentStates.push_back(_colorBlendAttachmentInfo);
	//pipelineParams.colorBlendAttachmentStates.push_back(_colorBlendAttachmentInfo);

}

void VkPipelineSetup::AddColorBlendStateInfo()
{
	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo _colorBlendInfo = {};
	// TODO: Have these values dynamically assigned
	_colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	_colorBlendInfo.pNext = VK_NULL_HANDLE;
	_colorBlendInfo.attachmentCount = (VkBool32)pipelineParams.colorBlendAttachmentStates.size();
	_colorBlendInfo.pAttachments = &pipelineParams.colorBlendAttachmentStates[0];
	_colorBlendInfo.logicOpEnable = VK_FALSE;
	_colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;

	pipelineParams.colorBlendInfo = _colorBlendInfo;
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
