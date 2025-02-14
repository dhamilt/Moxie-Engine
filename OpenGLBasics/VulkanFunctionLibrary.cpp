#include "glPCH.h"
#include "VulkanFunctionLibrary.h"
#include "RenderingPipeline.h"

void VulkanFunctionLibrary::CreateVkBuffer(VkDevice device, const VkAllocationCallbacks* allocationCallback, VkPhysicalDevice physicalDevice,
	VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, UniformBufferParams& params, VkBool32 bufferCreateFlags)
{
	VkDeviceSize totalSize = 0;	
	
	totalSize = params.bufferSize * params.instanceCount;

	VkBufferCreateInfo bufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = bufferCreateFlags,
		.size = totalSize,
		.usage = bufferUsageFlags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VkResult result = vkCreateBuffer(device, &bufferInfo, allocationCallback,
		&params.buffer);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to reserve memory for buffer!");
	}

	// Retrieve memory requirements for setting up memory buffer
	vkGetBufferMemoryRequirements(device, params.buffer, &params.memoryReqs);
	
	// find out if memory type is supported in memory buffer
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	// Flags for allowing the device memory to be accessible and malleable to application code
	VkBool32 memoryFlagIndex = -1;
	for (VkBool32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (params.memoryReqs.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
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
		.allocationSize = params.memoryReqs.size,
		.memoryTypeIndex = memoryFlagIndex
	};

	
	result = vkAllocateMemory(device, &memoryAllocationInfo, allocationCallback, &params.deviceMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate device memory for buffer!");
	}
	
	// Bind device memory to buffer
	vkBindBufferMemory(device, params.buffer, params.deviceMemory, 0);
}

void VulkanFunctionLibrary::FillVkBuffer(VkDevice device, UniformBufferParams& params)
{
	// Map buffer memory
	void* data;
	VkDeviceSize memoryOffset = 0;
	VkResult result = vkMapMemory(device, params.deviceMemory, 0, params.memoryReqs.size, 0, &data);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to map memory to buffer handle!");
	}
	for (VkBool32 i = 0; i < params.instanceCount; ++i)
	{	
		
		memcpy((char*)data + memoryOffset, params.data[i], params.bufferSize);
	
		memoryOffset += params.bufferSize;
	}
	vkUnmapMemory(device, params.deviceMemory);
}
