#pragma once
#include "VulkanPlatformInit.h"
class VulkanRenderpassBuilder
{
public:
	void CreateSurface(SDL_Window* window);
	void RetrieveSwapchainImages();
	void CreateFramebuffers();
	void CreateCommandBuffers();
	void CreateRenderpass();
	void SetFramesInFlight(VkBool32 count);
	void SetResolution(VkExtent2D resolution);
	bool IsValid();
	VkRenderPass GetRenderpassHandle();
	VkFramebuffer GetFramebufferHandle(VkBool32 frameIndex);
	VkCommandBuffer GetCommandBufferHandle(VkBool32 frameIndex);


private:
	VkBool32 framesInFlight = 0;
	std::vector<VkFramebuffer> framebuffers;
	std::vector< VkCommandBuffer> commandbuffers;
	std::vector<PVkImageBuffer> swapchainImgParams;
	std::vector<VkImageView> framebufferAttachments;
	std::vector<VkImage> swapchainImgs;
	VkBool32 currentFrameIndex = -1;
	VkExtent2D resolution;
	PVkImageBuffer depthBufferParams;
	VkSwapchainKHR swapchain;
	VkRenderPass renderPass;
	VkSurfaceKHR surface;
};

