#pragma once
#include "3DTypeDefs.h"
#include <vulkan\vulkan.h>
#include <vector>
#include <string>
#include "VulkanPlatformInit.h"
#include <sstream>
#include <fstream>

const uint32_t MAX_DESCRIPTOR_POOL_SIZE = 1024;

struct VkShaderParams
{
	VkBool32 bindingIndex;
	void* data;
	size_t dataSize;
	VkShaderStageFlags shaderUsageFlags;
};

struct VkShaderStageConfigs
{
	std::string shaderFile;
	VkShaderStageFlags shaderFlag;
};

class VkShaderUtil
{
public:
	void LoadShaderStage(const VkShaderStageConfigs& shaderConfig);
	void LoadShaderStages(const std::vector<VkShaderStageConfigs>& shaderConfigs);
	static bool LoadShaderModule(const VkShaderStageConfigs shaderConfig, VkShaderModule& shaderModule);

	VkShaderUtil() {};
	VkShaderUtil(VkShaderStageConfigs shaderConfig) { LoadShaderStage(shaderConfig); }
	VkShaderUtil(std::vector<VkShaderStageConfigs> shaderConfigs) { LoadShaderStages(shaderConfigs); }
	
	// uniform utility functions
	void SetParams(std::vector<VkShaderParams> uniformParams);
	void SetBool(const VkBool32& bindingIndex, const bool& val, VkShaderStageFlags shaderStage);
	void SetFloat(const VkBool32& bindingIndex, const float& val, VkShaderStageFlags shaderStage);
	void SetInt(const VkBool32& bindingIndex, const int& val, VkShaderStageFlags shaderStage);
	void SetMat3(const VkBool32& bindingIndex, const DMat3x3& val, VkShaderStageFlags shaderStage);
	void SetMat4(const VkBool32& bindingIndex, const DMat4x4& val, VkShaderStageFlags shaderStage);
	void SetFloat4(const VkBool32& bindingIndex, const DVector4& val, VkShaderStageFlags shaderStage);
	void SetFloat4(const VkBool32& bindingIndex, const float& x, const float& y, const float& z, const float& w, VkShaderStageFlags shaderStage);
	void SetFloat3(const VkBool32& bindingIndex, const DVector3& val, VkShaderStageFlags shaderStage);
	void SetFloat3(const VkBool32& bindingIndex, const float& x, const float& y, const float& z, VkShaderStageFlags shaderStage);
	void SetFloat2(const VkBool32& bindingIndex, const DVector2& val, VkShaderStageFlags shaderStage);
	void SetFloat2(const VkBool32& bindingIndex, const float& x, const float& y, VkShaderStageFlags shaderStage);
	//void SetTexture()
	void BindParams();


private:
	uint32_t bindingCount = 0;
	VkShaderStageFlags shaderStagingMask;
	std::vector<std::string> shaderFiles;
	std::vector<VkShaderParams> bindings;
};

inline void VkShaderUtil::LoadShaderStage(const VkShaderStageConfigs& shaderConfig)
{
	if(~(shaderConfig.shaderFlag & shaderStagingMask))
	shaderStagingMask |= shaderConfig.shaderFlag;
	shaderFiles.push_back(shaderConfig.shaderFile);
}

inline void VkShaderUtil::LoadShaderStages(const std::vector<VkShaderStageConfigs>& shaderConfigs)
{
	for (auto config : shaderConfigs)
		LoadShaderStage(config);
}

inline bool VkShaderUtil::LoadShaderModule(const VkShaderStageConfigs shaderConfig, VkShaderModule& shaderModule)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	std::vector<char> fileBuf;
	std::ifstream file (shaderConfig.shaderFile, std::ios::ate | std::ifstream::binary);
	size_t fileSize = 0;
	// pass shader file data into buffer
	if (file)
	{
		fileSize = file.tellg();
		fileBuf.resize(fileSize);
		file.seekg(0);
		file.read(fileBuf.data(), fileSize);

		file.close();
	}
	else
		return false;
	auto code = reinterpret_cast<const uint32_t*>(fileBuf.data());
	// feed it into shader module
	VkShaderModuleCreateInfo shaderModuleInfo = {};
	shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleInfo.pNext = VK_NULL_HANDLE;
	shaderModuleInfo.pCode = code;
	shaderModuleInfo.codeSize = fileSize;
	shaderModuleInfo.flags = 0;
	VkResult result = vkCreateShaderModule(vkSettings->device, &shaderModuleInfo, vkSettings->allocationCallback, &shaderModule);
	if (result != VK_SUCCESS)
		return false;


	return true;
		

}

