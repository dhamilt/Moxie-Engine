#pragma once
#include<vulkan/vulkan.h>
#include "VulkanShaders.h"

struct VkVertexAttributeToBindingMapping
{
	VkVertexInputBindingDescription inputBinding;
	std::vector<VkVertexInputAttributeDescription> attributesForBinding;
};

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
	VkPipelineDynamicStateCreateInfo dynamicStateInfo; 
	std::vector<VkPipelineLayoutCreateInfo> layoutInfo;
	
	std::vector<VkVertexAttributeToBindingMapping> vertexBindingMappings;
	std::vector<VkViewport> viewports;
	std::vector<VkRect2D>scissors;
	std::vector<VkShaderStageConfigs> shaderStagingConfigs;
	std::vector<VkShaderModule> shaders;
	std::vector<VkPushConstantRange> pushConstants;
	std::vector<VkPipelineLayout> pipelineLayouts;
	
};

static VkPipelineInputAssemblyStateCreateInfo defaultInputAssemblyState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	.pNext = 0,
	.flags = 0,
	.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	.primitiveRestartEnable = VK_TRUE
};

static const VkDynamicState defaultPipelineDynamicStates[2]{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

static VkPipelineDynamicStateCreateInfo defaultDynamicState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	.pNext = VK_NULL_HANDLE,
	.dynamicStateCount = 2,
	.pDynamicStates = defaultPipelineDynamicStates
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
	.stencilTestEnable = VK_TRUE,
	.minDepthBounds = 0.0f,
	.maxDepthBounds = 1.0f
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
	.setLayoutCount = 2,
	.pSetLayouts = VK_NULL_HANDLE,
	.pushConstantRangeCount = 1,
	.pPushConstantRanges = VK_NULL_HANDLE
};
static VkVertexInputBindingDescription defaultVertexInputBindingInfo = {
	.binding = 0,
	.stride = sizeof(DVertex),
	.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

static VkVertexInputAttributeDescription defaultVertexPosAttr = {
	.location = 0,
	.binding = 0,
	.format = VK_FORMAT_R32G32B32_SFLOAT,
	.offset = offsetof(DVertex, DVertex::pos)
};

static VkVertexInputAttributeDescription defaultVertexTexCoordAttr = {
	.location = 1,
	.binding = 0,
	.format = VK_FORMAT_R32G32_SFLOAT,
	.offset = offsetof(DVertex, DVertex::texCoord)
};

static VkVertexInputAttributeDescription defaultVertexNormalAttr = {
	.location = 2,
	.binding = 0,
	.format = VK_FORMAT_R32G32B32_SFLOAT,
	.offset = offsetof(DVertex, DVertex::normal)
};

static VkVertexInputAttributeDescription defaultVertexAttrDescriptions[3] = { defaultVertexPosAttr, defaultVertexTexCoordAttr, defaultVertexNormalAttr };

static VkPipelineVertexInputStateCreateInfo defaultPipelineVertexInputStateCreateInfo = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	.pNext = VK_NULL_HANDLE,
	.flags = 0,
	.vertexBindingDescriptionCount = 1,
	.pVertexBindingDescriptions = &defaultVertexInputBindingInfo,
	.vertexAttributeDescriptionCount = 3,
	.pVertexAttributeDescriptions = defaultVertexAttrDescriptions
};

static VkPipelineViewportStateCreateInfo defaultViewportStateInfo =
{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	.pNext = VK_NULL_HANDLE,
	.flags = 0
};

class VkPipelineBuilder
{
public:
	VkPipelineBuilder();
	~VkPipelineBuilder() { }
	void SetInputAssembly(VkPipelineBuilderParams& params, VkPrimitiveTopology topology, bool primitiveRestart = true);
	void SetInputAssembly(VkPipelineBuilderParams& params, VkPipelineInputAssemblyStateCreateInfo inputAssemInfo = defaultInputAssemblyState);
	void LoadShaderModule(VkShaderStageConfigs shaderConfig, VkPipelineBuilderParams& params);
	void LoadRenderpass(VkRenderPass renderpass);
	void LoadDepthStencilState(VkPipelineBuilderParams& params, VkPipelineDepthStencilStateCreateInfo depthStencilInfo = defaultDepthStencilState);
	void LoadPipelineLayout(VkPipelineBuilderParams& params, VkPipelineLayout* pPipelineLayout);
	void LoadColorBlendState(VkPipelineBuilderParams& params, VkPipelineColorBlendStateCreateInfo colorBlendInfo = defaultColorBlendState);
	void LoadMultispamplingState(VkPipelineBuilderParams& params, VkPipelineMultisampleStateCreateInfo multisampleInfo = defaultMultisamplingState);
	// TODO: overload with method that only accepts window's extents
	void LoadViewportInfo(VkPipelineBuilderParams& params, VkBool32 viewportCount, VkViewport* viewports, VkBool32 scissorCount, VkRect2D* scissors);
	void LoadViewportInfo(VkPipelineBuilderParams& params, VkExtent2D screenResolution);
	//void GetTriangleShaderPipeline(VkExtent2D windowExtent, VkPipeline* pipeline);
	void CreateMeshShaderPipeline(VkPipeline* pipeline, VkPipelineBuilderParams& params);
	void BuildVertexInputState(VkPipelineBuilderParams& params, VkPipelineVertexInputStateCreateInfo vertexInputInfo);
	VkGraphicsPipelineCreateInfo* GetPipelineInfo();
	
private:
	VkShaderModule* fragModule, * vertModule;
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
};

