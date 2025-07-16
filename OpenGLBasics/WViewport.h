#pragma once
#include "GUI_Base.h"

class PVulkanPlatformInit;

struct ViewportImageParamsVk
{
	VkDescriptorSet descriptorSet;
	VkSampler sampler;
	VkDeviceSize imgSize;
	VkDeviceMemory imgMemory,stagingMemory;
	VkBuffer buffer;
	VkImage viewportImg;
	VkImageView viewportImgView;
	ViewportImageParamsVk() { memset(this, 0, sizeof(*this)); }
	~ViewportImageParamsVk();
	
};

// GUI element that mimics the pixel data of whats being drawn on GL viewport(s)
class WViewport : public GUI_Base
{
public:
	WViewport();
	~WViewport();
	virtual void Paint() override;
	void CreateViewportFramebuffers();
	bool CreateViewportSwapchain();
	void CreateViewportRenderPass();
	void CreateViewportCommandBuffers();
	void AttachToImGUI();
	bool operator==( GUI_Base& other);
	bool FrameBufferErrorCheck(GLuint status);
	void VkCopySwapchainImg(VkCommandBuffer cmdBuffer, VkBool32 frameIndex);
	void VkSetViewportImg(VkCommandBuffer cmdBuffer, VkBool32 frameIndex);
	

private:
	int width = 800, height = 600;
	GLuint fbo, textureID, rbo;
	GLubyte* pixelData;
	GLuint pixelCount=0, bytesPerPixel=4;
	bool windowOpen;
	std::vector<ViewportImageParamsVk> paramCollection;
	VkBool32 currentRenderingFrame;
	VkRenderPass renderPass;
	VkSwapchainKHR swapchain;
	std::vector<VkCommandBuffer> cmdBuffers;
	std::vector<VkFramebuffer> frameBuffers;
};

