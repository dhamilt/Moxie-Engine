#pragma once
#include <vulkan/vulkan.h>
class PVulkanRendering
{
public:

	void CleanUp();
	static PVulkanRendering* Get();
	
protected:
	PVulkanRendering();
	
};

static PVulkanRendering* instance;