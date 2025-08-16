#include "glPCH.h"
#include "VkCubemap.h"
#include "VulkanPlatformInit.h"
#include "TextureData.h"
#include "GLSetup.h"


extern GLSetup* GGLSPtr;

VkCubemap::~VkCubemap()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult idleResult = vkDeviceWaitIdle(vkSettings->device);
	assert(idleResult == VK_SUCCESS);

	vkFreeCommandBuffers(vkSettings->device, cmdPool, MAX_VULKAN_FRAMES_IN_FLIGHT, cmdBuffers.data());
	vkDestroyCommandPool(vkSettings->device, cmdPool, vkSettings->allocationCallback);
	vkFreeMemory(vkSettings->device, cubemapParams.imageMemory, vkSettings->allocationCallback);
	vkDestroyImage(vkSettings->device, cubemapParams.img, vkSettings->allocationCallback);
	vkDestroyImageView(vkSettings->device, cubemapParams.imgView, vkSettings->allocationCallback);
	for (VkBool32 i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroyFramebuffer(vkSettings->device, framebuffers[i], vkSettings->allocationCallback);
	}
}

void VkCubemap::Setup()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult idleResult = vkDeviceWaitIdle(vkSettings->device);
	CreateCommandPool();
	CreateCommandBuffers();
	CreateRenderpass();
	CreateFramebuffers();
	CreateCubemapShaders();
	AddVertexInputBindings();
	CreateViewportState();
	CreateCubemapPipelines();
}

void VkCubemap::CreateCommandPool()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = vkSettings->queueFamilies[0];
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.pNext = NULL;


	VkResult result = vkCreateCommandPool(vkSettings->device, &poolInfo, vkSettings->allocationCallback, &cmdPool);
	if(result != VK_SUCCESS)
		throw std::runtime_error("Unable to create command pool for cubemap!");
}

void VkCubemap::CreateCommandBuffers()
{
	VkCommandBufferAllocateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferInfo.commandPool = cmdPool;
	bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferInfo.commandBufferCount = MAX_VULKAN_FRAMES_IN_FLIGHT;

	cmdBuffers.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result = vkAllocateCommandBuffers(vkSettings->device, &bufferInfo, cmdBuffers.data());
	if(result != VK_SUCCESS)
		throw std::runtime_error("Unable to create command buffers for cubemap!");
}

