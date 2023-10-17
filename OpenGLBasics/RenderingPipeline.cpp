#include "glPCH.h"
#include "RenderingPipeline.h"
#include "GLSetup.h"
#include "Material.h"
#include "Mesh.h"
#include "Cubemaps.h"

extern GLSetup* GGLSPtr;
const int BRenderingPipeline::MAX_LIGHT_COUNT = 10;
BRenderingPipeline::BRenderingPipeline()
{
	
}

void BRenderingPipeline::CleanupRenderingPipeline()
{
#if USE_VULKAN
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
#endif
	// Free primitive rendering data
	for (auto it = primitives.begin(); it != primitives.end(); it++) {

#if USE_VULKAN
		auto renderData = it->second;
		vkDestroyBuffer(vkSettings->device, renderData->vertexBuffer, vkSettings->allocationCallback);
		vkFreeMemory(vkSettings->device, renderData->vertexBufferMemory, vkSettings->allocationCallback);

		vkDestroyBuffer(vkSettings->device, renderData->indexBuffer, vkSettings->allocationCallback);
		vkFreeMemory(vkSettings->device, renderData->indexBufferMemory, vkSettings->allocationCallback);
		for(auto descriptorSetPtr = renderData->descriptorSetLayouts.begin(); descriptorSetPtr != renderData->descriptorSetLayouts.end(); descriptorSetPtr++)
			vkDestroyDescriptorSetLayout(vkSettings->device, *descriptorSetPtr, vkSettings->allocationCallback);
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
			vkDestroyBuffer(vkSettings->device, data->vertexBuffer, vkSettings->allocationCallback);
			vkFreeMemory(vkSettings->device, data->vertexBufferMemory, vkSettings->allocationCallback);
		}
		if (!data->indices.empty())
		{
			vkDestroyBuffer(vkSettings->device, data->indexBuffer, vkSettings->allocationCallback);
			vkFreeMemory(vkSettings->device, data->indexBufferMemory, vkSettings->allocationCallback);
		}
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

	// Fill vertex buffer with data
	FillVkVertexBuffer(primitiveName);

	// Fill index buffer with data
	FillVkIndexBuffer(primitiveName);
#endif

}

void BRenderingPipeline::Import(std::string primitiveName, Mesh* mesh)
{
	MeshDataParams* meshData = mesh->GetMeshData();
	Import(primitiveName, meshData->vertices, meshData->indices);
}

void BRenderingPipeline::LoadVertexReadingFormatToVkPipeline(std::string primitiveName)
{
	auto params = primitives[primitiveName]->pipelineBuilderParams;
	// Determines format on how to read vertex buffer
	VkVertexInputBindingDescription vertexInputBindingInfo;
	vertexInputBindingInfo.binding = 0;
	vertexInputBindingInfo.stride = sizeof(DVertex);
	vertexInputBindingInfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// Determines the mapping of vertex buffer data
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
	vertexInputAttributes.resize(3); 
	vertexInputAttributes[0].binding = 0;
	vertexInputAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributes[0].location = 0;
	vertexInputAttributes[0].offset = offsetof(DVertex, DVertex::pos);

	vertexInputAttributes[1].binding = 0;
	vertexInputAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributes[1].location = 1;
	vertexInputAttributes[1].offset = offsetof(DVertex, DVertex::texCoord);

	vertexInputAttributes[2].binding = 0;
	vertexInputAttributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributes[2].location = 2;
	vertexInputAttributes[2].offset = offsetof(DVertex, DVertex::normal);

	vulkanPipelineBuilder->BuildVertexInputDescriptions(params, vertexInputAttributes.data(),
		(VkBool32)vertexInputAttributes.size());

	vulkanPipelineBuilder->BuildVertexInputBindings(params, &vertexInputBindingInfo, 1);
}

void BRenderingPipeline::CreateVkVertexBuffer(std::string primitiveName)
{
	// Reserve memory for vertex buffer
	auto renderBufferData = primitives[primitiveName];
	auto vertexBuffer = &renderBufferData->vertexBuffer;

	// set vertex buffer size
	renderBufferData->vertexBufferSize = sizeof(renderBufferData->vertices[0]) * renderBufferData->vertices.size();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = renderBufferData->vertexBufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result = vkCreateBuffer(vkSettings->device, &bufferInfo, vkSettings->allocationCallback,
		vertexBuffer);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to reserve memory for vertex buffer!");
	}

	// Retrieve memory requirements for setting up vertex buffer
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkSettings->device, *vertexBuffer, &memoryRequirements);

	// find out if memory type is supported in vertex buffer
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkSettings->physicalDevices[vkSettings->discreteGPUIndex],
		&memoryProperties);
	// Flags for allowing the device memory to be accessible and malleable to application code
	VkBool32 memoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBool32 memoryFlagIndex = -1;
	for (VkBool32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (memoryRequirements.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryType) == memoryType)
		{
			memoryFlagIndex = i;
			break;
		}
	}

	if (memoryFlagIndex == -1)
	{
		throw new std::runtime_error("Unable to access vulkan device memory!");
	}

	// Allocate device memory for vertex buffer
	VkMemoryAllocateInfo memoryAllocationInfo = {};
	memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocationInfo.memoryTypeIndex = memoryFlagIndex;
	memoryAllocationInfo.allocationSize = memoryRequirements.size;

	result = vkAllocateMemory(vkSettings->device, &memoryAllocationInfo, vkSettings->allocationCallback, &renderBufferData->vertexBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate device memory for vertex buffer!");
	}

	// Bind device memory to vertex buffer
	vkBindBufferMemory(vkSettings->device, *vertexBuffer, renderBufferData->vertexBufferMemory, 0);
}

