#pragma once
#include <vulkan\vulkan.h>
#include "VulkanPlatformInit.h"
#include <stdexcept>

struct UniformBufferParams;

class VulkanFunctionLibrary
{
public:
	static void CreateVkBuffer(VkDevice device, const VkAllocationCallbacks* allocationCallback, VkPhysicalDevice physicalDevice, VkBufferUsageFlags bufferUsageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags, UniformBufferParams& params, VkBool32 bufferCreateFlags = 0);

	static void FillVkBuffer(VkDevice device, UniformBufferParams& params);
};