void VkCubemap::CreateRenderpass()
{
	// Create color attachment(s)
	VkAttachmentDescription colorAttachmentInfo = {};
	colorAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
	colorAttachmentInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	// Create depth attachment
	VkAttachmentDescription depthAttachmentInfo = {};
	depthAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
	depthAttachmentInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	std::vector<VkAttachmentDescription> attachments(2);
	attachments[0] = colorAttachmentInfo;
	attachments[1] = depthAttachmentInfo;


	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthRef = {};
	depthRef.attachment = 1;
	depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference resolveAttachments[2]{ colorRef, depthRef };

	VkBool32 preserveAttachments[2] = { 0, 1 };
	// Create Present to Depth/Stencil subpass
	VkSubpassDescription presentToDepthStencilSubpass;
	presentToDepthStencilSubpass.flags = 0;
	presentToDepthStencilSubpass.colorAttachmentCount = 1;
	presentToDepthStencilSubpass.pColorAttachments = &colorRef;
	presentToDepthStencilSubpass.pDepthStencilAttachment = &depthRef;
	presentToDepthStencilSubpass.inputAttachmentCount = 0;
	presentToDepthStencilSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	presentToDepthStencilSubpass.preserveAttachmentCount = 0;
	presentToDepthStencilSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
	presentToDepthStencilSubpass.pResolveAttachments = VK_NULL_HANDLE;
	presentToDepthStencilSubpass.pInputAttachments = VK_NULL_HANDLE;

	// Create Depth/Stencil to Color subpass
	VkSubpassDescription depthStencilToColorSubpass;
	depthStencilToColorSubpass.flags = 0;
	depthStencilToColorSubpass.colorAttachmentCount = 1;
	depthStencilToColorSubpass.pColorAttachments = &colorRef;
	depthStencilToColorSubpass.pDepthStencilAttachment = &depthRef;
	depthStencilToColorSubpass.inputAttachmentCount = 0;
	depthStencilToColorSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	depthStencilToColorSubpass.preserveAttachmentCount = 0;
	depthStencilToColorSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
	depthStencilToColorSubpass.pResolveAttachments = VK_NULL_HANDLE;
	depthStencilToColorSubpass.pInputAttachments = VK_NULL_HANDLE;

	// Create Color to Transfer subpass
	VkSubpassDescription colorToTransferSubpass;
	colorToTransferSubpass.flags = 0;
	colorToTransferSubpass.colorAttachmentCount = 1;
	colorToTransferSubpass.pColorAttachments = &colorRef;
	colorToTransferSubpass.pDepthStencilAttachment = &depthRef;
	colorToTransferSubpass.inputAttachmentCount = 0;
	colorToTransferSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	colorToTransferSubpass.preserveAttachmentCount = 0;
	colorToTransferSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
	colorToTransferSubpass.pResolveAttachments = VK_NULL_HANDLE;
	colorToTransferSubpass.pInputAttachments = VK_NULL_HANDLE;


	VkSubpassDescription subpasses[3]{ presentToDepthStencilSubpass, depthStencilToColorSubpass, colorToTransferSubpass };

	// Create Present to Depth/Stencil attachment dependency
	VkSubpassDependency presentToDepthStencilDependency;
	presentToDepthStencilDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	presentToDepthStencilDependency.dstSubpass = 0;
	presentToDepthStencilDependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	presentToDepthStencilDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	presentToDepthStencilDependency.srcAccessMask = 0;
	presentToDepthStencilDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	presentToDepthStencilDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create Depth/Stencil to Color attachment dependency
	VkSubpassDependency depthStencilToColorDependency;
	depthStencilToColorDependency.srcSubpass = 0;
	depthStencilToColorDependency.dstSubpass = 1;
	depthStencilToColorDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depthStencilToColorDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	depthStencilToColorDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	depthStencilToColorDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	depthStencilToColorDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create Color to Transfer attachment dependency
	VkSubpassDependency colorToTransferDependency;
	colorToTransferDependency.srcSubpass = 1;
	colorToTransferDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	colorToTransferDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	colorToTransferDependency.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	colorToTransferDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	colorToTransferDependency.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	colorToTransferDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	VkSubpassDependency subpassDependencies[3]{ presentToDepthStencilDependency, depthStencilToColorDependency, colorToTransferDependency };

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = &attachments[0];
	renderPassInfo.pNext = VK_NULL_HANDLE;
	renderPassInfo.subpassCount = 3;
	renderPassInfo.pSubpasses = subpasses;
	renderPassInfo.dependencyCount = 3;
	renderPassInfo.pDependencies = subpassDependencies;
	renderPassInfo.flags = NULL;
	VkResult result = vkCreateRenderPass(vkSettings->device, &renderPassInfo, vkSettings->allocationCallback, &renderpass);
	assert(result == VK_SUCCESS);

	// Load default depth/stencil operations for cubemap pipeline
	pipelineBuilder.LoadDepthStencilState(pipelineBuilderParams);
	
	// Load renderpass to cubemap pipeline
	pipelineBuilder.LoadRenderpass(renderpass);
}

void VkCubemap::CreateFramebuffers()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkImageView attachments[2]{ cubemapParams.imgView, cubemapParams.depthView };
	framebuffers.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
	VkFramebufferCreateInfo framebufferInfo;
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = VK_NULL_HANDLE;
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.flags = 0;
	framebufferInfo.layers = 6;
	framebufferInfo.attachmentCount = 2;
	framebufferInfo.renderPass = renderpass;
	framebufferInfo.pAttachments = attachments;	
	VkResult result = vkCreateFramebuffer(vkSettings->device, &framebufferInfo, vkSettings->allocationCallback, &framebuffers[0]);
	if(result != VK_SUCCESS)
		throw std::runtime_error("Unable to create framebuffers for cubemap!");
}

