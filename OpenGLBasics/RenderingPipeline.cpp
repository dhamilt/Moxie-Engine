#include "glPCH.h"
#include "RenderingPipeline.h"
#include "GLSetup.h"
#include "Material.h"
#include "Mesh.h"
#include "Cubemaps.h"
#include "VulkanFunctionLibrary.h"
#include "VulkanPlatformInit.h"
#include <typeinfo>

extern GLSetup* GGLSPtr;



BRenderingPipeline::BRenderingPipeline()
{
	
}

BRenderingPipeline::~BRenderingPipeline()
{
	CleanupRenderingPipeline();
}

void BRenderingPipeline::CleanupRenderingPipeline()
{
#if USE_VULKAN
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
#endif
	auto device = vkSettings->device;
	auto allocationCallback = vkSettings->allocationCallback;
	// Free primitive rendering data
	for (auto it = primitives.begin(); it != primitives.end(); it++) {

#if USE_VULKAN
		auto renderData = it->second;
		/*vkDestroyBuffer(device, renderData->vertexBufParams.buffer, allocationCallback);
		vkFreeMemory(device, renderData->vertexBufferMemory, allocationCallback);

		vkDestroyBuffer(device, renderData->indexBuffer, allocationCallback);
		vkFreeMemory(device, renderData->indexBufferMemory, allocationCallback);*/
		//for (uint16_t i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; ++i)
		//{
		//	vkDestroyBuffer(device, renderData->mvpParams.buffers[i], allocationCallback);
		//	vkDestroyBuffer(device, renderData->normalParams.buffers[i], allocationCallback);
		//	vkDestroyBuffer(device, renderData->lightParams.buffers[i], allocationCallback);
		//	vkDestroyBuffer(device, renderData->viewParams.buffers[i], allocationCallback);
		//	vkDestroyBuffer(device, renderData->objParams.buffers[i], allocationCallback);

		//	vkFreeMemory(device, renderData->mvpParams.deviceMemory[i], allocationCallback);
		//	vkFreeMemory(device, renderData->normalParams.deviceMemory[i], allocationCallback);
		//	vkFreeMemory(device, renderData->lightParams.deviceMemory[i], allocationCallback);
		//	vkFreeMemory(device, renderData->viewParams.deviceMemory[i], allocationCallback);
		//	vkFreeMemory(device, renderData->objParams.deviceMemory[i], allocationCallback);
		//}


		for(uint16_t j = 0; j < (uint16_t)renderData->descriptorSetLayouts.size(); ++j)
		vkDestroyDescriptorSetLayout(vkSettings->device, renderData->descriptorSetLayouts[j], allocationCallback);
#endif
		delete it->second;
	}

#if USE_VULKAN
	// free vulkan pipeline builder
	delete vulkanPipelineBuilder;
	// free framebuffers
	for (int i = 0; i < (int)vkFramebuffers.size(); i++)
		vkDestroyFramebuffer(vkSettings->device, vkFramebuffers[i], vkSettings->allocationCallback);
	vkFramebuffers.clear();
#endif
	primitives.clear();
}

void BRenderingPipeline::Init()
{
#if USE_OPENGL
	GenerateDefaultFramebuffer();
#elif USE_VULKAN
	GenerateVkFrameBuffers();
	vulkanPipelineBuilder = new VkPipelineBuilder();
#endif

}

void BRenderingPipeline::Import(std::string primitiveName, std::vector<DVertex> _vertices, std::vector<uint16_t> _indices)
{
	// if the primitive already exists in the pipeline
	if (primitives.find(primitiveName) != primitives.end())
	{
		// update the mesh data in the render data struct
		RenderBufferData* data = primitives[primitiveName];
#if USE_VULKAN
		
		
		// if mesh data already exists and using vulkan
		// free up the previous memory buffers in order to be reallocated
		auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
		if (!data->vertices.empty())
		{
			vkDestroyBuffer(vkSettings->device, data->vertexBufParams.buffer, vkSettings->allocationCallback);
			vkFreeMemory(vkSettings->device, data->vertexBufParams.deviceMemory, vkSettings->allocationCallback);
		}
		if (!data->indices.empty())
		{
			vkDestroyBuffer(vkSettings->device, data->indexBufParams.buffer, vkSettings->allocationCallback);
			vkFreeMemory(vkSettings->device, data->indexBufParams.deviceMemory, vkSettings->allocationCallback);
		}
		// destroy uniform buffers and release memory associated
		/*for (auto uniformBufferPtr = data->vertexUniformBuffers.begin(); uniformBufferPtr != data->vertexUniformBuffers.end(); uniformBufferPtr++)
			vkDestroyBuffer(vkSettings->device, *uniformBufferPtr, vkSettings->allocationCallback);*/
		for (auto uniformBufferMemoryPtr = data->uniformBuffersMemory.begin(); uniformBufferMemoryPtr != data->uniformBuffersMemory.end(); uniformBufferMemoryPtr++)
			vkFreeMemory(vkSettings->device, *uniformBufferMemoryPtr, vkSettings->allocationCallback);
		
		// destroy descriptor layout bindings
		for(int i = 0; i < (int)data->descriptorSetLayouts.size(); ++i)
			vkDestroyDescriptorSetLayout(vkSettings->device, data->descriptorSetLayouts[i], vkSettings->allocationCallback);
#endif
		data->vertices	= _vertices;
		data->indices = _indices;
	}
	else
	{
		// Create container that holds mesh data
		RenderBufferData* renderData = new RenderBufferData();
		renderData->indices = _indices;
		renderData->vertices = _vertices;
		
		// Cache container under identifier
		primitives.insert({ primitiveName, renderData });
	}
#if USE_VULKAN
	// Send reading format (basically a header) for the vertex data to graphics pipeline
	LoadVertexReadingFormatToVkPipeline(primitiveName);

	// Create, allocate, and bind memory for vertex buffer
	CreateVkVertexBuffer(primitiveName);

	// Create, allocate, and bind memory for index buffer
	CreateVkIndexBuffer(primitiveName);

	// Create, allocate and bind memory for uniform buffers
	CreateVkUniformBuffers(primitiveName);

	// Create Descriptor Layouts for uniform buffers in shader(s)
	SetVkDescriptorForUniformBuffers(primitiveName);

	// Fill vertex buffer with data
	FillVkVertexBuffer(primitiveName);

	// Fill index buffer with data
	FillVkIndexBuffer(primitiveName);

	// Fill uniform buffers with data
	//FillVkUniformBuffers(primitiveName);
#endif

}

