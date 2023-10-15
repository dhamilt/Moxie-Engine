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

static VkPipelineColorBlendAttachmentState defaultColorBlendAttachmentState = { .blendEnable= VK_FALSE };

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
	void AddRasterizationInfo(VkRasterizationInfo rasterizationInfo = VkRasterizationInfo());
	// Add configuration for sampling the rasterization stage of the shader pipeline
	void AddMultiSamplingInfo(VkMultisamplingInfo multisamplingInfo = VkMultisamplingInfo());
	// Add configuration for depth/stencil attachments in shader pipeline
	void AddDepthStencilInfo(VkDepthStencilInfo depthStencilInfo = VkDepthStencilInfo());
	// TODO: allow for parameterized inputs for the color blending attachment
	void AddColorBlendAttachmentInfo(VkPipelineColorBlendAttachmentState colorBlendAttachmentState = VkPipelineColorBlendAttachmentState());
	// TODO: allow for parameterized inputs for the color blending state
	void AddColorBlendStateInfo();
	// TODO: allow for parameterized viewport info
	void AddViewportInfo(VkViewport* viewports, VkRect2D* scissors, const VkBool32& viewportCount, const VkBool32& scissorCount);
	// TODO: allow for parameterized input for the pipeline layout
	void AddPipelineLayout();


private:
	VkPipelineParams pipelineParams;
};