void BRenderingPipeline::CreateVkIndexBuffer(std::string primitiveName)
{
	// Reserve memory for vertex buffer
	auto renderBufferData = primitives[primitiveName];
	auto indexBuffer = &renderBufferData->indexBuffer;

	// set index buffer size
	renderBufferData->indexBufferSize = sizeof(renderBufferData->indices[0]) * renderBufferData->indices.size();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = renderBufferData->indexBufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result = vkCreateBuffer(vkSettings->device, &bufferInfo, vkSettings->allocationCallback,
		indexBuffer);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to reserve memory for index buffer!");
	}

	// Retrieve memory requirements for setting up vertex buffer
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkSettings->device, *indexBuffer, &memoryRequirements);

	// find out if memory type is supported in vertex buffer
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkSettings->physicalDevices[vkSettings->discreteGPUIndex],
		&memoryProperties);
	// Flags for allowing the device memory to be accessible and malleable to application code
	VkBool32 memoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBool32 memoryFlagIndex = -1;
	for (VkBool32 i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (memoryRequirements.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryType) == memoryType)
		{
			memoryFlagIndex = i;
			break;
		}
	}

	if (memoryFlagIndex == -1)
	{
		throw new std::runtime_error("Unable to access vulkan device memory!");
	}

	// Allocate device memory for vertex buffer
	VkMemoryAllocateInfo memoryAllocationInfo = {};
	memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocationInfo.memoryTypeIndex = memoryFlagIndex;
	memoryAllocationInfo.allocationSize = memoryRequirements.size;

	result = vkAllocateMemory(vkSettings->device, &memoryAllocationInfo,
		vkSettings->allocationCallback, &renderBufferData->indexBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw new std::runtime_error("Unable to allocate device memory for vertex buffer!");
	}
	
	

	// Bind device memory to vertex buffer
	vkBindBufferMemory(vkSettings->device, *indexBuffer, renderBufferData->indexBufferMemory, 0);
}

void BRenderingPipeline::FillVkVertexBuffer(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	void* data;
	vkMapMemory(vkSettings->device, renderData->vertexBufferMemory, 0, renderData->vertexBufferSize, 0, &data);
	memcpy(data, renderData->vertices.data(), renderData->vertexBufferSize);
	vkUnmapMemory(vkSettings->device, renderData->vertexBufferMemory);
}

void BRenderingPipeline::FillVkIndexBuffer(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	void* data;
	vkMapMemory(vkSettings->device, renderData->indexBufferMemory, 0, renderData->indexBufferSize, 0, &data);
	memcpy(data, renderData->indices.data(), (size_t)renderData->indexBufferSize);
	vkUnmapMemory(vkSettings->device, renderData->indexBufferMemory);
}

void BRenderingPipeline::SetVkDescriptorsForUniformBuffers(std::string primitiveName, std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings)
{
	auto renderData = primitives[primitiveName];
	renderData->descriptorLayoutBindings = descriptorLayoutBindings;

	VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = (VkBool32)renderData->descriptorLayoutBindings.size(),
		.pBindings = renderData->descriptorLayoutBindings.data()
	};

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkDescriptorSetLayout descriptorSetLayout;
	VkResult result = vkCreateDescriptorSetLayout(vkSettings->device, &descriptorLayoutInfo, vkSettings->allocationCallback, &descriptorSetLayout);
	renderData->descriptorSetLayouts.push_back(descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create descriptor set layout!");
	}

}

void BRenderingPipeline::CreatePipelineLayout(std::string primitiveName)
{
	auto renderData = primitives[primitiveName];

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = (VkBool32)renderData->descriptorSetLayouts.size(),
		.pSetLayouts = renderData->descriptorSetLayouts.data()
	};

	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	VkResult result = vkCreatePipelineLayout(vkSettings->device, &pipelineLayoutInfo, vkSettings->allocationCallback, &renderData->pipelineBuilderParams.pipelineLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create pipeline layout from descriptor set layout!");
	}
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
	if (GGLSPtr)
		GGLSPtr->GetWindowDimensions(screenWidth, screenHeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, defaultFrameBufferTextureID, 0);
	// create renderbuffer
	glGenRenderbuffers(1, &renderbuffer);
	// As well as the renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
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
	GGLSPtr->GetWindowDimensions(screenWidth, screenHeight);
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.width = screenWidth;
	framebufferInfo.height = screenHeight;
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

void BRenderingPipeline::DrawVk(VkCommandBuffer cmdBuffer)
{
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	auto it = primitives.begin();
	for (;it != primitives.end(); it++)
	{
		vertexBuffers.push_back(it->second->vertexBuffer);
		offsets.push_back(it->second->vertexBufferOffset);
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
	for (auto it = primitives.begin(); it != primitives.end(); it++)
	{
		auto renderData = it->second;
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1,
			&renderData->vertexBuffer, &renderData->vertexBufferOffset);

		vkCmdBindIndexBuffer(cmdBuffer, renderData->indexBuffer, 0,
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
	// remove ownership to Shader
	if(shader)
		shader.reset();

	// remove glsl hooks
	glDisableVertexAttribArray(0 | 1 | 2);

	// free allocation to all buffer objects
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}