void BRenderingPipeline::Import(std::string primitiveName, Mesh* mesh)
{
	MeshDataParams* meshData = mesh->GetMeshData();
	Import(primitiveName, meshData->vertices, meshData->indices);
}

void BRenderingPipeline::LoadVertexReadingFormatToVkPipeline(std::string primitiveName)
{
	auto& params = primitives[primitiveName]->pipelineBuilderParams;
	// Determines format on how to read vertex buffer
	auto& vertexInputBindings = primitives[primitiveName]->inputBindingDescriptions;

	VkVertexInputBindingDescription inputBinding = {
		.binding = 0,
		.stride = sizeof(DVertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
	vertexInputBindings.push_back(inputBinding);

	// Determines the mapping of vertex buffer data
	auto &vertexInputAttributes = primitives[primitiveName]->inputAttributeDescriptions;
	VkVertexInputAttributeDescription attributes[3] = {
		{
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(DVertex, DVertex::pos)
		},

		{
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(DVertex, DVertex::texCoord)
		},

		{
			.location = 2,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(DVertex, DVertex::normal)
		}
	};
	for (VkBool32 attrIndex = 0; attrIndex < VkBool32(sizeof(attributes) / sizeof(VkVertexInputAttributeDescription)); attrIndex++)
		vertexInputAttributes.push_back(attributes[attrIndex]);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.vertexBindingDescriptionCount = (VkBool32)vertexInputBindings.size(),
		.pVertexBindingDescriptions = vertexInputBindings.data(),
		.vertexAttributeDescriptionCount = (VkBool32)vertexInputAttributes.size(),
		.pVertexAttributeDescriptions = vertexInputAttributes.data()
	};

	vulkanPipelineBuilder->BuildVertexInputState(params, vertexInputInfo);

}

void BRenderingPipeline::CreateVkVertexBuffer(std::string primitiveName)
{
	// Reserve memory for vertex buffer
	auto renderBufferData = primitives[primitiveName];
	UniformBufferParams& vertexBufParams = renderBufferData->vertexBufParams;
	
	
	// set vertex buffer size
	vertexBufParams.bufferSize = sizeof(DVertex) * renderBufferData->vertices.size();
	// Point to the data buffer should have
	vertexBufParams.data = renderBufferData->vertices.data();
	
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, vkSettings->physicalDevices[vkSettings->discreteGPUIndex], 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBufParams);

}

void BRenderingPipeline::CreateVkIndexBuffer(std::string primitiveName)
{
	// Reserve memory for vertex buffer
	auto renderBufferData = primitives[primitiveName];
	UniformBufferParams& indexBufParams = renderBufferData->indexBufParams;

	// set index buffer size
	indexBufParams.bufferSize = sizeof(uint16_t) * renderBufferData->indices.size();	
	indexBufParams.data = renderBufferData->indices.data();

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, vkSettings->physicalDevices[vkSettings->discreteGPUIndex],
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexBufParams);
}

void BRenderingPipeline::CreateVkUniformBuffers(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	auto vertexUniformBuffers = &renderData->uniformBufferParamsForShader;

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();


	UniformBufferParams* mvp = &renderData->mvpParams;
	mvp->bufferSize = sizeof(MVPBuffer);
	mvp->data = &renderData->mvpBuffer;

	UniformBufferParams* normal = &renderData->normalParams;
	normal->bufferSize = sizeof(NormalBuffer);
	normal->data = &renderData->normalBuffer;

	UniformBufferParams* lightBuf = &renderData->lightParams;
	lightBuf->bufferSize = sizeof(LightBuffer);
	lightBuf->data = &renderData->lightBuffer;

	UniformBufferParams* viewBuf = &renderData->viewParams;
	viewBuf->bufferSize = sizeof(ViewBuffer);
	viewBuf->data = &renderData->viewBuffer;

	UniformBufferParams* objectBuf = &renderData->objParams;
	objectBuf->bufferSize = sizeof(ObjectPropertyBuffer);
	objectBuf->data = &renderData->objectBuffer;

	/*for (VkBool32 i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; i++)
	{		*/
		VkPhysicalDevice gpu = vkSettings->physicalDevices[vkSettings->discreteGPUIndex];
		VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, gpu, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *mvp);

		VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, gpu, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *normal);
		
		VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, gpu,	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *lightBuf);		

		VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, gpu, 	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *viewBuf);

		VulkanFunctionLibrary::CreateVkBuffer(vkSettings->device, vkSettings->allocationCallback, gpu,	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *objectBuf);
		
	//}
}