void VkCubemap::CreateViewportState()
{
	auto renderingPipeline = GGLSPtr->pipeline;
	VkBool32 viewportCount, scissorCount;
	renderingPipeline->GetViewportInfo(viewportCount, VK_NULL_HANDLE, scissorCount, VK_NULL_HANDLE);
	std::vector<VkViewport> viewports;
	viewports.resize(viewportCount);
	std::vector<VkRect2D> scissors;
	scissors.resize(scissorCount);
	renderingPipeline->GetViewportInfo(viewportCount, viewports.data(), scissorCount, scissors.data());
	

	pipelineBuilder.LoadViewportInfo(pipelineBuilderParams, viewportCount, viewports.data(), scissorCount, scissors.data());
}

void VkCubemap::GenerateCubeMap(std::vector<TextureData*> cubemapTextureData)
{
	// ensure that all sides of the cubemap have texture data
	assert((int)cubemapTextureData.size() == 6);
	// create cubemap
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	GGLSPtr->GetWindowDimensions(width, height);

	
	cubemapParams.images = cubemapTextureData;

	VkImageCreateInfo cubemapInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_B8G8R8A8_UNORM,
		.extent = {(VkBool32)cubemapTextureData[0]->width, (VkBool32)cubemapTextureData[0]->height, 1},
		.mipLevels = 1,
		.arrayLayers = 6,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &vkSettings->queueFamilies[0],
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};

	VkPhysicalDeviceImageFormatInfo2 supportedImgFmtInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
		.pNext = VK_NULL_HANDLE,
		.format = cubemapInfo.format,
		.type = cubemapInfo.imageType,
		.usage = cubemapInfo.usage,
		.flags = cubemapInfo.flags
	};
	std::vector<VkImageFormatProperties2> supportedImgFmts;
	for (VkBool32 i = VK_FORMAT_R8G8B8_UNORM; i <= VK_FORMAT_R8G8B8A8_UNORM; ++i)
	{
		supportedImgFmtInfo.format = (VkFormat)i;
		VkImageFormatProperties2 supportedImgFmtProperties =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
			.pNext = VK_NULL_HANDLE
		};
		vkGetPhysicalDeviceImageFormatProperties2(vkSettings->physicalDevices[vkSettings->discreteGPUIndex], &supportedImgFmtInfo, &supportedImgFmtProperties);
		if (supportedImgFmtProperties.imageFormatProperties.maxExtent.height > 0)
			supportedImgFmts.push_back(supportedImgFmtProperties);
	}


	VkResult result = vkCreateImage(vkSettings->device, &cubemapInfo, vkSettings->allocationCallback, &cubemapParams.img);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to create Cubemap!");
	}


	VkMemoryRequirements imgMemReqs;
	vkGetImageMemoryRequirements(vkSettings->device, cubemapParams.img, &imgMemReqs);
	VkMemoryPropertyFlags imgMemPropertiesFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	// find out if memory type is supported in memory buffer
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkSettings->physicalDevices[vkSettings->discreteGPUIndex], &memoryProperties);

	// Flags for allowing the device memory to be accessible and malleable to application code
	VkBool32 memoryFlagIndex = -1;
	for (VkBool32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (imgMemReqs.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & imgMemPropertiesFlags) == imgMemPropertiesFlags)
		{
			memoryFlagIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo imgMemInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.allocationSize = imgMemReqs.size,
		.memoryTypeIndex = memoryFlagIndex
	};
	result = vkAllocateMemory(vkSettings->device, &imgMemInfo, vkSettings->allocationCallback, &cubemapParams.imageMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate memory for image!");
	}

	result = vkBindImageMemory(vkSettings->device, cubemapParams.img, cubemapParams.imageMemory, 0);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to bind Cubemap memory!");
	}

	VkImageViewCreateInfo cubemapViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = 0,
		.flags = 0,
		.image = cubemapParams.img,
		.viewType = VK_IMAGE_VIEW_TYPE_CUBE,
		.format = cubemapInfo.format,
		.components =
					{
						VK_COMPONENT_SWIZZLE_R,
						VK_COMPONENT_SWIZZLE_G,
						VK_COMPONENT_SWIZZLE_B
					},
		.subresourceRange =
					{
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel = 0,
						.levelCount = 1,
						.baseArrayLayer = 0,
						.layerCount = 6
					}

	};

	result = vkCreateImageView(vkSettings->device, &cubemapViewInfo, vkSettings->allocationCallback, &cubemapParams.imgView);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to create image view!");
	}

	cubemapParams.stagingBufferSize = cubemapTextureData[0]->width * cubemapTextureData[0]->height * cubemapTextureData[0]->channels * 6;
	cubemapParams.layerSize = cubemapParams.stagingBufferSize / 6;

	VkBufferCreateInfo stagingBufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.size = cubemapParams.stagingBufferSize,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	result = vkCreateBuffer(vkSettings->device, &stagingBufferInfo, vkSettings->allocationCallback, &cubemapParams.stagingBuffer);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to create buffer for cubemap!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkSettings->device, cubemapParams.stagingBuffer, &memoryRequirements);

	VkMemoryPropertyFlags  memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	// Flags for allowing the device memory to be accessible and malleable to application code
	memoryFlagIndex = -1;
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
	VkMemoryAllocateInfo memoryInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.allocationSize = memoryRequirements.size,
		.memoryTypeIndex = memoryFlagIndex
	};
	result = vkAllocateMemory(vkSettings->device, &memoryInfo, vkSettings->allocationCallback, &cubemapParams.stagingMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate memory for cubemap!");
	}

	vkBindBufferMemory(vkSettings->device, cubemapParams.stagingBuffer, cubemapParams.stagingMemory, 0);

	VkImageSubresourceLayers cubemapImgSubResourceLayer;
	cubemapImgSubResourceLayer.aspectMask = cubemapViewInfo.subresourceRange.aspectMask;
	cubemapImgSubResourceLayer.mipLevel = 0;
	cubemapImgSubResourceLayer.baseArrayLayer = 0;
	cubemapImgSubResourceLayer.layerCount = 6;

	VkBufferImageCopy imgCopyInfo;
	imgCopyInfo.bufferOffset = 0;
	imgCopyInfo.bufferImageHeight = cubemapTextureData[0]->height;
	imgCopyInfo.bufferRowLength = cubemapTextureData[0]->width;
	imgCopyInfo.imageExtent = VkExtent3D(cubemapTextureData[0]->width, cubemapTextureData[0]->height, 1);
	imgCopyInfo.imageOffset = VkOffset3D(0, 0, 0);
	imgCopyInfo.imageSubresource = cubemapImgSubResourceLayer;
	
	VkDeviceSize offset = 0;
	VkDeviceSize cubemapImgSize = cubemapTextureData[0]->width * cubemapTextureData[0]->height;
	for (VkBool32 i = 0; i < 6; ++i)
	{
		imgCopyInfo.bufferOffset = offset;
		// TODO find the origin point of the cubemap
		// to properly set the image offsets
		//// right side
		//if (i == 0)
		//{
		//	imgCopyInfo.imageOffset = VkOffset3D
		//}
		//// left side
		//if (i == 1)
		//{
		//}
		//// top side
		//if (i == 2)
		//{

		//}
		//// bottom side
		//if (i == 3)
		//{

		//}
		//// forward side
		//if (i == 4)
		//{

		//}
		//// bottom side
		//if (i == 5)
		//{
		//}
		cubemapImgCopyInfo.push_back(imgCopyInfo);
		offset += cubemapImgSize;
	}


	void* dataBinding;
	result = vkMapMemory(vkSettings->device, cubemapParams.stagingMemory, 0, cubemapParams.stagingBufferSize, 0, &dataBinding);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to map memory to cubemap images!");
	}
	VkDeviceSize memOffset = 0;
	for (int i = 0; i < 6; ++i)
	{

		memcpy((char*)dataBinding + memOffset, cubemapTextureData[i]->data, cubemapParams.layerSize);
		memOffset += cubemapParams.layerSize;
	}
	vkUnmapMemory(vkSettings->device, cubemapParams.stagingMemory);


	// Create Depth Buffer

	VkImageCreateInfo imgCreateInfo = vkSettings->depthImageInfo;
	imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgCreateInfo.pNext = VK_NULL_HANDLE;
	imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imgCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imgCreateInfo.extent = { (VkBool32)cubemapTextureData[0]->width, (VkBool32)cubemapTextureData[0]->height, 1 },
	imgCreateInfo.extent.depth = 1;
	imgCreateInfo.mipLevels = 1;
	imgCreateInfo.arrayLayers = 6;
	imgCreateInfo.samples = VK_NUM_OF_SAMPLES;
	imgCreateInfo.queueFamilyIndexCount = 1;
	imgCreateInfo.pQueueFamilyIndices = &vkSettings->queueFamilies[0];
	imgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imgCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	result = vkCreateImage(vkSettings->device, &imgCreateInfo, vkSettings->allocationCallback, &cubemapParams.depthImg);

	if (result != VK_SUCCESS)
		throw std::runtime_error("Unable to create depth buffer image object for cubemap!");

	// Allocate memory for the depth buffer
	VkMemoryRequirements depthBufMemReqs;
	vkGetImageMemoryRequirements(vkSettings->device, cubemapParams.depthImg, &depthBufMemReqs);
	VkMemoryAllocateInfo depthBufMemoryInfo = {};
	depthBufMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	depthBufMemoryInfo.allocationSize = depthBufMemReqs.size;

	result = vkAllocateMemory(vkSettings->device, &depthBufMemoryInfo, vkSettings->allocationCallback, &cubemapParams.depthMemory);

	if (result != VK_SUCCESS)
		throw std::runtime_error("Unable to allocate memory for the Depth Buffer!");

	// Bind memory buffer to depth buffer
	vkBindImageMemory(vkSettings->device, cubemapParams.depthImg, cubemapParams.depthMemory, 0);

	VkImageViewCreateInfo depthViewInfo;
	depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthViewInfo.pNext = VK_NULL_HANDLE;
	depthViewInfo.flags = 0;
	depthViewInfo.image = cubemapParams.depthImg;
	depthViewInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depthViewInfo.subresourceRange.baseMipLevel = 0;
	depthViewInfo.subresourceRange.levelCount = 1;
	depthViewInfo.subresourceRange.baseArrayLayer = 0;
	depthViewInfo.subresourceRange.layerCount = 6;
	depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;

	result = vkCreateImageView(vkSettings->device, &depthViewInfo, vkSettings->allocationCallback, &cubemapParams.depthView);

	if (result != VK_SUCCESS)
		throw std::runtime_error("Unable to create depth buffer image view for cubemap!");
}

