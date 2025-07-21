#include "glPCH.h"
#include "VulkanFunctionLibrary.h"
#include "RenderingPipeline.h"

VkCommandBuffer VulkanFunctionLibrary::BeginOneOffCommandBuffer()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	// Create a temporary command buffer
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkSettings->commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(vkSettings->device, &allocInfo, &commandBuffer);


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

void VulkanFunctionLibrary::EndOneOffCommandBuffer(VkCommandBuffer cmdBuffer)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	vkQueueSubmit(vkSettings->queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vkSettings->queue);

	vkFreeCommandBuffers(vkSettings->device, vkSettings->commandPool, 1, &cmdBuffer);
}

void VulkanFunctionLibrary::TransitionImageLayout(VkImage image, VkFormat fmt, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	auto cmdBuffer = BeginOneOffCommandBuffer();

	TransitionImageLayout(cmdBuffer, image, fmt, aspect, oldLayout, newLayout);

	EndOneOffCommandBuffer(cmdBuffer);
}

void VulkanFunctionLibrary::TransitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkFormat fmt, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = {
							.aspectMask = aspect,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
							}
	};

	VkPipelineStageFlags sourceStage, destinationStage;
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_HOST_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		destinationStage = VK_PIPELINE_STAGE_HOST_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	}

	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_GENERAL)
	{
		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
	{
		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
	{
		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("Unsupported Layout Transition!");
	}
	vkCmdPipelineBarrier(
		cmdBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

}

std::vector<VkCommandBuffer> VulkanFunctionLibrary::CreateDefaultCommandBuffers(VkBool32 count)
{
	std::vector<VkCommandBuffer> val(count);

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkCommandBufferAllocateInfo cmdBufferInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.commandPool = vkSettings->commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		.commandBufferCount = 1,
	};
	VkResult result;
	for (VkBool32 i = 0; i < count; ++i)
	{
		result = vkAllocateCommandBuffers(vkSettings->device, &cmdBufferInfo, &val[i]);
		if (result != VK_SUCCESS)
			throw new std::runtime_error("Unable to create command buffer!");
	}

	return val;
}

VkImageView VulkanFunctionLibrary::CreateColor2DImageView(VkImage image, VkFormat fmt)
{
	VkImageView val;

	VkImageViewCreateInfo imgViewInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = fmt,
		.components = {	VK_COMPONENT_SWIZZLE_R,
						VK_COMPONENT_SWIZZLE_G,
						VK_COMPONENT_SWIZZLE_B,
						VK_COMPONENT_SWIZZLE_A},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkResult result = vkCreateImageView(vkSettings->device, &imgViewInfo, vkSettings->allocationCallback, &val);
	if (result != VK_SUCCESS)
		throw new std::runtime_error("Unable to create image view!");

	return val;
}

std::vector<VkFramebuffer> VulkanFunctionLibrary::CreateDefaultFramebuffers(VkBool32 imgViewCount, VkImageView* pImgViews, VkExtent2D resolution, VkRenderPass renderpass)
{
	std::vector<VkFramebuffer> framebuffers(imgViewCount);	
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result;

	for (VkBool32 i = 0; i < imgViewCount; ++i)
	{
		VkFramebufferCreateInfo framebufferInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.renderPass = renderpass,
		.attachmentCount = 1,
		.pAttachments = (pImgViews + i),
		.width = resolution.width,
		.height = resolution.height,
		.layers = 1
		};
		result = vkCreateFramebuffer(vkSettings->device, &framebufferInfo, vkSettings->allocationCallback, &framebuffers[i]);
		if (result != VK_SUCCESS)
			throw new std::runtime_error("Unable to create framebuffer!");
	}

	return framebuffers;
}

VkRenderPass VulkanFunctionLibrary::CreateDefaultRenderpass()
{
	VkAttachmentDescription colorAttachment = {
		.flags = 0,
		.format = VK_FORMAT_B8G8R8A8_UNORM,
		.samples = VK_NUM_OF_SAMPLES,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorRef = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorRef
	};

	VkSubpassDependency subpassDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	VkRenderPassCreateInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &colorAttachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies =  &subpassDependency
	};

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkRenderPass val;

	VkResult result = vkCreateRenderPass(vkSettings->device, &renderPassInfo, vkSettings->allocationCallback, &val);
	if (result != VK_SUCCESS)
		throw new std::runtime_error("Unable to create render pass!");

	return val;
}

VkPipelineCache VulkanFunctionLibrary::CreateDefaultPipelineCache()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkPipelineCacheCreateInfo pipelineCacheInfo;
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheInfo.pNext = VK_NULL_HANDLE;
	pipelineCacheInfo.flags = 0;
	pipelineCacheInfo.initialDataSize = 0;
	pipelineCacheInfo.pInitialData = VK_NULL_HANDLE;

	VkPipelineCache pipelineCache;

	VkResult result = vkCreatePipelineCache(vkSettings->device, &pipelineCacheInfo, vkSettings->allocationCallback, &pipelineCache);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Unable to create pipeline cache!");
	return pipelineCache;
}

VkDescriptorPool VulkanFunctionLibrary::CreateDefaultDescriptorPool()
{
	VkResult result;
	VkDescriptorPool pool;
	// Create a Descriptor Pool
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 });
	poolSizes.push_back({ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 });

	VkDescriptorPoolCreateInfo poolInfo;
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = VK_NULL_HANDLE;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1000 * (uint32_t)poolSizes.size();
	poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	result = vkCreateDescriptorPool(vkSettings->device, &poolInfo, vkSettings->allocationCallback, &pool);

	if (result != VK_SUCCESS)
		throw std::runtime_error("Unable to create the descriptor pool!");

	return pool;

}

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