/// <summary>
/// Insert a list of layout bindings
/// </summary>
/// <param name="uniformParams"></param>
inline void VkShaderUtil::SetParams(std::vector<VkShaderParams> uniformParams)
{
	bindingCount += (uint32_t)uniformParams.size();
	for (auto param : uniformParams)
		bindings.push_back(param);
}

/// <summary>
/// Insert boolean value to layout bindings
/// </summary>
/// <param name="val">boolean</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetBool(const VkBool32& bindingIndex, const bool& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(bool);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert floating point value to layout bindings
/// </summary>
/// <param name="val">float</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat(const VkBool32& bindingIndex, const float& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(float);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert integer value to layout bindings
/// </summary>
/// <param name="val">integer</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetInt(const VkBool32& bindingIndex, const int& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(int);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert matrix 3x3 value to layout bindings
/// </summary>
/// <param name="val">Matrix 3x3</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetMat3(const VkBool32& bindingIndex, const DMat3x3& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStage & shaderStagingMask);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DMat3x3);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert matrix 4x4 value to layout bindings
/// </summary>
/// <param name="val">Matrix 4x4</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetMat4(const VkBool32& bindingIndex, const DMat4x4& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DMat4x4);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert Vector2 value to layout bindings
/// </summary>
/// <param name="val">DVector2</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat2(const VkBool32& bindingIndex, const DVector2& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DVector2);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert Vector2 value to layout bindings
/// </summary>
/// <param name="x">float</param>
/// <param name="y">float</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat2(const VkBool32& bindingIndex, const float& x, const float& y, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DVector2);
	param.shaderUsageFlags = shaderStage;
	DVector2 val(x, y);
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert Vector3 value to layout bindings
/// </summary>
/// <param name="x">float</param>
/// <param name="y">float</param>
/// <param name="z">float</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat3(const VkBool32& bindingIndex, const float& x, const float& y, const float& z, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DVector3);
	param.shaderUsageFlags = shaderStage;
	DVector3 val(x, y, z);
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert Vector3 value to layout bindings
/// </summary>
/// <param name="val">DVector3</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat3(const VkBool32& bindingIndex, const DVector3& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DVector3);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert Vector4 value to layout bindings
/// </summary>
/// <param name="val">DVector4</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat4(const VkBool32& bindingIndex, const DVector4& val, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DVector4);
	param.shaderUsageFlags = shaderStage;
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

/// <summary>
/// Insert Vector4 value to layout bindings
/// </summary>
/// <param name="x">float</param>
/// <param name="y">float</param>
/// <param name="z">float</param>
/// <param name="w">float</param>
/// <param name="shaderStage">VkShaderStageFlags</param>
inline void VkShaderUtil::SetFloat4(const VkBool32& bindingIndex, const float& x, const float& y, const float& z, const float& w, VkShaderStageFlags shaderStage)
{
	assert(shaderStagingMask & shaderStage);
	VkShaderParams param = {};
	param.bindingIndex = bindingIndex;
	param.dataSize = sizeof(DVector4);
	param.shaderUsageFlags = shaderStage;
	DVector4 val(x, y, z, w);
	memcpy(&param.data, &val, param.dataSize);
	bindings.push_back(param);
	bindingCount++;

}

