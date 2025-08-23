#include "glPCH.h"
#include "VulkanFunctionLibrary.h"
#include "RenderingPipeline.h"

VkCommandBuffer VulkanFunctionLibrary::BeginOneOffCommandBuffer()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	assert(vkResetCommandPool(vkSettings->device, vkSettings->oneOffCommandPool, 0) == VK_SUCCESS);
	// Create a temporary command buffer
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkSettings->oneOffCommandPool;
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

	vkFreeCommandBuffers(vkSettings->device, vkSettings->oneOffCommandPool, 1, &cmdBuffer);
}

void VulkanFunctionLibrary::TransitionImageLayout(VkImage image, VkFormat fmt, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout, VkBool32 layerCount)
{
	auto cmdBuffer = BeginOneOffCommandBuffer();

	TransitionImageLayout(cmdBuffer, image, fmt, aspect, oldLayout, newLayout, layerCount);

	EndOneOffCommandBuffer(cmdBuffer);
}

void VulkanFunctionLibrary::TransitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkFormat fmt, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout, VkBool32 layerCount)
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
							.layerCount = layerCount
							}
	};
	VkPipelineStageFlags sourceStage;
	switch (oldLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;
		case VK_IMAGE_LAYOUT_GENERAL:
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			break;
		default:
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
			break;
	}
	VkPipelineStageFlags destinationStage;
	switch (newLayout)
	{	
	case VK_IMAGE_LAYOUT_GENERAL:
		barrier.dstAccessMask = 0;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		barrier.dstAccessMask = 0;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
	default:
		barrier.dstAccessMask = 0;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		break;
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
	assert(vkResetCommandPool(vkSettings->device, vkSettings->oneOffCommandPool, 0) == VK_SUCCESS);
	VkCommandBufferAllocateInfo cmdBufferInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.commandPool = vkSettings->oneOffCommandPool,
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
	VkRenderPass renderpass;
	// Create color attachment(s)
	VkAttachmentDescription colorAttachmentInfo = {};
	colorAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
	colorAttachmentInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Create depth attachment
	VkAttachmentDescription depthAttachmentInfo = {};
	depthAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
	depthAttachmentInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	std::vector<VkAttachmentDescription> attachments(1);
	attachments[0] = colorAttachmentInfo;
	//attachments[1] = depthAttachmentInfo;


	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	/*VkAttachmentReference depthRef = {};
	depthRef.attachment = 1;
	depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/


	VkBool32 preserveAttachments[2] = { 0, 1 };
	// Create Present to Shader Read Subpass
	VkSubpassDescription presentToShaderReadSubpass;
	presentToShaderReadSubpass.flags = 0;
	presentToShaderReadSubpass.colorAttachmentCount = 0;
	presentToShaderReadSubpass.pColorAttachments = VK_NULL_HANDLE;
	presentToShaderReadSubpass.pDepthStencilAttachment = VK_NULL_HANDLE;
	presentToShaderReadSubpass.inputAttachmentCount = 0;
	presentToShaderReadSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	presentToShaderReadSubpass.preserveAttachmentCount = 0;
	presentToShaderReadSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
	presentToShaderReadSubpass.pResolveAttachments = VK_NULL_HANDLE;
	presentToShaderReadSubpass.pInputAttachments = VK_NULL_HANDLE;

	// Create Shader Read to Color Subpass
	VkSubpassDescription shaderReadToColorSubpass;
	shaderReadToColorSubpass.flags = 0;
	shaderReadToColorSubpass.colorAttachmentCount = 1;
	shaderReadToColorSubpass.pColorAttachments = &colorRef;
	shaderReadToColorSubpass.pDepthStencilAttachment = VK_NULL_HANDLE;
	shaderReadToColorSubpass.inputAttachmentCount = 0;
	shaderReadToColorSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	shaderReadToColorSubpass.preserveAttachmentCount = 0;
	shaderReadToColorSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	shaderReadToColorSubpass.pResolveAttachments = VK_NULL_HANDLE;
	shaderReadToColorSubpass.pInputAttachments = VK_NULL_HANDLE;

	// Create Color to Present Subpass
	VkSubpassDescription colorToPresentSubpass;
	colorToPresentSubpass.flags = 0;
	colorToPresentSubpass.colorAttachmentCount = 1;
	colorToPresentSubpass.pColorAttachments = &colorRef;
	colorToPresentSubpass.pDepthStencilAttachment = VK_NULL_HANDLE;
	colorToPresentSubpass.inputAttachmentCount = 0;
	colorToPresentSubpass.pInputAttachments = VK_NULL_HANDLE;
	colorToPresentSubpass.preserveAttachmentCount = 0;
	colorToPresentSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	colorToPresentSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	colorToPresentSubpass.pResolveAttachments = VK_NULL_HANDLE;
	


	std::vector<VkSubpassDescription> subpasses = { presentToShaderReadSubpass, shaderReadToColorSubpass, colorToPresentSubpass };

	// Create Present to Shader Read Dependency
	VkSubpassDependency presentToShaderReadDependency;
	presentToShaderReadDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	presentToShaderReadDependency.dstSubpass = 0;
	presentToShaderReadDependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	presentToShaderReadDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	presentToShaderReadDependency.srcAccessMask = VK_ACCESS_NONE;
	presentToShaderReadDependency.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	presentToShaderReadDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create Shader Read to Color Dependency
	VkSubpassDependency shaderReadToColorDependency;
	shaderReadToColorDependency.srcSubpass = 0;
	shaderReadToColorDependency.dstSubpass = 1;
	shaderReadToColorDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	shaderReadToColorDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	shaderReadToColorDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	shaderReadToColorDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	shaderReadToColorDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create Color to Present attachment dependency
	VkSubpassDependency colorToPresentDependency;
	colorToPresentDependency.srcSubpass = 1;
	colorToPresentDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	colorToPresentDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	colorToPresentDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	colorToPresentDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	colorToPresentDependency.dstAccessMask = VK_ACCESS_NONE;
	colorToPresentDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	std::vector<VkSubpassDependency> dependencies = { presentToShaderReadDependency, shaderReadToColorDependency, colorToPresentDependency };

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (VkBool32)attachments.size();
	renderPassInfo.pAttachments = &attachments[0];
	renderPassInfo.pNext = VK_NULL_HANDLE;
	renderPassInfo.subpassCount = (VkBool32)subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = (VkBool32)dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();
	renderPassInfo.flags = NULL;
	VkResult result = vkCreateRenderPass(vkSettings->device, &renderPassInfo, vkSettings->allocationCallback, &renderpass);
	assert(result == VK_SUCCESS);

	return renderpass;
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

std::vector<VkSubpassDependency> VulkanFunctionLibrary::CreatePipelineSubpassDependencies()
{
	// Create Color to Depth/Stencil attachment dependency
	VkSubpassDependency colorToDepthStencilDependency;
	colorToDepthStencilDependency.srcSubpass = 0;
	colorToDepthStencilDependency.dstSubpass = 1;
	colorToDepthStencilDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	colorToDepthStencilDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	colorToDepthStencilDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	colorToDepthStencilDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	colorToDepthStencilDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create Depth/Stencil to Transfer dependency
	VkSubpassDependency depthStencilToTransferDependency;
	depthStencilToTransferDependency.srcSubpass = 1;
	depthStencilToTransferDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	depthStencilToTransferDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	depthStencilToTransferDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	depthStencilToTransferDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	depthStencilToTransferDependency.dstAccessMask = VK_ACCESS_NONE;
	depthStencilToTransferDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create Transfer to Present dependency
	VkSubpassDependency transferToPresentDependency;
	transferToPresentDependency.srcSubpass = 1;
	transferToPresentDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	transferToPresentDependency.srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	transferToPresentDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	transferToPresentDependency.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
	transferToPresentDependency.dstAccessMask = VK_ACCESS_NONE;
	transferToPresentDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	std::vector<VkSubpassDependency> chain {colorToDepthStencilDependency, depthStencilToTransferDependency/*, transferToPresentDependency*/};
	return chain;
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