void VkCubemap::CopyCubeMapToBuffer(VkBool32 frameIndex)
{
	vkCmdCopyBufferToImage(cmdBuffers[frameIndex], cubemapParams.stagingBuffer, cubemapParams.img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, (VkBool32)cubemapImgCopyInfo.size(), cubemapImgCopyInfo.data());
}

void VkCubemap::CreateCubemapShaders()
{
	// Load vertex and fragment shader data from external SPIR-V file
	shaderConfigs.AddFileForShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "CubeMapVert.spv");
	shaderConfigs.AddFileForShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "CubeMapFrag.spv");
	// Create vertex and fragment shaders in vulkan
	assert(VkShaderUtil::LoadVertexShaderModule(shaderConfigs, &cubemapParams.vertexShader));
	assert(VkShaderUtil::LoadFragmentShaderModule(shaderConfigs, &cubemapParams.fragmentShader));

	// add shader modules to cubemap pipeline
	pipelineBuilder.LoadShaderModule(shaderConfigs, pipelineBuilderParams);

	// Set data and offsets for bindings in vertex and fragment shaders
	VkPushConstantsInfo pushConstantsForVertexShader;
	pushConstantsForVertexShader.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
	pushConstantsForVertexShader.pNext = VK_NULL_HANDLE;
	pushConstantsForVertexShader.layout;
	pushConstantsForVertexShader.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantsForVertexShader.offset = 0;
	pushConstantsForVertexShader.size = sizeof(CubemapVertConstants);

	VkPushConstantsInfo pushConstantsForFragmentShader;
	pushConstantsForFragmentShader.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
	pushConstantsForFragmentShader.pNext = VK_NULL_HANDLE;
	pushConstantsForFragmentShader.layout;
	pushConstantsForFragmentShader.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantsForFragmentShader.offset = 0;
	pushConstantsForFragmentShader.size = sizeof(CubemapFragConstants);

	VkPushConstantRange vertexBindingRange;
	vertexBindingRange.offset = 0;
	vertexBindingRange.size = sizeof(CubemapVertConstants);

	VkPushConstantRange fragmentBindingRange;
	fragmentBindingRange.offset = 0;
	fragmentBindingRange.size = sizeof(CubemapFragConstants);

	VkDescriptorSetLayoutBinding vertexShaderObjectPropertyBinding;
	vertexShaderObjectPropertyBinding.binding = 0;
	vertexShaderObjectPropertyBinding.descriptorType = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
	vertexShaderObjectPropertyBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderObjectPropertyBinding.descriptorCount = sizeof(CubemapObjectProperties);
	vertexShaderObjectPropertyBinding.pImmutableSamplers = VK_NULL_HANDLE;

	VkDescriptorSetLayoutCreateInfo vertexDescriptorSetLayoutInfo;
	vertexDescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	vertexDescriptorSetLayoutInfo.flags = 0;
	vertexDescriptorSetLayoutInfo.pNext = VK_NULL_HANDLE;
	vertexDescriptorSetLayoutInfo.bindingCount = 1;
	vertexDescriptorSetLayoutInfo.pBindings = &vertexShaderObjectPropertyBinding;


	VkDescriptorSetLayoutBinding fragmentShaderSkyBoxBinding;
	fragmentShaderSkyBoxBinding.binding = 2;
	fragmentShaderSkyBoxBinding.descriptorCount = 1;
	fragmentShaderSkyBoxBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	fragmentShaderSkyBoxBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderSkyBoxBinding.pImmutableSamplers = VK_NULL_HANDLE;

	VkDescriptorSetLayoutCreateInfo fragmentDescriptorSetLayoutInfo;
	fragmentDescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	fragmentDescriptorSetLayoutInfo.flags = 0;
	fragmentDescriptorSetLayoutInfo.pNext = VK_NULL_HANDLE;
	fragmentDescriptorSetLayoutInfo.bindingCount = 1;
	fragmentDescriptorSetLayoutInfo.pBindings = &fragmentShaderSkyBoxBinding;

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VkResult layoutResult = vkCreateDescriptorSetLayout(vkSettings->device, &vertexDescriptorSetLayoutInfo, vkSettings->allocationCallback, &cubemapParams.vertexLayout);
	assert(layoutResult == VK_SUCCESS);
	layoutResult = vkCreateDescriptorSetLayout(vkSettings->device, &fragmentDescriptorSetLayoutInfo, vkSettings->allocationCallback, &cubemapParams.fragmentLayout);
	assert(layoutResult == VK_SUCCESS);

	VkPushConstantRange constants[2]{ vertexBindingRange, fragmentBindingRange };
	VkDescriptorSetLayout layouts[2]{ cubemapParams.vertexLayout, cubemapParams.fragmentLayout };
	VkPipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = VK_NULL_HANDLE;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = VK_NULL_HANDLE;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount= 2;
	pipelineLayoutInfo.pSetLayouts = layouts;

	VkResult pipelineLayoutResult = vkCreatePipelineLayout(vkSettings->device, &pipelineLayoutInfo, vkSettings->allocationCallback, &cubemapParams.pipelineLayout);
	assert(pipelineLayoutResult == VK_SUCCESS);
	
	pushConstantsForVertexShader.layout = cubemapParams.pipelineLayout;
	pushConstantsForFragmentShader.layout = cubemapParams.pipelineLayout;



	pipelineBuilder.LoadPipelineLayout(pipelineBuilderParams, &cubemapParams.pipelineLayout);
	// Add to descriptor set
	// Add to graphics pipeline for cubemap
}

