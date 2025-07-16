#include "glPCH.h"
#include "VulkanRenderpassBuilder.h"


VkRenderPass VulkanRenderpassBuilder::GetRenderpassHandle()
{
    return renderPass;
}

VkFramebuffer VulkanRenderpassBuilder::GetFramebufferHandle(VkBool32 frameIndex)
{
    if(IsValid())
        if(framebuffers.size() > frameIndex)
            return framebuffers[frameIndex];

    return NULL;
}

VkCommandBuffer VulkanRenderpassBuilder::GetCommandBufferHandle(VkBool32 frameIndex)
{
	if (IsValid())
	{
		if (commandbuffers.size() > frameIndex)
		{
			if (frameIndex != currentFrameIndex)
			{
				VkCommandBufferBeginInfo cmdBeginInfo = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.pNext = VK_NULL_HANDLE,
				.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
				.pInheritanceInfo = VK_NULL_HANDLE
				};
				assert(vkBeginCommandBuffer(commandbuffers[frameIndex], &cmdBeginInfo) == VK_SUCCESS);
				currentFrameIndex = frameIndex;
			}
			return commandbuffers[frameIndex];
		}
	}

	return NULL;
}

void VulkanRenderpassBuilder::CreateSurface(SDL_Window* window)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	if (!SDL_Vulkan_CreateSurface(window, vkSettings->instance, &surface))
	{
		printf("Error! %s\n", SDL_GetError());
		throw std::runtime_error("Could not create Vulkan surface.");
	}
}

void VulkanRenderpassBuilder::RetrieveSwapchainImages()
{
	//VkSurfaceCapabilitiesKHR surfaceCapabilities;
	auto currentVKSettings = PVulkanPlatformInit::Get()->GetInfo();

	//vkGetPhysicalDeviceSurfaceCapabilitiesKHR(currentVKSettings->physicalDevices[currentVKSettings->discreteGPUIndex], currentVKSettings->surface, &surfaceCapabilities);
	//// Make one more than the swapchain requires
	//// as long as it is permitted
	//VkSwapchainCreateInfoKHR swapchainInfo;
	//swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	//swapchainInfo.minImageCount = currentVKSettings->minImageCount;
	//// Clamp swapchain image extent to surface extent threshold
	//swapchainInfo.imageExtent.height = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageExtent.height,
	//	surfaceCapabilities.maxImageExtent.height, surfaceCapabilities.currentExtent.height);
	//swapchainInfo.imageExtent.width = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageExtent.width,
	//	surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.currentExtent.width);

	//// Set swapchain's starting transform to use default transform if surface supports it
	//// otherwise use the current surface transform
	//swapchainInfo.preTransform = surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ?
	//	VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

	//// Find a supported composite alpha mode - one of these is guaranteed to be set
	//swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
	//	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	//	VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
	//	VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	//	VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	//};

	//for (auto it = compositeAlphaFlags.begin(); it != compositeAlphaFlags.end(); it++) {
	//	if (surfaceCapabilities.supportedCompositeAlpha & *it) {
	//		swapchainInfo.compositeAlpha = *it;
	//		break;
	//	}
	//}
	//	

	//swapchainInfo.imageArrayLayers = 1;
	//swapchainInfo.presentMode = PVulkanPlatformInit::Get()->SetPresentMode(currentVKSettings->presentModes);
	//swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//swapchainInfo.queueFamilyIndexCount = 1;
	//swapchainInfo.pQueueFamilyIndices = &currentVKSettings->queueFamilies[0];
	//swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
	//swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	//swapchainInfo.clipped = true;
	//swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	//swapchainInfo.surface = surface;
	//swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	auto device = currentVKSettings->device;
	//// Create the swapchain
	//VkResult result = vkCreateSwapchainKHR(device, &swapchainInfo, currentVKSettings->allocationCallback, &swapchain);

	//assert(result == VK_SUCCESS, "Error! Unable to create swapchain!");	

	auto imgCount = currentVKSettings->swapchainImageCount;
	// Retrieve the swapchain images
	swapchainImgs = currentVKSettings->swapchainImages;

	VkResult result;
	// Create image view(s) for the swapchain
	for (VkBool32 i = 0; i < imgCount; ++i)
	{
		PVkImageBuffer imgParam = {};
		imgParam.image = swapchainImgs[i];
		VkImageViewCreateInfo imgViewInfo = {};
		imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewInfo.pNext = nullptr;
		imgViewInfo.flags = 0;
		imgViewInfo.image = imgParam.image;
		imgViewInfo.format = currentVKSettings->surfaceFormat;
		imgViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imgViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imgViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imgViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imgViewInfo.subresourceRange.baseMipLevel = 0;
		imgViewInfo.subresourceRange.levelCount = 1;
		imgViewInfo.subresourceRange.baseArrayLayer = 0;
		imgViewInfo.subresourceRange.layerCount = 1;
		imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		result = vkCreateImageView(device, &imgViewInfo, currentVKSettings->allocationCallback, &imgParam.imageView);
		assert(result == VK_SUCCESS);
		swapchainImgParams.push_back(imgParam);
	};
	// Create Depth Buffer
	VkImageCreateInfo imgCreateInfo = {};
	imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgCreateInfo.pNext = VK_NULL_HANDLE;
	imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imgCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;	
	imgCreateInfo.extent.width = resolution.width;
	imgCreateInfo.extent.height = resolution.height;
	imgCreateInfo.extent.depth = 1;
	imgCreateInfo.mipLevels = 1;
	imgCreateInfo.arrayLayers = 1;
	imgCreateInfo.samples = VK_NUM_OF_SAMPLES;
	imgCreateInfo.queueFamilyIndexCount = 1;
	imgCreateInfo.pQueueFamilyIndices = &currentVKSettings->queueFamilies[0];
	imgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imgCreateInfo.flags = 0;
	imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	

	result = vkCreateImage(device, &imgCreateInfo, currentVKSettings->allocationCallback, &depthBufferParams.image);

	assert(result == VK_SUCCESS, "Error! Unable to create depth buffer image object!");
		
	// Allocate memory for the depth buffer
	VkMemoryRequirements depthBufMemReqs;
	vkGetImageMemoryRequirements(device, depthBufferParams.image, &depthBufMemReqs);
	VkMemoryAllocateInfo depthBufMemoryInfo = {};
	depthBufMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	depthBufMemoryInfo.allocationSize = depthBufMemReqs.size;

	VkDeviceMemory depthBufMemory;
	result = vkAllocateMemory(device, &depthBufMemoryInfo, currentVKSettings->allocationCallback, &depthBufMemory);

	assert(result == VK_SUCCESS, "Error! Unable to allocate memory for the Depth Buffer!");
	
	// Bind memory buffer to depth buffer
	vkBindImageMemory(device, depthBufferParams.image, depthBufMemory, 0);

	VkImageViewCreateInfo depthViewInfo = {};
	depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthViewInfo.pNext = VK_NULL_HANDLE;
	depthViewInfo.flags = 0;
	depthViewInfo.image = depthBufferParams.image;
	depthViewInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depthViewInfo.subresourceRange.baseMipLevel = 0;
	depthViewInfo.subresourceRange.levelCount = 1;
	depthViewInfo.subresourceRange.baseArrayLayer = 0;
	depthViewInfo.subresourceRange.layerCount = 1;
	depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	result = vkCreateImageView(device, &depthViewInfo, currentVKSettings->allocationCallback, &depthBufferParams.imageView);

	assert(result == VK_SUCCESS, "Error! Unable to create depth buffer image view!");

}