void BRenderingPipeline::FillVkVertexBuffer(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	
	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->vertexBufParams);

	//void* data;
	//vkMapMemory(vkSettings->device, vertBufParams.deviceMemory, 0, vertBufParams.bufferSize, 0, &data);
	//memcpy(data, renderData->vertices.data(), vertBufParams.bufferSize);
	//vkUnmapMemory(vkSettings->device, vertBufParams.deviceMemory);
}

void BRenderingPipeline::FillVkIndexBuffer(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->indexBufParams);
	/*void* data;
	vkMapMemory(vkSettings->device, indexBufParams.deviceMemory, 0, indexBufParams.bufferSize, 0, &data);
	memcpy(data, renderData->indices.data(), indexBufParams.bufferSize);
	vkUnmapMemory(vkSettings->device, indexBufParams.deviceMemory);*/
}

void BRenderingPipeline::FillVkUniformBuffers(std::string primitiveName)
{
	auto renderBufferData = primitives[primitiveName];
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();

	auto device = vkSettings->device;
	auto mvp = renderBufferData->mvpParams;
	auto normal = renderBufferData->normalParams;
	auto lightBuf = renderBufferData->lightParams;
	auto viewBuf = renderBufferData->viewParams;
	auto objectBuf = renderBufferData->objParams;

	/*for (VkBool32 i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; i++)
	{*/
			// Fill MVP buffer
			vkMapMemory(device, mvp.deviceMemory, mvp.bufferSize, sizeof(MVPBuffer), 0, &mvp.data);
			memcpy(mvp.data, mvp.buffer, sizeof(MVPBuffer));
			vkUnmapMemory(device, mvp.deviceMemory);
			mvp.bufferSize = sizeof(MVPBuffer);

			// Fill Normals buffer
			vkMapMemory(device, normal.deviceMemory, normal.bufferSize, sizeof(NormalBuffer), 0, &normal.data);
			memcpy(normal.data, normal.buffer, sizeof(NormalBuffer));
			vkUnmapMemory(device, normal.deviceMemory);
			normal.bufferSize = sizeof(NormalBuffer);

			// Fill light buffer
			vkMapMemory(device, lightBuf.deviceMemory, lightBuf.bufferSize, sizeof(LightBuffer), 0, &lightBuf.data);
			memcpy(lightBuf.data, lightBuf.buffer, sizeof(LightBuffer));
			vkUnmapMemory(device, lightBuf.deviceMemory);
			lightBuf.bufferSize = sizeof(LightBuffer);

			// Fill view buffer
			vkMapMemory(device, viewBuf.deviceMemory, viewBuf.bufferSize, sizeof(ViewBuffer), 0, &viewBuf.data);
			memcpy(viewBuf.data, viewBuf.buffer, sizeof(ViewBuffer));
			vkUnmapMemory(device, viewBuf.deviceMemory);
			viewBuf.bufferSize = sizeof(ViewBuffer);

			// Fill object buffer
			vkMapMemory(device, objectBuf.deviceMemory, objectBuf.bufferSize, sizeof(ObjectPropertyBuffer), 0, &objectBuf.data);
			memcpy(objectBuf.data, objectBuf.buffer, sizeof(ObjectPropertyBuffer));
			vkUnmapMemory(device, objectBuf.deviceMemory);
			objectBuf.bufferSize = sizeof(ObjectPropertyBuffer);
	//}
}

