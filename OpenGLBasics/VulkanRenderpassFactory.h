#pragma once
#include <vulkan/vulkan.h>

struct VkRenderPassParams
{
	VkDevice* device;
	VkAllocationCallbacks* allocationCallback;
	VkRenderPassCreateInfo* renderPassInitParams;
};

class FVkRenderpassFactory
{
public:
	static bool CreateRenderpass(VkRenderPassParams* renderpassParams, VkRenderPass* renderpass);
;
	
};

bool FVkRenderpassFactory::CreateRenderpass(VkRenderPassParams* renderpassParams, VkRenderPass* renderpass)
{
	VkResult result;
	result = vkCreateRenderPass(*renderpassParams->device, renderpassParams->renderPassInitParams, renderpassParams->allocationCallback, renderpass);

	return result == VK_SUCCESS;

}