void VkCubemap::AddVertexInputBindings()
{
	VkVertexInputBindingDescription vertexBindingInfo;
	vertexBindingInfo.binding =0;
	vertexBindingInfo.stride = sizeof(CubemapVertConstants);
	vertexBindingInfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexAttrInfo;
	vertexAttrInfo.binding = 0;
	vertexAttrInfo.format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexAttrInfo.location = 0;
	vertexAttrInfo.offset = offsetof(CubemapVertConstants, position);

	VkVertexInputAttributeDescription texCoordAttrInfo;
	texCoordAttrInfo.binding = 0;
	texCoordAttrInfo.location = 1;
	texCoordAttrInfo.format = VK_FORMAT_R32G32B32_SFLOAT;
	texCoordAttrInfo.offset = offsetof(CubemapVertConstants, texCoord);

	VkVertexInputAttributeDescription vertexInputAttributes[2]{ vertexAttrInfo, texCoordAttrInfo };

	VkVertexAttributeToBindingMapping vertexMapping;
	vertexMapping.inputBinding = vertexBindingInfo;
	vertexMapping.attributesForBinding.push_back(vertexAttrInfo);
	vertexMapping.attributesForBinding.push_back(texCoordAttrInfo);

	pipelineBuilderParams.vertexBindingMappings.push_back(vertexMapping);

	VkPipelineVertexInputStateCreateInfo pipelineVertexStateInfo;
	pipelineVertexStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexStateInfo.pNext = VK_NULL_HANDLE;
	pipelineVertexStateInfo.flags = 0;
	pipelineVertexStateInfo.vertexBindingDescriptionCount = 1;
	pipelineVertexStateInfo.pVertexBindingDescriptions = &pipelineBuilderParams.vertexBindingMappings.front().inputBinding;
	pipelineVertexStateInfo.vertexAttributeDescriptionCount = 2;
	pipelineVertexStateInfo.pVertexAttributeDescriptions = pipelineBuilderParams.vertexBindingMappings.front().attributesForBinding.data();

	pipelineBuilder.BuildVertexInputState(pipelineBuilderParams, pipelineVertexStateInfo);
}

void VkCubemap::CreateCubemapPipelines()
{
	auto pipelinePtr = pipelineBuilder.GetPipelineInfo();

	// Create the remaining parameters for the first subpass pipeline 
	pipelinePtr->subpass = 0;
	pipelineBuilder.LoadMultispamplingState(pipelineBuilderParams);
	pipelineBuilder.LoadColorBlendState(pipelineBuilderParams);
	pipelinePtr->pDynamicState = &defaultDynamicState;
	pipelinePtr->pRasterizationState = &defaultRasterizationState;
	pipelinePtr->pInputAssemblyState = &defaultInputAssemblyState;
	
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult pipelineResult = vkCreateGraphicsPipelines(vkSettings->device, vkSettings->pipelineCache, 1, pipelinePtr, vkSettings->allocationCallback, &cubemapParams.pipeline);
	assert(pipelineResult == VK_SUCCESS);
	// Create pipelines for the remain subpasses using the pipeline of the first subpass as the base
}