void BRenderingPipeline::SetVkDescriptorForUniformBuffers(std::string primitiveName, std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings)
{
	auto renderData = primitives[primitiveName];
	
	renderData->descriptorLayoutBindings = descriptorLayoutBindings;


	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.bindingCount = descriptorLayoutBindings.back().binding + 1,
		.pBindings = descriptorLayoutBindings.data()
	};

	VkDescriptorSetLayout descriptorSetLayout;

	VkResult result = vkCreateDescriptorSetLayout(vkSettings->device, &descriptorSetLayoutInfo, vkSettings->allocationCallback, &descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create descriptor set layout!");
	}
	renderData->descriptorSetLayouts.push_back(descriptorSetLayout);

	VkDescriptorSet descriptorSet;

	auto descriptorSetAllocInfo = &vkSettings->descriptorInfo;
	descriptorSetAllocInfo->descriptorPool = vkSettings->descriptorPool;
	descriptorSetAllocInfo->descriptorSetCount = (VkBool32)renderData->descriptorSetLayouts.size();
	descriptorSetAllocInfo->pSetLayouts = renderData->descriptorSetLayouts.data();
	descriptorSetAllocInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	vkAllocateDescriptorSets(vkSettings->device, &vkSettings->descriptorInfo, &descriptorSet);
	renderData->descriptorSets.push_back(descriptorSet);

	// Fill each uniform buffer with its initial data for descriptor set	
	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->mvpParams);
	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->normalParams);
	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->lightParams);
	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->viewParams);
	VulkanFunctionLibrary::FillVkBuffer(vkSettings->device, renderData->objParams);



	std::vector<UniformBufferParams*> uniformBufferParams
	{
		&renderData->mvpParams,
		&renderData->normalParams,
		&renderData->lightParams,
		&renderData->viewParams,
		&renderData->objParams
	};
	std::vector<VkDescriptorBufferInfo> bufferInformationForDescriptorSet;

	// Create mapping between descriptor layout bindings and buffer info associated
	std::unordered_map<int, VkDescriptorBufferInfo> layoutBindingToBufferMapping = {
		{0, {}},
		{1, {}},
		{2, {}},
		{3, {}},
		{4, {}}
	};
	int bindingIndex = 0;
	// buffer info for shader stages and mapping
	for (const UniformBufferParams* params : uniformBufferParams)
	{
		VkDescriptorBufferInfo info =
		{
			.buffer = params->buffer,
			.offset = 0,
			.range = params->bufferSize
		};
		layoutBindingToBufferMapping[bindingIndex] = info;
		bindingIndex++;
	}


	//const UniformBufferParams** param = uniformBufferParams.data();
	//// mvp buffer info
	//VkDescriptorBufferInfo mvpBufInfo =
	//{
	//	.buffer = (*param)->buffer,
	//	.offset = 0,
	//	.range = sizeof(MVPBuffer)
	//};

	//layoutBindingToBufferMapping[bindingIndex] = mvpBufInfo;

	//++param;
	//++bindingIndex;

	//// normal buffer info
	//VkDescriptorBufferInfo normalInfo =
	//{
	//	.buffer = param->buffer,
	//	.offset = 0,
	//	.range = sizeof(NormalBuffer)
	//};
	//layoutBindingToBufferMapping[bindingIndex] = normalInfo;

	//++param;
	//++bindingIndex;

	//// light buffer info
	//VkDescriptorBufferInfo lightBufInfo =
	//{
	//	.buffer = param->buffer,
	//	.offset = 0,
	//	.range = sizeof(LightBuffer)
	//};
	//layoutBindingToBufferMapping[bindingIndex] = lightBufInfo;
	//

	//++param;
	//++bindingIndex;

	//// view property buffer info
	//VkDescriptorBufferInfo viewInfo =
	//{
	//	.buffer = param->buffer,
	//	.offset = 0,
	//	.range = sizeof(ViewBuffer)
	//};
	//layoutBindingToBufferMapping[bindingIndex] = viewInfo;

	//++param;
	//++bindingIndex;
	//// object property buffer info
	//VkDescriptorBufferInfo objBufInfo =
	//{
	//	.buffer = param->buffer,
	//	.offset = 0,
	//	.range = sizeof(ObjectPropertyBuffer)
	//};
	//layoutBindingToBufferMapping[bindingIndex] = objBufInfo
		;
	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	for (const VkDescriptorSetLayoutBinding& binding : descriptorLayoutBindings)
	{
		VkWriteDescriptorSet writeDescriptorSet =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = VK_NULL_HANDLE,
			.dstSet = renderData->descriptorSets.front(),
			.dstBinding = binding.binding,
			.dstArrayElement = 0,
			.descriptorCount = binding.descriptorCount,
			.descriptorType = binding.descriptorType,
			.pImageInfo = VK_NULL_HANDLE,
			.pBufferInfo = &layoutBindingToBufferMapping[binding.binding],
			.pTexelBufferView = VK_NULL_HANDLE
		};
		//printf("%x\n", writeDescriptorSet.pBufferInfo->buffer);
		writeDescriptorSets.push_back(writeDescriptorSet);
	}

	vkUpdateDescriptorSets(vkSettings->device, (VkBool32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);


}

void BRenderingPipeline::LoadVkShaderStages(std::string primitiveName, VkBool32 shaderStageFileCount, VkShaderStageConfigs* shaderStages)
{
	auto renderData = primitives[primitiveName];
	for (VkBool32 i = 0; i < shaderStageFileCount; i++)
	{
		auto config = shaderStages[i];
		renderData->vkShaderStageFiles.push_back(config);
		vulkanPipelineBuilder->LoadShaderModule(config, renderData->pipelineBuilderParams);
	}
}

void BRenderingPipeline::SetVkPipelineDepthState(std::string primitiveName, VkCompareOp comparisonOperation, bool isDepthBoundsEnabled, float minDepthBounds, float maxDepthBounds)
{
	auto depthStencilInfo = primitives[primitiveName]->pipelineBuilderParams.depthStencilInfo;
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = VK_TRUE;
	depthStencilInfo.depthWriteEnable = VK_TRUE;
	depthStencilInfo.depthCompareOp = comparisonOperation;
	depthStencilInfo.depthBoundsTestEnable = isDepthBoundsEnabled;
	if (isDepthBoundsEnabled)
	{
		depthStencilInfo.minDepthBounds = minDepthBounds;
		depthStencilInfo.maxDepthBounds = maxDepthBounds;
	}
	vulkanPipelineBuilder->LoadDepthStencilState(primitives[primitiveName]->pipelineBuilderParams, depthStencilInfo);
}

