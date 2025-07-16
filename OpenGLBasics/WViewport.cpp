#include "glPCH.h"
#include "WViewport.h"
#include "VulkanFunctionLibrary.h"
#include "GLSetup.h"


extern GLSetup* GGLSPtr;
WViewport::WViewport()
{
	name = typeid(*this).name();

	// Grab the viewport texture/render target
	GGLSPtr->GetViewportTextureID(textureID, rbo);
	// Retrieve the initial dimensions of the viewport
	GGLSPtr->GetViewportDimensions(width, height);	

#if USE_VULKAN
	paramCollection.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
#endif
}

ViewportImageParamsVk::~ViewportImageParamsVk()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	vkFreeDescriptorSets(vkSettings->device, vkSettings->descriptorPool, 1, &descriptorSet);
	vkFreeMemory(vkSettings->device, imgMemory, vkSettings->allocationCallback);
	vkDestroyImageView(vkSettings->device, viewportImgView, vkSettings->allocationCallback);
	vkDestroyImage(vkSettings->device, viewportImg, vkSettings->allocationCallback);
	vkDestroySampler(vkSettings->device, sampler, vkSettings->allocationCallback);
}

WViewport::~WViewport()
{
#if USE_OPENGL
	if (fbo > 0)
		glDeleteFramebuffers(1, &fbo);
	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);
	if (pixelData)
		free(pixelData);
	if (textureID > 0)
		glDeleteTextures(1, &textureID);
#elif USE_VULKAN

#endif
}

void WViewport::Paint()
{	
	// TODO: Return true if Viewport ImGui window is focused
	// Get the current window	
	if (ImGui::IsWindowFocused())
		printf("Viewport is focused\n");

	ImGui::Begin("Viewport", &windowOpen);

#if USE_OPENGL
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, textureID);	
	
	// Bind the render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	// If the viewport image has been resized
	if ((int)windowSize.x != width || (int)windowSize.y != height)
	{
		// Cache the new dimensions
		width = (int)windowSize.x;
		height = (int)windowSize.y;
		// Store the pixel data for this frame
		pixelCount = width * height;
		if (!pixelData)
			pixelData = (GLubyte*)malloc(sizeof(char) * pixelCount * bytesPerPixel);
		else
			pixelData = (GLubyte*)realloc(pixelData,sizeof(char) * pixelCount * bytesPerPixel);

		assert(pixelData);
		

		// Resize the color attachment texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
		// As well as the depth attachment
		// Store the depth and stencil data into the render buffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);



	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		ImGui::Image((ImTextureID)textureID, ImGui::GetWindowSize(),ImVec2(0, 0), ImVec2(1,-1));
	
	else
		FrameBufferErrorCheck(glCheckFramebufferStatus(GL_FRAMEBUFFER));

	
#elif USE_VULKAN
	ImGui::Image((ImTextureID)paramCollection[currentRenderingFrame].descriptorSet, ImGui::GetContentRegionAvail());
#endif
	ImGui::End();
	
}

void WViewport::CreateViewportFramebuffers()
{
	std::vector<VkImageView> imgViews;
	for (auto it = paramCollection.begin(); it != paramCollection.end(); ++it)
		imgViews.push_back(it->viewportImgView);
	
	frameBuffers = VulkanFunctionLibrary::CreateDefaultFramebuffers(MAX_VULKAN_FRAMES_IN_FLIGHT, &imgViews[0], VkExtent2D(width, height), renderPass);
}

bool WViewport::CreateViewportSwapchain()
{
	return false;
}

void WViewport::CreateViewportRenderPass()
{
	renderPass = VulkanFunctionLibrary::CreateDefaultRenderpass();
}

void WViewport::CreateViewportCommandBuffers()
{
	cmdBuffers = VulkanFunctionLibrary::CreateDefaultCommandBuffers(MAX_VULKAN_FRAMES_IN_FLIGHT);
}

void WViewport::AttachToImGUI()
{

}

bool WViewport::operator==( GUI_Base& other)
{
	return GetName() == other.GetName();
}

