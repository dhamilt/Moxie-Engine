#pragma once
#include "VulkanPipelineSetup.h"

// Director (Builder) class for VkRasterizationInfo data object
class VkRasterizationPresets
{
public:
	static VkRasterizationInfo Default2DImageRasterization();
};


inline VkRasterizationInfo VkRasterizationPresets::Default2DImageRasterization()
{
	return VkRasterizationInfo();
}