void BRenderingPipeline::SetVkPipelineStencilState(std::string primitiveName, VkStencilOpState frontStencilState, VkStencilOpState backStencilState)
{
	auto depthStencilInfo = primitives[primitiveName]->pipelineBuilderParams.depthStencilInfo;
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.stencilTestEnable = VK_TRUE;
	depthStencilInfo.front = frontStencilState;
	depthStencilInfo.back = backStencilState;
	vulkanPipelineBuilder->LoadDepthStencilState(primitives[primitiveName]->pipelineBuilderParams, depthStencilInfo);
}

void BRenderingPipeline::CreatePipelineLayout(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	//for (VkBool32 descriptorSetIndex = 0; descriptorSetIndex < (VkBool32)renderData->descriptorSetLayouts.size(); descriptorSetIndex++) {
	//	
	//	
	//}
	VkPipelineLayoutCreateInfo layoutInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = (VkBool32)renderData->descriptorSetLayouts.size(), /** TODO make this dynamic for non - static layout bindings*/
			.pSetLayouts = renderData->descriptorSetLayouts.data(),
			.pushConstantRangeCount = (VkBool32)renderData->pipelineBuilderParams.pushConstants.size(),
			.pPushConstantRanges = renderData->pipelineBuilderParams.pushConstants.data()
	};

	renderData->pipelineBuilderParams.layoutInfo.push_back(layoutInfo);
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	size_t pipelineLayoutCount = renderData->pipelineBuilderParams.layoutInfo.size();
	renderData->pipelineBuilderParams.pipelineLayouts.resize(pipelineLayoutCount);
	//Create and load pipeline layout to graphics pipeline
	/*for (VkBool32 pipelineLayoutIndex = 0; pipelineLayoutIndex < (VkBool32)pipelineLayoutCount; pipelineLayoutIndex++) {
		
	}*/

	VkResult result = vkCreatePipelineLayout(vkSettings->device, &renderData->pipelineBuilderParams.layoutInfo[0], vkSettings->allocationCallback, &renderData->pipelineBuilderParams.pipelineLayouts[0]);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create pipeline layout from descriptor set layout(s)!");
	}
	vulkanPipelineBuilder->LoadPipelineLayout(renderData->pipelineBuilderParams, renderData->pipelineBuilderParams.pipelineLayouts);
}

void BRenderingPipeline::GenerateCubemap(std::vector<TextureData*> cubemapTextureData)
{
	// ensure that all sides of the cubemap have texture data
	assert((int)cubemapTextureData.size() == 6);
	// create cubemap
	if (!cubemapParams)
		cubemapParams = &defaultSkyBox;
#if USE_OPENGL
	glGenTextures(1, &cubemapParams->cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapParams->cubemapID);
	int i = 0;
	for (auto it = cubemapTextureData.begin(); it != cubemapTextureData.end(); it++)
	{
		// pass in correlating texture for each face of cubemap
		TextureData* cubemapTexture = *it;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, cubemapTexture->width, cubemapTexture->height,
					0, GL_RGB, GL_UNSIGNED_BYTE, cubemapTexture->data);
		i++;
	}

	// Cubemap texture params
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// create the default cubemap shader
	cubemapShader = new Shader("CubeMap.vertex", "CubeMap.fragment");
#elif USE_VULKAN

#endif
	// Create vertex buffers and pass in data
	RequestForDefaultSkyboxVerts();
}

void BRenderingPipeline::RequestForMeshVertexData(std::string primitiveName)
{
	// if primitive has not yet been imported
	if(primitives.find(primitiveName) == primitives.end())
		std::runtime_error("Error! Primitive named " + primitiveName + " has not yet been imported!");

	RenderBufferData* renderData = primitives[primitiveName];
	// create buffers for the vertex array object, vertex buffer object,
	// and element buffer object
	
	
	
	// Bind the buffers and pass in the appropriate data for the following:
	
	// the vertex array object
	if (renderData->vao == 0)
		glGenVertexArrays(1, &renderData->vao);

	// the vertex buffer object
	if (renderData->vbo == 0)
		glGenBuffers(1, &renderData->vbo);

	// the index buffer
	if (renderData->ebo == 0)
		glGenBuffers(1, &renderData->ebo);
	glBindVertexArray(renderData->vao);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, renderData->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DVertex) * renderData->vertices.size(), &renderData->vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderData->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * renderData->indices.size(), &renderData->indices[0], GL_STATIC_DRAW);

	// Add vertex positions for model	
	glVertexAttribPointer(
		0, // start index
		3, // number of dimensions
		GL_FLOAT, // vertex buffer type
		GL_FALSE, // normalized?
		sizeof(DVertex), // stride
		(void*)offsetof(DVertex, pos) // buffer offset
	);
	glEnableVertexAttribArray(0);

	// Add vertex texture coordinates for model		
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(DVertex),
		(void*)offsetof(DVertex, texCoord)
	);
	glEnableVertexAttribArray(1);

	// Add vertex normals for model		
	glVertexAttribPointer(
		2, // layout index
		3, // number of dimensions
		GL_FLOAT, // value type for dimensions
		GL_FALSE, // normalized?
		sizeof(DVertex), // stride
		(void*)offsetof(DVertex, normal)// buffer offset
	);
	glEnableVertexAttribArray(2);
		
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BRenderingPipeline::RequestForDefaultSkyboxVerts()
{
	assert(cubemapParams);
	// Generate skybox vert buffer from default verts	
	if(cubemapParams->vao == 0)
		glGenVertexArrays(1, &cubemapParams->vao);
	if(cubemapParams->vbo == 0)
		glGenBuffers(1, &cubemapParams->vbo);
	if(cubemapParams->ebo == 0)
		glGenBuffers(1, &cubemapParams->ebo);

	// Feed vert data to renderer
	glBindVertexArray(cubemapParams->vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubemapParams->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DVector3) * cubemapParams->vertPositions.size(),
				&cubemapParams->vertPositions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubemapParams->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * cubemapParams->indices.size(), &cubemapParams->indices[0], GL_STATIC_DRAW);
	
	// Pass vertex data to GPU for shaders	
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(DVector3),
		(void*)0
	);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BRenderingPipeline::SetOrthoView(bool isOrtho)
{
	perspectiveView = !isOrtho;
}

