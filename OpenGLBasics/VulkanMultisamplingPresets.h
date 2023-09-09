#pragma once
#include "VulkanPipelineSetup.h"
// Director (Builder) class for VkMultisamplingInfo data object
class VkMultisamplingPresets
{
public:
	static VkMultisamplingInfo DefaultSingleSamplingSetup();
};


inline VkMultisamplingInfo VkMultisamplingPresets::DefaultSingleSamplingSetup()
{
	return VkMultisamplingInfo();
}
