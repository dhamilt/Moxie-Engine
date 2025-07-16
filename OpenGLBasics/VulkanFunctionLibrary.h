#pragma once
#include <vulkan\vulkan.h>
#include "VulkanPlatformInit.h"
#include <stdexcept>

struct UniformBufferParams;

class VulkanFunctionLibrary
{
public:
	static VkCommandBuffer BeginOneOffCommandBuffer();
	static void EndOneOffCommandBuffer(VkCommandBuffer cmdBuffer);
	static void TransitionImageLayout(VkImage image, VkFormat fmt, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout);
	static std::vector<VkCommandBuffer> CreateDefaultCommandBuffers(VkBool32 count);
	static VkImageView CreateColor2DImageView(VkImage image, VkFormat fmt);
	static std::vector<VkFramebuffer> CreateDefaultFramebuffers(VkBool32 imgViewCount, VkImageView* pImgViews, VkExtent2D resolution, VkRenderPass renderpass);
	static VkRenderPass CreateDefaultRenderpass();
	static void CreateVkBuffer(VkDevice device, const VkAllocationCallbacks* allocationCallback, VkPhysicalDevice physicalDevice, VkBufferUsageFlags bufferUsageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags, UniformBufferParams& params, VkBool32 bufferCreateFlags = 0);

	static void FillVkBuffer(VkDevice device, UniformBufferParams& params);
};



