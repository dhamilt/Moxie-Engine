#include "glPCH.h"
#include "WViewport.h"
#include "VulkanFunctionLibrary.h"
#include "VulkanPlatformInit.h"
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
	CreateViewportImages();
	CreateViewportImageViews();
	CreateViewportImageSamples();	
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
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	paramCollection.clear();

	for(VkFramebuffer framebuffer : frameBuffers)
		vkDestroyFramebuffer(vkSettings->device, framebuffer,vkSettings->allocationCallback);
#endif
}

void WViewport::Paint()
{	
	// TODO: Return true if Viewport ImGui window is focused
	// Get the current window	
	if (ImGui::IsWindowFocused())
		printf("Viewport is focused\n");

	ImGui::SetNextWindowSize(ImVec2(width, height));
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

void WViewport::CreateViewportFramebuffers(VkRenderPass _renderpass)
{
	renderpass = static_cast<VkRenderPass>(_renderpass);
	std::vector<VkImageView> imgViews;
	for (auto it = paramCollection.begin(); it != paramCollection.end(); ++it)
		imgViews.push_back(it->viewportImgView);
	
	frameBuffers = VulkanFunctionLibrary::CreateDefaultFramebuffers(MAX_VULKAN_FRAMES_IN_FLIGHT, &imgViews[0], VkExtent2D(width, height), renderpass);
}

void WViewport::ResizeFramebuffers(int _width, int _height)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	for(VkFramebuffer framebuf : frameBuffers)
		vkDestroyFramebuffer(vkSettings->device, framebuf, vkSettings->allocationCallback);
	frameBuffers.clear();
	width = _width;
	height = _height;
	CreateViewportFramebuffers(renderpass);

}

bool WViewport::CreateViewportImages()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkSwapchainCreateInfoKHR* swapchainInfo = &vkSettings->swapchainInfo;
	VkImageCreateInfo imgInfo;
	imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgInfo.pNext = VK_NULL_HANDLE;
	imgInfo.flags = 0;
	imgInfo.imageType = VK_IMAGE_TYPE_2D;
	imgInfo.format = swapchainInfo->imageFormat;
	imgInfo.extent.width = swapchainInfo->imageExtent.width;
	imgInfo.extent.height = swapchainInfo->imageExtent.height;
	imgInfo.extent.depth = 1;
	imgInfo.arrayLayers = swapchainInfo->imageArrayLayers;
	imgInfo.mipLevels = 1;
	imgInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imgInfo.sharingMode = swapchainInfo->imageSharingMode;
	imgInfo.queueFamilyIndexCount = 1;
	imgInfo.pQueueFamilyIndices = &vkSettings->queueFamilies.front();
	
	width = swapchainInfo->imageExtent.width;
	height = swapchainInfo->imageExtent.height;

	for (int i = 0; i < paramCollection.size(); ++i)
	{
		auto current = &paramCollection[i];
		VkResult result = vkCreateImage(vkSettings->device, &imgInfo, vkSettings->allocationCallback, &current->viewportImg);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Unable to create image for viewport!");

		// Allocate memory for the image 
		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(vkSettings->device, current->viewportImg, &memReqs);
		VkMemoryAllocateInfo memoryInfo = {};
		memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryInfo.allocationSize = memReqs.size;


		result = vkAllocateMemory(vkSettings->device, &memoryInfo, vkSettings->allocationCallback, &current->imgMemory);

		if (result != VK_SUCCESS)
		{
			perror("Error! Unable to allocate memory for the viewport image!");
			return false;
		}

		// Bind memory buffer to image
		result = vkBindImageMemory(vkSettings->device, current->viewportImg, current->imgMemory, 0);

		if (result != VK_SUCCESS)
		{
			perror("Error! Unable to bind memory to viewport image!");
			return false;
		}
	}
	return true;
}

bool WViewport::CreateViewportImageViews()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	auto imgViewInfo = vkSettings->swapchainImgViewInfo;	
	for (int i = 0; i < paramCollection.size(); ++i)
	{
		auto current = &paramCollection[i];
		imgViewInfo.image = current->viewportImg;
		VkResult result = vkCreateImageView(vkSettings->device, &imgViewInfo, vkSettings->allocationCallback, &current->viewportImgView);
		assert(result == VK_SUCCESS);
		
	}
	return true;
}

bool WViewport::CreateViewportImageSamples()
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
				.anisotropyEnable = VK_FALSE,
				.compareEnable = VK_FALSE,
				.minLod = -1000,
				.maxLod = 1000,
				.unnormalizedCoordinates = VK_FALSE
				};

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	for(int i = 0; i < paramCollection.size(); ++i)
	{
		auto current = &paramCollection[i];
		VkResult result =  vkCreateSampler(vkSettings->device, &viewportSmplrInfo, vkSettings->allocationCallback, &current->sampler);
		if(result != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create sampler for viewport image!");
		}
	}
	return true;
}

void WViewport::StartViewportRenderpass(VkCommandBuffer buffer, VkRenderPass renderpass, VkBool32 frameIndex)
{


	VkClearValue clearVal;
	clearVal.color = {0.0f, 0.0f, 0.0f, 1.0f};
	VkRenderPassBeginInfo renderpassInfo;
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpassInfo.pNext = VK_NULL_HANDLE;
	renderpassInfo.renderPass = renderpass;
	renderpassInfo.framebuffer = frameBuffers[frameIndex];
	renderpassInfo.renderArea = {0, 0, (VkBool32)width, (VkBool32)height};
	renderpassInfo.clearValueCount = 1;
	renderpassInfo.pClearValues = &clearVal;
	vkCmdBeginRenderPass(buffer, &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void WViewport::EndViewportRenderPass(VkCommandBuffer buffer)
{
	vkCmdEndRenderPass(buffer);
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
	

	

	VulkanFunctionLibrary::TransitionImageLayout(cmdBuffer, paramCollection[frameIndex].viewportImg, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	
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

	VulkanFunctionLibrary::TransitionImageLayout(cmdBuffer, paramCollection[frameIndex].viewportImg, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	currentRenderingFrame = frameIndex;
}

void WViewport::VkSetViewportImg(VkCommandBuffer cmdBuffer, VkBool32 frameIndex)
{
	if (paramCollection[frameIndex].descriptorSet != VK_NULL_HANDLE)
		ImGui_ImplVulkan_RemoveTexture(paramCollection[frameIndex].descriptorSet);
	paramCollection[frameIndex].descriptorSet = ImGui_ImplVulkan_AddTexture(paramCollection[frameIndex].sampler, paramCollection[frameIndex].viewportImgView, VK_IMAGE_LAYOUT_GENERAL);
}