void BRenderingPipeline::UpdateModelMatrix(DMat4x4 _transform, std::string primitiveName)
{
	if (primitives.find(primitiveName) == primitives.end())
		return;
	primitives[primitiveName]->transform = _transform;
}

void BRenderingPipeline::UpdateProjectionMatrix(float fieldOfView, float width, float height, float nearClippingPlane, float farClippingPlane)
{
	if (perspectiveView)
		projectionMatrix = glm::perspective(glm::radians(fieldOfView), width / height, nearClippingPlane, farClippingPlane);
	else
		projectionMatrix = glm::ortho(0.0f, width, height, 0.0f, nearClippingPlane, farClippingPlane);
}

void BRenderingPipeline::UpdateViewMatrix(DMat4x4 _view)
{
	viewMatrix = _view;
}

void BRenderingPipeline::UpdateTransformMatrix(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	renderData->mvpBuffer = {
		.model = renderData->transform,
		.view = viewMatrix,
		.projection = projectionMatrix,
	};
}

void BRenderingPipeline::CreateDefaultShader()
{
#if USE_OPENGL
	// create default mesh shader for cache
	std::shared_ptr<Shader> cachedShader(new Shader("DefaultMat.vertex", "DefaultMat.fragment"));
	shaderCache.push_back(cachedShader);
#elif USE_VULKAN
	
#endif
}

void BRenderingPipeline::LoadShader(std::string primitiveName, Material* mat)
{
	RenderBufferData* renderData;
	// does the primitive exist
	auto searchPName = primitives.begin();
	while (searchPName != primitives.end())
	{
		if (searchPName->first == primitiveName)
			break;
		searchPName++;
	}
	// if the primitive hasn't been added to the pipeline
	if (searchPName == primitives.end())
		renderData = new RenderBufferData();
	else
		renderData = primitives[primitiveName];

	// is shader in cache
	Shader shader = *mat->Get();
	auto searchShader = shaderCache.begin();	
	
	while (searchShader != shaderCache.end())
	{
		std::weak_ptr<Shader>weakShader(*searchShader);		
		Shader* cachedShader = weakShader.lock().get();
		if (shader == *cachedShader)
			break;
		searchShader++;		
	}
	//assert(searchShader != shaderCache.end());
	if (searchShader == shaderCache.end())
		shaderCache.push_back(std::make_shared<Shader>(shader));
	
	renderData->shader = *searchShader;
	renderData->color = mat->color;
}

void BRenderingPipeline::GetDefaultShader(Shader* defaultShader)
{
	*defaultShader = *shaderCache[0].get();
	
}

void BRenderingPipeline::LoadDefaultCubemapShader()
{
	if (cubemapShader)
		cubemapShader->Use();
}

void BRenderingPipeline::UpdateShaderCache(Shader* shaderToAdd)
{
	Shader temp = *shaderToAdd;
	auto searchPtr = shaderCache.begin();
	// if shader already exists in cache
	// exit function
	while (searchPtr != shaderCache.end())
	{
		std::weak_ptr<Shader> weakShader(*searchPtr);
		Shader* current = weakShader.lock().get();
		if (*current == *shaderToAdd)
			return;
		searchPtr++;
	}
	// add shader to cache
	if(searchPtr == shaderCache.end())
		shaderCache.push_back(std::make_shared<Shader>(*shaderToAdd));
}

void BRenderingPipeline::UpdateShaderCache(Material* shaderToAdd)
{
	UpdateShaderCache(shaderToAdd->Get());
}

void BRenderingPipeline::UpdateDefaultCubemapShader()
{
	if (cubemapShader)
	{
		// Converting view matrix from DMat4x4 to mat3 then back to DMat4x4
		// again in order to remove the translation
		cubemapShader->SetMat4("view", DMat4x4(DMat3x3(viewMatrix)));
		cubemapShader->SetMat4("projection", projectionMatrix);
	}
}

bool BRenderingPipeline::RequestForLight(Light& light)
{
	if ((int)lightCache.size() < MAX_LIGHT_COUNT)
	{
		return false;
	}

	Light* lightPtr = new Light();
	lightCache.push_back(lightPtr);
	light = *lightPtr;
	return true;
}