// create layout bindings for each of the parameters provided
// for all of the intended shader stage(s)
inline void VkShaderUtil::BindParams()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result;
	std::vector<VkDescriptorSetLayoutBinding> bindingDescriptors;
	// For each binding
	for (auto& binding : bindings)
	{
		// create a uniform buffer for it
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = 0;
		bufferInfo.size = binding.dataSize;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.pQueueFamilyIndices = NULL;
		bufferInfo.queueFamilyIndexCount = 0;
		bufferInfo.size = binding.dataSize;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.flags = 0;
		VkBuffer buffer;
		result = vkCreateBuffer(vkSettings->device, &bufferInfo, vkSettings->allocationCallback, &buffer);
		assert(result == VK_SUCCESS);

		// Allocate memory for the buffer
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(vkSettings->device, buffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryInfo = {};
		memoryInfo.allocationSize = memoryRequirements.size;
		memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryInfo.pNext = 0;
		memoryInfo.memoryTypeIndex = 0;

		VkDeviceMemory memoryAllocated;
		result = vkAllocateMemory(vkSettings->device, &memoryInfo, vkSettings->allocationCallback, &memoryAllocated);
		assert(result == VK_SUCCESS);

		// Map the allocated memory to a variable
		void* dataMapping;
		result = vkMapMemory(vkSettings->device, memoryAllocated, 0, memoryRequirements.size, 0, &dataMapping);
		assert(result == VK_SUCCESS);
		// Copy binding data to memory
		memcpy(dataMapping, binding.data, binding.dataSize);
		// Remove mapping
		vkUnmapMemory(vkSettings->device, memoryAllocated);

		// Bind memory to buffer
		vkBindBufferMemory(vkSettings->device, buffer, memoryAllocated, 0);

		// Create a shader descriptor for the binding
		VkDescriptorSetLayoutBinding bindingDescriptor = {};
		bindingDescriptor.binding = binding.bindingIndex;
		bindingDescriptor.descriptorCount = 1;
		bindingDescriptor.stageFlags = binding.shaderUsageFlags;
		// TODO create implementation that handles sampler inputs
		//bindingDescriptor.pImmutableSamplers = ;
		bindingDescriptors.push_back(bindingDescriptor);

	}

	// Create layout for bound shader descriptors
	VkDescriptorSetLayoutCreateInfo bindingDescriptorInfo = {};
	bindingDescriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	bindingDescriptorInfo.pNext = 0;
	bindingDescriptorInfo.pBindings = &bindingDescriptors[0];
	bindingDescriptorInfo.bindingCount = bindingCount;
	bindingDescriptorInfo.flags = 0;

	VkDescriptorSetLayout shaderDescriptorLayout;
	result = vkCreateDescriptorSetLayout(vkSettings->device, &bindingDescriptorInfo, vkSettings->allocationCallback, &shaderDescriptorLayout);
	assert(result == VK_SUCCESS);

	// Create a pipeline layout from shader layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = 0;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = VK_NULL_HANDLE;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pSetLayouts = &shaderDescriptorLayout;

	VkPipelineLayout pipelineLayout;
	result = vkCreatePipelineLayout(vkSettings->device, &pipelineLayoutInfo, vkSettings->allocationCallback, &pipelineLayout);
	assert(result == VK_SUCCESS);

	// Create a Descriptor Pool for the uniform buffer(s)
	// allocated for the shader
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
	descriptorPoolSizes = std::vector<VkDescriptorPoolSize>(bindingCount);
	for (auto it = descriptorPoolSizes.begin(); it != descriptorPoolSizes.end(); it++)
	{
		it->descriptorCount = 1;
		it->type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

	VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.pNext = VK_NULL_HANDLE;
	descriptorPoolInfo.flags = 0;
	descriptorPoolInfo.poolSizeCount = bindingCount;
	descriptorPoolInfo.pPoolSizes = &descriptorPoolSizes[0];
	descriptorPoolInfo.maxSets = MAX_DESCRIPTOR_POOL_SIZE;

	VkDescriptorPool descriptorPool;
	result = vkCreateDescriptorPool(vkSettings->device, &descriptorPoolInfo, vkSettings->allocationCallback, &descriptorPool);
	assert(result == VK_SUCCESS);

	// Allocate the pool with Descriptor sets
	VkDescriptorSetAllocateInfo descriptorAllocationInfo = {};
	descriptorAllocationInfo.descriptorPool = descriptorPool;
	descriptorAllocationInfo.descriptorSetCount = bindingCount;
	descriptorAllocationInfo.pNext = VK_NULL_HANDLE;
	descriptorAllocationInfo.pSetLayouts = &shaderDescriptorLayout;
	descriptorAllocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	VkDescriptorSet descriptorSet;
	vkAllocateDescriptorSets(vkSettings->device, &descriptorAllocationInfo, &descriptorSet);

}