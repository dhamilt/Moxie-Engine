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

BRenderingPipeline::~BRenderingPipeline()
{
	// Free primitive data 
	for (auto it = primitives.begin(); it != primitives.end(); it++)
		delete it->second;

	primitives.clear();
}

void BRenderingPipeline::Init()
{
	GenerateDefaultFramebuffer();
}

void BRenderingPipeline::Import(std::string primitiveName, std::vector<DVertex> _vertices, std::vector<uint16_t> _indices)
{
	// if the primitive already exists in the pipeline
	if (primitives.find(primitiveName) != primitives.end())
	{
		// update the mesh data in the render data struct
		RenderBufferData* data = primitives[primitiveName];
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
}

void BRenderingPipeline::Import(std::string primitiveName, Mesh* mesh)
{
	MeshDataParams* meshData = mesh->GetMeshData();
	Import(primitiveName, meshData->vertices, meshData->indices);
}

void BRenderingPipeline::GenerateCubemap(std::vector<TextureData*> cubemapTextureData)
{
	// ensure that all sides of the cubemap have texture data
	assert((int)cubemapTextureData.size() == 6);
	// create cubemap
	if (!cubemapParams)
		cubemapParams = &defaultSkyBox;
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
	std::shared_ptr<Shader> cachedShader(new Shader("DefaultMatVS.shader", "DefaultMatFS.shader"));
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

void BRenderingPipeline::GenerateVkFrameBuffer(VkBool32 attachmentCount)
{
	auto vkSettings = PVulkanPlatformInit::Get()->GetInfo();
	GGLSPtr->GetWindowDimensions(screenWidth, screenHeight);
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.width = screenWidth;
	framebufferInfo.height = screenHeight;
	framebufferInfo.attachmentCount = attachmentCount;
	std::vector<VkImageView> attachments;
	for (int i = 0; i < attachmentCount; i++)
		attachments.push_back(vkSettings->imageBuffer[i].imageView);
	framebufferInfo.pAttachments = &attachments[0];
	framebufferInfo.pNext = VK_NULL_HANDLE;
	// TODO: Make VkRenderPass accessible
	/*framebufferInfo.renderPass = vkSettings->renderPass;*/
	framebufferInfo.flags = 0;
	uint32_t index = vkFramebuffers.size();
	vkFramebuffers.resize( index + attachmentCount);
	VkFramebuffer framebuffer;
	auto result = vkCreateFramebuffer(vkSettings->device, &framebufferInfo, vkSettings->allocationCallback, &framebuffer);
	assert(result == VK_SUCCESS);
}

void BRenderingPipeline::LoadFramebuffer(GLuint fbID)
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