bool WViewport::FrameBufferErrorCheck(GLuint status)
{
	bool result = false;

	switch (status)
	{
		case GL_FRAMEBUFFER_UNDEFINED:
			printf("\rFramebuffer is not defined.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("\rFramebuffer attachments are incomplete.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("\rFramebuffer does not have at least one image attached to it.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			printf("\rDraw buffer cannot not be found in framebuffer.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("\rRead buffer cannot not be found in framebuffer.");
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("\rThe internal formats for this framebuffer are not supported");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			printf("\rFramebuffer samples are not all the same for the attached render buffers.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			printf("\rFramebuffer is layered and the attachment is not.");
			break;

		default:
			result = true;
			break;
	}
	return result;
}

void WViewport::VkCopySwapchainImg(VkCommandBuffer cmdBuffer, VkBool32 frameIndex)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	auto swapChainImage = vkSettings->swapchainImages[frameIndex];
	auto swapChainImageView = vkSettings->swapChainImgBufs[frameIndex].imageView;
	
	VkResult result;
	VkMemoryRequirements imgMemReqs;
		{
			VkImageCreateInfo viewportImgInfo =
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.pNext = VK_NULL_HANDLE,
				.flags = 0,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = VK_FORMAT_B8G8R8A8_UNORM,
				.extent = {static_cast<VkBool32>(width), static_cast<VkBool32>(height), 1},
				.mipLevels = 1,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1,
				.pQueueFamilyIndices = &vkSettings->queueFamilies[0],
				.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED
			};


			result = vkCreateImage(vkSettings->device, &viewportImgInfo, vkSettings->allocationCallback, &paramCollection[frameIndex].viewportImg);
			if (result != VK_SUCCESS)
			{
				throw new std::runtime_error("Unable to create image for viewport!");
			}


			vkGetImageMemoryRequirements(vkSettings->device, paramCollection[frameIndex].viewportImg, &imgMemReqs);
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
			result = vkAllocateMemory(vkSettings->device, &imgMemInfo, vkSettings->allocationCallback, &paramCollection[frameIndex].imgMemory);
			if (result != VK_SUCCESS)
			{
				throw new std::runtime_error("Unable to allocate memory for image!");
			}

			result = vkBindImageMemory(vkSettings->device, paramCollection[frameIndex].viewportImg, paramCollection[frameIndex].imgMemory, 0);
			if (result != VK_SUCCESS)
			{
				throw new std::runtime_error("Unable to bind viewport image memory!");
			}

			VulkanFunctionLibrary::TransitionImageLayout(paramCollection[frameIndex].viewportImg, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		}


		{
			VkImageViewCreateInfo viewportImgViewInfo = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = VK_NULL_HANDLE,
				.flags = 0,
				.image = paramCollection[frameIndex].viewportImg,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_B8G8R8A8_UNORM,
				.components = {	VK_COMPONENT_SWIZZLE_R,
								VK_COMPONENT_SWIZZLE_G,
								VK_COMPONENT_SWIZZLE_B,
								VK_COMPONENT_SWIZZLE_A },
				.subresourceRange = {
									VK_IMAGE_ASPECT_COLOR_BIT,
									0,
									1,
									0,
									1}

			};

			result = vkCreateImageView(vkSettings->device, &viewportImgViewInfo, vkSettings->allocationCallback, &paramCollection[frameIndex].viewportImgView);
			if (result != VK_SUCCESS)
			{
				throw new std::runtime_error("Unable to create image view for viewport!");
			}
		}

		{
			VkSamplerCreateInfo viewportSmplrInfo = {
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.pNext = VK_NULL_HANDLE,
				.flags = 0, 
				.magFilter = VK_FILTER_LINEAR,
				.minFilter = VK_FILTER_LINEAR,
				.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
				.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.maxAnisotropy = 1.0f,
				.minLod = -1000, 
				.maxLod = 1000				
			};

			result = vkCreateSampler(vkSettings->device, &viewportSmplrInfo, vkSettings->allocationCallback, &paramCollection[frameIndex].sampler);
			if (result != VK_SUCCESS)
			{
				throw new std::runtime_error("Unable to create image texture for viewport!");
			}

		}
	
	VkImageSubresourceLayers colorAttachLayer = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.mipLevel = 0,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	VkImageCopy colorAttachImg = {
		.srcSubresource = colorAttachLayer,
		.srcOffset = {0,0,0},
		.dstSubresource = colorAttachLayer,
		.dstOffset = {0,0,0},
		.extent = {static_cast<VkBool32>(width), static_cast<VkBool32>(height), 1}
	};
	

	vkCmdCopyImage(cmdBuffer, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, paramCollection[frameIndex].viewportImg, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &colorAttachImg);

	// Create a staging buffer for swapchain images to viewport
	VkBufferCreateInfo bufferInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.size = static_cast<VkBool32>(width) * static_cast<VkBool32>(height) * 4,
		.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = vkSettings->queueFamilies.data()
	};


	result = vkCreateBuffer(vkSettings->device, &bufferInfo, vkSettings->allocationCallback, &paramCollection[frameIndex].buffer);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to reserve memory for buffer!");
	}


	// Retrieve memory requirements for setting up memory buffer
	vkGetBufferMemoryRequirements(vkSettings->device, paramCollection[frameIndex].buffer, &imgMemReqs);

	// find out if memory type is supported in memory buffer
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkSettings->physicalDevices.front(), &memoryProperties);
	// Flags for allowing the device memory to be accessible and malleable to application code
	VkBool32 memoryFlagIndex = -1;
	for (VkBool32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (imgMemReqs.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
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
		.allocationSize = imgMemReqs.size,
		.memoryTypeIndex = memoryFlagIndex
	};


	result = vkAllocateMemory(vkSettings->device, &memoryAllocationInfo, vkSettings->allocationCallback, &paramCollection[frameIndex].stagingMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate device memory for buffer!");
	}

	// Bind device memory to buffer
	vkBindBufferMemory(vkSettings->device, paramCollection[frameIndex].buffer, paramCollection[frameIndex].stagingMemory, 0);

	// Copy swapchain image data to buffer
	VkBufferImageCopy bufCpyParams = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = colorAttachLayer,
		.imageExtent = colorAttachImg.extent
	};
	
	vkCmdCopyImageToBuffer(cmdBuffer, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, paramCollection[frameIndex].buffer, 1, &bufCpyParams);

	paramCollection[frameIndex].descriptorSet = ImGui_ImplVulkan_AddTexture(paramCollection[frameIndex].sampler, paramCollection[frameIndex].viewportImgView, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
	currentRenderingFrame = frameIndex;
}

