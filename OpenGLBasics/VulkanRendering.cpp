#include "glPCH.h"
#include "VulkanRendering.h"


PVulkanRendering* PVulkanRendering::Get()
{
	if (!instance)
		instance = new PVulkanRendering();

	return instance;

}

PVulkanRendering::PVulkanRendering()
{
}