void BRenderingPipeline::UpdateLightDataForShader(RenderBufferData* renderData)
{

	if (renderData)
	{
		Shader* shader;
		if (!renderData->shader)
			renderData->shader = shaderCache[0];
		
		shader = renderData->shader.get();
		DMat4x4 model = renderData->transform;
		/*DMat4x4 mvp = projectionMatrix * viewMatrix * model;
		DMat4x4 mv = viewMatrix * model;*/
		renderData->color = Color::Blue;
		DMat3x3 normalMatrix = DMat3x3(glm::transpose(glm::inverse(model)));
		DVector3 eyeDir = GGLSPtr->mainCamera->transform.GetForwardVector();
		shader->SetMat4("model", model);
		shader->SetMat4("view", viewMatrix);
		shader->SetMat4("projection", projectionMatrix);
		shader->SetFloat4("objectColor", renderData->color.rgba());
		shader->SetMat3("normalMatrix", normalMatrix);
		shader->SetInt("lightCount", (int)lightCache.size());
		for (int i = 0; i < (int)lightCache.size(); i++)
		{
			Light* light = lightCache[i];
			std::string attributeName;
			std::string index = "[" + std::to_string(i) + "]";

			attributeName = "Lights" + index + ".isEnabled";
			shader->SetBool(attributeName.c_str(), light->active);

			attributeName = "Lights" + index + ".isLocal";
			shader->SetBool(attributeName.c_str(), light->isLocal);

			attributeName = "Lights" + index + ".isSpot";
			shader->SetBool(attributeName.c_str(), light->isSpot);

			attributeName = "Lights" + index + ".ambient";
			shader->SetFloat3(attributeName.c_str(), light->ambient);

			attributeName = "Lights" + index + ".color";
			shader->SetFloat3(attributeName.c_str(), light->lightColor.rgb());

			attributeName = "Lights" + index + ".position";
			shader->SetFloat3(attributeName.c_str(), light->position);

			attributeName = "Lights" + index + ".halfVector";
			shader->SetFloat3(attributeName.c_str(), light->halfVector);

			attributeName = "Lights" + index + ".coneDir";
			shader->SetFloat3(attributeName.c_str(), light->direction);

			attributeName = "Lights" + index + ".strength";
			shader->SetFloat(attributeName.c_str(), light->lightIntensity);

			attributeName = "Lights" + index + ".spotCosineCutoff";
			shader->SetFloat(attributeName.c_str(), light->spotCosineCutoff);

			attributeName = "Lights" + index + ".spotExponent";
			shader->SetFloat(attributeName.c_str(), light->spotExponent);

			attributeName = "Lights" + index + ".constantAttenuation";
			shader->SetFloat(attributeName.c_str(), light->constant);

			attributeName = "Lights" + index + ".linearAttenuation";
			shader->SetFloat(attributeName.c_str(), light->linear);

			attributeName = "Lights" + index + ".quadraticAttenuation";
			shader->SetFloat(attributeName.c_str(), light->quadratic);
		}
	}
}

void BRenderingPipeline::GenerateDefaultFramebuffer()
{	
	// create framebuffer
	glGenFramebuffers(1, &defaultFramebuffer);
	// Attach texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	// create texture to attach
	glGenTextures(1, &defaultFrameBufferTextureID);
	// Texture params
	glBindTexture(GL_TEXTURE_2D, defaultFrameBufferTextureID);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenResolution.width, screenResolution.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, defaultFrameBufferTextureID, 0);
	// create renderbuffer
	glGenRenderbuffers(1, &renderbuffer);
	// As well as the renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenResolution.width, screenResolution.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
	Moxie::FrameBufferErrorCheck();
	

	// Remove bindings
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Load the default framebuffer
	currentFramebuffer = defaultFramebuffer;

}

void BRenderingPipeline::GenerateVkFrameBuffers()
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.width = screenResolution.width;
	framebufferInfo.height = screenResolution.height;
	// if being recalled due to resizing
	if (vkFramebuffers.size() > 0)
	{
		// destroy existing framebuffers
		for (int i = 0; i < (int)vkFramebuffers.size(); i++)
			vkDestroyFramebuffer(vkSettings->device, vkFramebuffers[i], vkSettings->allocationCallback);
		vkFramebuffers.clear();
	}
	
	// Set the attachment count to be same as the number of image buffers allocated for in the swapchain
	framebufferInfo.attachmentCount = 2;
	for (uint32_t i = 0; i < vkSettings->swapchainImageCount; i++)
	{
		std::vector<VkImageView> attachments;
		attachments.push_back(vkSettings->imageBuffers[i].imageView);
		attachments.push_back(vkSettings->depthBuffer.imageView);
		framebufferInfo.pAttachments = &attachments[0];
		framebufferInfo.pNext = VK_NULL_HANDLE;
		framebufferInfo.renderPass = vkSettings->renderPass;
		framebufferInfo.layers = 1;
		framebufferInfo.flags = 0;
		VkFramebuffer vkFramebuffer;
		auto result = vkCreateFramebuffer(vkSettings->device, &framebufferInfo,
			vkSettings->allocationCallback, &vkFramebuffer);
		assert(result == VK_SUCCESS);
		vkFramebuffers.push_back(vkFramebuffer);
	}
}

void BRenderingPipeline::SetViewportInfo(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	vulkanPipelineBuilder->LoadViewportInfo(renderData->pipelineBuilderParams, screenResolution);
}