void WViewport::VkSetViewportImg(VkCommandBuffer cmdBuffer, VkBool32 frameIndex)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	auto swapChainImage = vkSettings->swapchainImages[frameIndex];

	currentRenderingFrame = frameIndex;

	VkImageSubresourceLayers colorAttachLayer = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.mipLevel = 0,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	VkImageCopy colorAttachImg = {
		.srcSubresource = colorAttachLayer,
		.srcOffset = {0,0,0},
		.dstSubresource = colorAttachLayer,
		.dstOffset = {0,0,0},
		.extent = {static_cast<VkBool32>(width), static_cast<VkBool32>(height), 1}
	};


	// Copy swapchain image data to buffer
	VkBufferImageCopy bufCpyParams = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = colorAttachLayer,
		.imageExtent = colorAttachImg.extent
	};

	vkCmdCopyImageToBuffer(cmdBuffer, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, paramCollection[frameIndex].buffer, 1, &bufCpyParams);


	vkCmdCopyImage(cmdBuffer, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, paramCollection[frameIndex].viewportImg, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &colorAttachImg);
	VkDescriptorImageInfo descriptorImgInfo;
	descriptorImgInfo.sampler = paramCollection[frameIndex].sampler;
	descriptorImgInfo.imageView = paramCollection[frameIndex].viewportImgView;
	descriptorImgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkDescriptorBufferInfo descriptorBufInfo = 
	{
		.buffer = paramCollection[frameIndex].buffer,
		.offset = 0,
		.range = colorAttachImg.extent.width * colorAttachImg.extent.height * 4
	};

	
	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.dstSet = paramCollection[frameIndex].descriptorSet;	
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &descriptorBufInfo;
	descriptorWrite.pImageInfo = &descriptorImgInfo;
	

	//vkUpdateDescriptorSets(vkSettings->device, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
}
