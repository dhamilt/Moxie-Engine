#pragma once
#include <vulkan/vulkan.h>
#include <vector>

typedef enum VkMultisamplingFlagBits {
	VK_PIPELINE_MULTISAMPLING_NONE = 0x0000000,
	VK_PIPELINE_MULTISAMPLING_SAMPLE_SHADING_BIT = 0x0000001,
	VK_PIPELINE_MULTISAMPLING_ALPHA_TO_COVERAGE_BIT = 0x0000002,
	VK_PIPELINE_MULTISAMPLING_ALPHA_TO_ONE_BIT = 0x0000004
}VkMultisamplingFlagBits;

typedef enum VkRasterizationFlagBits {
	VK_PIPELINE_RASTERIZATION_DEPTH_BIAS_BIT = 0x0000001,
	VK_PIPELINE_RASTERIZATION_DEPTH_CLAMP_BIT = 0x0000002,
	// Discards all primitives before starting the rasterization process
	VK_PIPELINE_RASTERIZATION_DISCARD_BIT = 0x0000004
}VkRasterizationFlagBits;

typedef enum VkDepthStencilFlagBits {
	VK_PIPELINE_DEPTH_BIT = 0x0000001,
	VK_PIPELINE_DEPTH_WRITE_BIT = 0x0000002,
	VK_PIPELINE_DEPTH_BOUNDS_BIT = 0x0000004,
	VK_PIPELINE_STENCIL_BIT = 0x0000008,
}VkDepthStencilFlagBits;

struct VkPipelineParams
{
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfo;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
	VkPipelineMultisampleStateCreateInfo multisamplingInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	VkPipelineLayout pipelineLayout;
};

struct VkVertexInputInfo
{
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
};

typedef VkFlags VkRasterizationFlags;
typedef VkFlags VkMultisamplingFlags;
typedef VkFlags VkDepthStencilFlags;

struct VkRasterizationInfo
{
	VkRasterizationFlags rasterizationFlags;
	VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
	VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
	VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
	float lineWidth = 1.0f;
	float depthBiasClamp = 0.0f;
	float depthBiasConstantFactor = 0.0f;
	float depthBiasSlopeFactor = 0.0f;

};

struct VkMultisamplingInfo
{
	VkMultisamplingFlags multisamplingFlags = VK_PIPELINE_MULTISAMPLING_NONE;
	VkSampleCountFlags rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	float minSampleShading = 1.0f;
	std::vector<VkSampleMask> sampleMasks;
};

struct VkDepthStencilInfo
{
	VkDepthStencilFlags depthStencilFlags = VK_PIPELINE_DEPTH_BIT | VK_PIPELINE_DEPTH_WRITE_BIT | VK_PIPELINE_STENCIL_BIT;
	VkCompareOp depthComparisonOperations = VK_COMPARE_OP_LESS_OR_EQUAL;
	VkStencilOpState frontStencilState;
	VkStencilOpState backStencilState;
	float minDepthBounds = 0.0f;
	float maxDepthBounds = 1.0f;

};

// default color blending state is set off
static VkPipelineColorBlendAttachmentState defaultColorBlendAttachmentState = { 
	.blendEnable= VK_FALSE,
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

// default multisampling state is single sampling
static VkPipelineMultisampleStateCreateInfo defaultMultisamplingState = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	.sampleShadingEnable = VK_FALSE,
	.minSampleShading = 1.0f
};

// default depth/stencil state is depth testing and writing enabled
// but depth bounds testing and stencil testing are disabled
static VkPipelineDepthStencilStateCreateInfo defaultDepthStencilInfo = {
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
static VkPipelineRasterizationStateCreateInfo defaultRasterizationInfo = {
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



class VkPipelineSetup
{
public:
	void BuildPipelines(VkDevice device, VkRenderPass renderPass, VkPipeline* pipelines);
	VkPipelineParams GetPipelineInfo() { return pipelineParams; }

	void AddShaderStageInfo(VkShaderModule shaderModule, VkShaderStageFlagBits shaderStage);
	// TODO: allow for parameterized vertex inputs
	void AddVertexInputInfo(VkVertexInputInfo viInfo = VkVertexInputInfo());
	// TODO: allow for parameterized input assembly
	void AddInputAssemblyInfo(VkPrimitiveTopology topology);
	// Adds configuration for rasterization stage of the shader pipeline
	void AddRasterizationInfo(VkPipelineRasterizationStateCreateInfo rasterizationInfo = defaultRasterizationInfo);
	// Add configuration for sampling the rasterization stage of the shader pipeline
	void AddMultiSamplingInfo(VkPipelineMultisampleStateCreateInfo multisamplingInfo = defaultMultisamplingState);
	// Add configuration for depth/stencil attachments in shader pipeline
	void AddDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo depthStencilInfo = defaultDepthStencilInfo);
	// Adds configuration for color blending attachments
	void AddColorBlendAttachmentInfo(VkPipelineColorBlendAttachmentState colorBlendAttachmentState = defaultColorBlendAttachmentState);
	// Adds configuration for color blend state
	void AddColorBlendStateInfo(VkPipelineColorBlendStateCreateInfo colorBlendState = defaultColorBlendState);
	// TODO: allow for parameterized viewport info
	void AddViewportInfo(VkViewport* viewports, VkRect2D* scissors, const VkBool32& viewportCount, const VkBool32& scissorCount);
	// Add the shader layouts for the descriptors and bindings for the pipeline
	void AddPipelineLayout(VkPipelineLayout pipelineLayout);
	void AddPipelineLayout(VkPipelineLayoutCreateInfo pipelineInfo = defaultPipelineLayoutInfo);


private:
	VkPipelineParams pipelineParams;
};