void BRenderingPipeline::DrawVk(VkCommandBuffer cmdBuffer)
{
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	auto it = primitives.begin();
	for (;it != primitives.end(); it++)
	{
		vertexBuffers.push_back(it->second->vertexBufParams.buffer);
		offsets.push_back(it->second->vertexBufParams.bufferSize);
	}

	vkCmdBindVertexBuffers(cmdBuffer, 0, (VkBool32)vertexBuffers.size(),
		vertexBuffers.data(), offsets.data());

	for (it = primitives.begin(); it != primitives.end(); it++)
	{
		auto renderData = it->second;
		vkCmdDraw(cmdBuffer, (VkBool32)renderData->vertices.size(), 1, 0, 0);
	}
	
}

void BRenderingPipeline::DrawVkIndexed(VkCommandBuffer cmdBuffer)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	for (auto it = primitives.begin(); it != primitives.end(); it++)
	{
		auto renderData = it->second;
		// update mvp for model
		UpdateTransformMatrix(it->first);
		// TODO: Use Pipeline builder to dynamically create graphics pipeline for mesh
		if(renderData->graphicsPipeline == NULL)
			vulkanPipelineBuilder->CreateMeshShaderPipeline(&renderData->graphicsPipeline, renderData->pipelineBuilderParams);
		// TODO: bind the specified descriptor sets that have the mesh's rendering data for the shaders

		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
			renderData->pipelineBuilderParams.pipelineLayouts[0], 0,
			(VkBool32)renderData->descriptorSets.size(), renderData->descriptorSets.data(), 0, 0);
		// tell vulkan to use the graphics pipeline attached to current primitive
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderData->graphicsPipeline);

		vkCmdBindVertexBuffers(cmdBuffer, 0, 1,
			&renderData->vertexBufParams.buffer, &renderData->vertexBufParams.bufferSize);

		vkCmdBindIndexBuffer(cmdBuffer, renderData->indexBufParams.buffer, 0,
			VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(cmdBuffer, (VkBool32)renderData->indices.size(),
			1, 0, 0, 0);
	}
}

void BRenderingPipeline::GetVkFramebuffer(VkFramebuffer& framebuf, VkBool32 frameBufIndex)
{
	assert(frameBufIndex < (VkBool32)vkFramebuffers.size());
	framebuf = vkFramebuffers[frameBufIndex];
}

void BRenderingPipeline::ResizeScreen(int width, int height)
{
	screenResolution.width = width;
	screenResolution.height = height;
	// Resize the framebuffer capture to new resolution
	GenerateVkFrameBuffers();
	// Update the viewport info for the graphics pipeline(s)
	for (auto primitivePtr = primitives.begin(); primitivePtr != primitives.end(); primitivePtr++)
		vulkanPipelineBuilder->LoadViewportInfo(primitivePtr->second->pipelineBuilderParams, screenResolution);
}

void BRenderingPipeline::LoadGLFramebuffer(GLuint fbID)
{
	
	// if the framebuffer doesn't exist
	auto fbIdPtr = glFramebuffers.begin();
	while (fbIdPtr != glFramebuffers.end())
	{
		if (*fbIdPtr == fbID)
		{
			currentFramebuffer = fbID;
			return;
		}
		fbIdPtr++;
	}
	// throw an exception if the framebuffer does not exist
	throw std::runtime_error("Error! Framebuffer does not exist!");
}

void BRenderingPipeline::LoadCurrentFramebuffer()
{
	assert(currentFramebuffer != 0);

	glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);
}

void BRenderingPipeline::UnloadFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BRenderingPipeline::RenderPrimitives()
{
	for (auto it = primitives.begin(); it != primitives.end(); it++)
	{
		RenderBufferData* primitive = it->second;
		UpdateLightDataForShader(primitive);
		DrawMesh(primitive);
	}
}

void BRenderingPipeline::DrawMesh(RenderBufferData* renderData)
{	
	// Load the shader from the cache
	Shader* shader = renderData->shader.get();
	assert(shader);
	shader->Use();
	// Load texture(s)
	// TODO
	// Use vertex data for current primitive
	// Draw wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(renderData->vao);	
	
	glDrawElements(GL_TRIANGLES, (uint16_t)renderData->indices.size(), GL_UNSIGNED_SHORT, 0);
	
	glBindVertexArray(0);	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BRenderingPipeline::DrawCubeMap()
{	
	glDepthMask(GL_FALSE);
	if (cubemapShader)
	{
		UpdateDefaultCubemapShader();

		cubemapShader->Use();
	}
	glBindVertexArray(cubemapParams->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubemapParams->ebo);
	glBindBuffer(GL_ARRAY_BUFFER, cubemapParams->vbo);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapParams->cubemapID);
	glDrawElements(GL_TRIANGLES, (int)cubemapParams->indices.size(), GL_UNSIGNED_SHORT, 0);
	/*glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);*/
	glDepthMask(GL_TRUE);

}

RenderBufferData::~RenderBufferData()
{
#if USE_OPENGL
	// remove ownership to Shader
	if(shader)
		shader.reset();

	// remove glsl hooks
	glDisableVertexAttribArray(0 | 1 | 2);

	// free allocation to all buffer objects
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
#elif USE_VULKAN

#endif

}
