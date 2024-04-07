#pragma once
#include <vulkan\vulkan.h>
#include "VulkanPlatformInit.h"
#include <stdexcept>

class VulkanFunctionLibrary
{
public:
	static void CreateVkBuffer(VkDevice device, const VkAllocationCallbacks* allocationCallback, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
};

inline void VulkanFunctionLibrary::CreateVkBuffer(VkDevice device, const VkAllocationCallbacks* allocationCallback, VkPhysicalDevice physicalDevice, VkDeviceSize bufferSize, 
	VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = bufferSize,
		.usage = bufferUsageFlags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VkResult result = vkCreateBuffer(device, &bufferInfo, allocationCallback,
		&buffer);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to reserve memory for buffer!");
	}

	// Retrieve memory requirements for setting up memory buffer
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	// find out if memory type is supported in memory buffer
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice,	&memoryProperties);
	// Flags for allowing the device memory to be accessible and malleable to application code
	VkBool32 memoryFlagIndex = -1;
	for (VkBool32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (memoryRequirements.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
		{
			memoryFlagIndex = i;
			break;
		}
	}

	if (memoryFlagIndex == -1)
	{
		throw new std::runtime_error("Unable to access vulkan device memory!");
	}

	// Allocate device memory for memory buffer
	VkMemoryAllocateInfo memoryAllocationInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memoryRequirements.size,
		.memoryTypeIndex = memoryFlagIndex
	};

	result = vkAllocateMemory(device, &memoryAllocationInfo, allocationCallback, &bufferMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate device memory for buffer!");
	}

	// Bind device memory to memory buffer
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}