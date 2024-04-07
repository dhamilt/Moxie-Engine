#pragma once
#include<vulkan/vulkan.h>
#include "VulkanShaders.h"


struct VkPipelineBuilderParams {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineTessellationStateCreateInfo tessellationInfo; // optional
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisamplingInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo; // optional
	VkPipelineLayoutCreateInfo layoutInfo;
	VkRenderPass* renderPassPtr;
	
	std::vector<VkViewport> viewports;
	std::vector<VkRect2D>scissors;
	std::vector<VkShaderStageConfigs> shaderStagingConfigs;
	std::vector<VkShaderModule> shaders;
	std::vector<VkPushConstantRange> pushConstants;
	VkPipelineLayout pipelineLayout;
};

static VkPipelineInputAssemblyStateCreateInfo defaultInputAssemblyState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	.pNext = 0,
	.flags = 0,
	.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	.primitiveRestartEnable = VK_TRUE
};

// default multisampling state 
static VkPipelineMultisampleStateCreateInfo defaultMultisamplingState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	.pNext = 0,
	.flags = 0,
	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	.sampleShadingEnable = VK_FALSE,
	.alphaToCoverageEnable = VK_FALSE,
	.alphaToOneEnable = VK_FALSE
};

// default color blending state is set off
static VkPipelineColorBlendAttachmentState defaultColorBlendAttachmentState = {
	.blendEnable = VK_FALSE,
	.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT

};

//the blending is just "no blend", but we do write to the color attachment
static VkPipelineColorBlendStateCreateInfo defaultColorBlendState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	.logicOpEnable = VK_FALSE,
	.logicOp = VK_LOGIC_OP_COPY,
	.attachmentCount = 1,
	.pAttachments = &defaultColorBlendAttachmentState
};

// default depth/stencil state is depth testing and writing enabled
// but depth bounds testing and stencil testing are disabled
static VkPipelineDepthStencilStateCreateInfo defaultDepthStencilState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
	.depthTestEnable = VK_TRUE,
	.depthWriteEnable = VK_TRUE,
	.depthCompareOp = VK_COMPARE_OP_LESS,
	.depthBoundsTestEnable = VK_FALSE,
	.stencilTestEnable = VK_FALSE
};

// default rasterization state for graphics pipeline is neither having depth clamping
// nor discarding while culling the triangles behind the mesh with respect to the camera and
// setting the vertex order for reading the vertices for each face in clockwise order
static VkPipelineRasterizationStateCreateInfo defaultRasterizationState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	.depthClampEnable = VK_FALSE,
	.rasterizerDiscardEnable = VK_FALSE,
	.polygonMode = VK_POLYGON_MODE_FILL,
	.cullMode = VK_CULL_MODE_BACK_BIT,
	.frontFace = VK_FRONT_FACE_CLOCKWISE,
	.lineWidth = 1.0f
};

static VkPipelineLayoutCreateInfo defaultPipelineLayoutInfo = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	.pNext = VK_NULL_HANDLE,
	.flags = 0,
	.setLayoutCount = 0,
	.pSetLayouts = VK_NULL_HANDLE,
	.pushConstantRangeCount = 0,
	.pPushConstantRanges = VK_NULL_HANDLE
};

class VkPipelineBuilder
{
public:
	VkPipelineBuilder();
	~VkPipelineBuilder() { }
	void SetInputAssembly(VkPipelineBuilderParams& params,VkPrimitiveTopology topology, bool primitiveRestart = true);
	void LoadShaderModule(VkShaderStageConfigs shaderConfig, VkPipelineBuilderParams& params);
	void LoadDepthStencilState(VkPipelineBuilderParams& params, VkPipelineDepthStencilStateCreateInfo depthStencilInfo);
	void LoadPipelineLayout(VkPipelineBuilderParams& params, VkPipelineLayout pipelineLayout);
	void LoadColorBlendState(VkPipelineBuilderParams& params, VkPipelineColorBlendStateCreateInfo colorBlendInfo);
	void LoadMultispamplingState(VkPipelineBuilderParams& params, VkPipelineMultisampleStateCreateInfo multisampleInfo);
	// TODO: overload with method that only accepts window's extents
	void LoadViewportInfo(VkPipelineBuilderParams& params, VkBool32 viewportCount, VkViewport* viewports, VkBool32 scissorCount, VkRect2D* scissors);
	void LoadViewportInfo(VkPipelineBuilderParams& params, VkExtent2D screenResolution);
	//void GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline);
	void CreateMeshShaderPipeline(VkPipeline* pipeline, VkPipelineBuilderParams& params);
	void BuildVertexInputState(VkPipelineBuilderParams& params, VkPipelineVertexInputStateCreateInfo vertexInputInfo);
	
private:
	VkShaderModule* fragModule, * vertModule;
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
};