void VulkanRenderpassBuilder::CreateFramebuffers()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.width = resolution.width;
	framebufferInfo.height = resolution.height;	

	// Set the attachment count to be same as the number of image buffers allocated for in the swapchain
	framebufferInfo.attachmentCount = 2;
	for (uint32_t i = 0; i < vkSettings->swapchainImageCount; i++)
	{
		std::vector<VkImageView> attachments;
		attachments.push_back(swapchainImgParams[i].imageView);
		attachments.push_back(depthBufferParams.imageView);
		framebufferInfo.pAttachments = &attachments[0];
		framebufferInfo.pNext = VK_NULL_HANDLE;
		framebufferInfo.renderPass = vkSettings->renderPass;
		framebufferInfo.layers = 1;
		framebufferInfo.flags = 0;
		VkFramebuffer vkFramebuffer;
		auto result = vkCreateFramebuffer(vkSettings->device, &framebufferInfo,
			vkSettings->allocationCallback, &vkFramebuffer);
		assert(result == VK_SUCCESS);
		framebuffers.push_back(vkFramebuffer);
	}
}

void VulkanRenderpassBuilder::CreateCommandBuffers()
{
    if (framesInFlight > 0)
    {
		auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
		commandbuffers.resize(framesInFlight);
		VkCommandBufferAllocateInfo cmdBufferInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.commandPool = vkSettings->commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		VkResult result;
		for (VkBool32 i = 0; i < framesInFlight; ++i)
		{
			result = vkAllocateCommandBuffers(vkSettings->device, &cmdBufferInfo, &commandbuffers[i]);
			if (result != VK_SUCCESS)
				throw new std::runtime_error("Unable to create command buffer!");
		}
    }
}

void VulkanRenderpassBuilder::CreateRenderpass()
{
	// Create color attachment(s)
	VkAttachmentDescription colorAttachmentInfo = {};
	colorAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
	colorAttachmentInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	// Create depth attachment
	VkAttachmentDescription depthAttachmentInfo = {};
	depthAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
	depthAttachmentInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	std::vector<VkAttachmentDescription> attachments(2);
	attachments[0] = colorAttachmentInfo;
	attachments[1] = depthAttachmentInfo;


	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkAttachmentReference depthRef = {};
	depthRef.attachment = 1;
	depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Create Subpass info
	VkSubpassDescription subpassDescription = {};
	subpassDescription.flags = 0;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorRef;
	subpassDescription.pDepthStencilAttachment = &depthRef;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = VK_NULL_HANDLE;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
	subpassDescription.pResolveAttachments = VK_NULL_HANDLE;
	subpassDescription.pInputAttachments = VK_NULL_HANDLE;


	// Create subpass dependency for auto transitioning between image layouts
	VkSubpassDependency dependency = {};
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	auto currentVKSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pNext = VK_NULL_HANDLE;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	renderPassInfo.pAttachments = &attachments[0];
	renderPassInfo.flags = NULL;
	VkResult result = vkCreateRenderPass(currentVKSettings->device, &renderPassInfo, currentVKSettings->allocationCallback, &renderPass);
	assert(result == VK_SUCCESS);
}

void VulkanRenderpassBuilder::SetFramesInFlight(VkBool32 count)
{
    framesInFlight = count;
}

void VulkanRenderpassBuilder::SetResolution(VkExtent2D _resolution)
{
	resolution = _resolution;
}

bool VulkanRenderpassBuilder::IsValid()
{
    return framesInFlight > 0;
}
