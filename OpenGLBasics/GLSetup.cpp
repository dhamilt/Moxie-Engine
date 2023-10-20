#include "glPCH.h"
#include "GLSetup.h"
#include <cassert>
#include "GameLoop.h"
#include "Material.h"
#include "Mesh.h"
#include "VulkanPlatformInit.h"
#include "VulkanPipelineBuilder.h"




extern GLSetup* GGLSPtr = new GLSetup();
extern GameLoop* GGLPtr;

GLSetup::GLSetup()
{
	
	
}

GLSetup::~GLSetup()
{
	// Clean up Rendering Pipeline
	pipeline->CleanupRenderingPipeline();
	// Shutdown and clean GUI
#if USE_VULKAN
	if (clearValues)
		delete[] clearValues;
	ImGui_ImplVulkan_Shutdown();
	PVulkanPlatformInit::Get()->CleanupVulkan();
#else
	ImGui_ImplOpenGL3_Shutdown();
#endif // USE_VULKAN

	
	ImGui_ImplSDL2_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();	
	// if window exists destroy it
	if (sdlWindow)
		SDL_DestroyWindow(sdlWindow);
	
	// End usage of GLFW
	//glfwTerminate();
}

void GLSetup::Init()
{
	// Initialize the rendering pipeline
	pipeline = new BRenderingPipeline();
	// Create the main window
	StartSDLWindow();
	// Create main menu bar for app
	mainMenu = new WMainMenu();
	// Create viewport for application
	viewport = new WViewport();
}

ImGuiContext* GLSetup::GetCurrentContext()
{
	return mainWindowGUIContext;
}

bool GLSetup::IsViewportInFocus()
{
	return viewportInFocus;
}

void GLSetup::StartSDLWindow()
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return;
	}
#if USE_OPENGL
	// Set the minimum OpenGL version this program run on
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Enable the Depth and Stencil Buffers
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	// Creates a window using SDL
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	sdlWindow = SDL_CreateWindow("Moxie Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
	// Make the window the current context 
	SDL_GL_MakeCurrent(sdlWindow, mainSDLContext);
	// Lower the number of buffer swaps for each frame
		// in order to optimize efficiency(V-Sync)
	SDL_GL_SetSwapInterval(1);
	// Create an OpenGL context associated with the SDL window
	mainSDLContext = SDL_GL_CreateContext(sdlWindow);
	if (!sdlWindow)
	{
		printf("Error! %s\n", SDL_GetError());
		assert(sdlWindow);
		throw std::runtime_error("Context window could not be created!");
	}
	
	// Initialize GLEW
	glewExperimental = GL_TRUE;
	bool glewResult = glewInit();
	// if OpenGL extension library doesn't initialize properly
	if (glewResult != GLEW_OK)
	{
		// Throw an exception
		assert(glewResult == GLEW_OK);
		throw std::runtime_error("GLEW could not be initialized!");		
	}
#endif // USE_OPENGL

#if USE_VULKAN
	
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	// Create Vulkan Window
	sdlWindow = SDL_CreateWindow("Moxie Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
	if (!sdlWindow)
	{
		printf("Error! %s\n", SDL_GetError());
		assert(sdlWindow);
		throw std::runtime_error("Context window could not be created!");
	}

	auto platformInstance =PVulkanPlatformInit::Get();
	auto vkSettings = platformInstance->GetInfo();
	// Make sure the vulkan instance was created correctly
	assert(platformInstance->CreateInstance(sdlWindow));

	// Keep ref to the logical device being used
	currentVkDevice = &vkSettings->device;

	auto platformInfo = platformInstance->GetInfo();
	auto surface = &platformInfo->surface;
	auto instance = &platformInfo->instance;
	// Create Vulkan Surface for rendering
	if (!SDL_Vulkan_CreateSurface(sdlWindow, *instance, surface))
	{
		printf("Error! %s\n", SDL_GetError());
		throw std::runtime_error("Could not create Vulkan surface.");
	}

	// Ensure that the debug callbacks were created
	assert(platformInstance->SetupDebugCallbacks());

	// Ensure that the discrete gpu is being used
	assert(platformInstance->GetPhysicalDevices());

	// Ensure that the logical device and device queue are created properly
	assert(platformInstance->CreateLogicalDeviceAndQueue());

	// Ensure the descriptor pool is created properly
	assert(platformInstance->CreateDescriptorPool());

	// Ensure that the swap chain is created properly
	assert(platformInstance->CreateSwapChain());

	// Keep ref to swapchain being used
	VkSwapchain = &vkSettings->swapchain;

	// Ensure that the render pass is created properly
	assert(platformInstance->CreateRenderPass());
	 
	// Allocate commandBuffer queue for max possible frames in flight
	cmdBuffers.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
	// Ensure that the command buffer is created properly
	assert(platformInstance->CreateCommandPool(cmdBuffers.data()));

	inFlightFences.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
	// Ensure that the sync object fence is created properly
	assert(platformInstance->CreateFences(inFlightFences.data()));

	imageAvailableSemaphores.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
	// Ensure that the sync object semaphores are created properly
	assert(platformInstance->CreateSemaphores(imageAvailableSemaphores.data(), renderFinishedSemaphores.data()));

	// Dictate how the command buffer is used on startup of each frame

	beginCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginCmdBufferInfo.pNext = 0;
	beginCmdBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginCmdBufferInfo.pInheritanceInfo = VK_NULL_HANDLE;

	// Dictate how the render pass is used for the startup of each frame
	// NOTE: only the static values are assigned here
	// the dynamic values are assigned at the beginning of each frame
	beginRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginRenderPassInfo.pNext = 0;
	beginRenderPassInfo.renderArea.offset = { 0, 0 };

	/*assert(vkBeginCommandBuffer(*cmdBuffer, beginCmdBufferInfo) == VK_SUCCESS);


	vkResetCommandBuffer(*cmdBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);*/

#endif // USE_VULKAN
	
	

	// Setup GUI
	IMGUI_CHECKVERSION();
	mainWindowGUIContext = ImGui::CreateContext();	
	mainImPlotContext = ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	/*io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;*/	

	// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = false;
	io.ConfigDockingTransparentPayload = true;
	io.ConfigDockingAlwaysTabBar = true;

	// Set the GUI style
	ImGui::StyleColorsDark();
#if USE_OPENGL
	// Create default mesh shader
	pipeline->CreateDefaultShader();

	// Link the GUI to the correct context and rendering frameworks
	ImGui_ImplSDL2_InitForOpenGL(sdlWindow, mainSDLContext);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Setup an initial Framebuffer
	if (pipeline)
		pipeline->GenerateDefaultFramebuffer();
#elif USE_VULKAN
	// Make sure that imgui is setup correctly on the SDL2 Window
	assert(platformInstance->ImGuiVkSetup(sdlWindow));
	ImGui_ImplSDL2_InitForVulkan(sdlWindow);
	ImGui_ImplVulkan_Init(&platformInfo->imGuiInitInfo, platformInstance->GetInfo()->renderPass);

	// Create clear values for Vulkan screen
	clearValues = new VkClearValue[2];
	clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	// Setup initial framebuffers
	if (pipeline)
		pipeline->GenerateVkFrameBuffers();
	CreateVkPipelineForTriangle();
#endif
	

	
	

	
	// Setup main camera
	mainCamera = new Camera(DVector3(0, 1, 0));
	// Set up mouse capture callback for rotating the camera
	GGLPtr->AddMouseCapCallback(std::bind(&Camera::RotateCamera, mainCamera, std::placeholders::_1, std::placeholders::_2));
	GGLPtr->GetMainInputHandle()->leftMouseButton->Subscribe(std::bind(&Camera::OnMouseButtonDown, mainCamera), MOX_RELEASED);

	// Set up mouse scroll wheel callback for changing the fov
	GGLPtr->GetMainInputHandle()->scrollWheel->Subscribe(std::bind(&GLSetup::ScrollWheelCallback, this, std::placeholders::_1));
#if USE_OPENGL
	// Enable Depth and Stencil test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	// Stencil parameters
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	// Accept fragments that are closer to the camera
	glDepthFunc(GL_LESS);
#endif // USE_OPENGL
}

void GLSetup::UpdateLightingCollection(std::string lightComponentName, Light* lightInfo)
{
	// if the current light has not been added to the light map
	if (lightMap.find(lightComponentName) == lightMap.end())
	{
		// add it to the queue
		lightMap.insert(std::pair<std::string, Light>(lightComponentName,*lightInfo));
	}
	// otherwise update the light info
	else
		lightMap[lightComponentName] = *lightInfo;

	// Update the light info array cache
	// TODO: Refactor this light caching to be event driven
	lightsCache.clear();
	for (std::pair<std::string, Light> lightInfo : lightMap)
		lightsCache.push_back(lightInfo.second);
}

void GLSetup::GetAllLightInfo(std::vector<Light>& _lightingInfo)
{
	_lightingInfo.clear();
	// replace and/or retrieve the current light info in the level
	_lightingInfo = lightsCache;
}

void GLSetup::Render()
{
	// Run one frame of the Render thread
	if (sdlWindow)
	{
		// Keep a reference of the 4x4 view and projection matrices each frame
		// in order to pass into the drawing of meshes
		view = mainCamera->GetViewMatrix();
		pipeline->UpdateViewMatrix(view);

		// get projection matrix
		//projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
		pipeline->UpdateProjectionMatrix(fov, (float)width, (float)height, nearClippingPlane, farClippingPlane);

#if USE_OPENGL
		pipeline->LoadCurrentFramebuffer();

		// Use the initial framebuffer to record all render data for this frame
		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// What color to use when clearing the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Black background
		
		
		// Draw Cubemap
		pipeline->DrawCubeMap();

		// Draw all primitives to the screen

		pipeline->RenderPrimitives();
		
		//// Send projection and view matrices to objects
		//// being rendered
		//for (int i = 0; i < renderObjs.size(); i++)
		//	renderObjs[i]->Draw(projection, view);

		// Stop capturing render data for initial framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		pipeline->UnloadFramebuffer();
		// Start a new frame for the GUI to render
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		
		//// Create docking space
		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(mainViewport, ImGuiDockNodeFlags_PassthruCentralNode);

		// Call all Paint calls for UI elements 
		// that exist on the GUI		
		for(auto uiElement : uiElements)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			uiElement->Paint();
		}

		// if quit button is selected
		if (mainMenu->IsExitingApplication())
			// queue the main game loop to quit
			GGLPtr->QuitLoop();

		windowInFocus = mainWindowGUIContext->NavWindow;
		// if there is a window in focus
		if (windowInFocus)
			viewportInFocus = !strcmp("Viewport", windowInFocus->Name);
		else
			viewportInFocus = false;

		// Render the GUI
		ImGui::Render();
		// What color to use when clearing the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Black background
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		
		
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// If multi viewports are enabled
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_GLContext backupContext = SDL_GL_GetCurrentContext();
			SDL_Window* backupWindow = SDL_GL_GetCurrentWindow();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			// Make sure SDL working in the context of this window
			SDL_GL_MakeCurrent(backupWindow, backupContext);
		}		
		
		// Refresh screen with new buffer
		SDL_GL_SwapWindow(sdlWindow);
#elif USE_VULKAN
		auto platformInstance = PVulkanPlatformInit::Get();
		auto vkSettings = platformInstance->GetInfo();

		// Get the index of the requested swapchain index being rendered on
		// set timeout period to 1 second
		VkBool32 swapchainImgIndex;
		assert(vkAcquireNextImageKHR(*currentVkDevice, *VkSwapchain, 1000000000, imageAvailableSemaphores[currentRenderingFrame], VK_NULL_HANDLE, &swapchainImgIndex) == VK_SUCCESS);

		// Wait for GPU to finish rendering the previous frame before drawing the current frame
		// set timeout period to 1 second
		assert(vkWaitForFences(*currentVkDevice, 1, &inFlightFences[currentRenderingFrame], VK_TRUE, 1000000000) == VK_SUCCESS);
		assert(vkResetFences(*currentVkDevice, 1, &inFlightFences[currentRenderingFrame]) == VK_SUCCESS);


		// Restart the command buffer to be ready to record draw commands for the current frame
		assert(vkResetCommandBuffer(cmdBuffers[currentRenderingFrame], 0) == VK_SUCCESS);

		// Begin command buffer recording for the current frame
		assert(vkBeginCommandBuffer(cmdBuffers[currentRenderingFrame], &beginCmdBufferInfo) == VK_SUCCESS);

		

		VkClearDepthStencilValue clearDepthStencilVal = {};
		clearDepthStencilVal.depth = 0.0f;
		clearDepthStencilVal.stencil = 0;	

		// Get the current framebuffer to be used from swapchain
		VkFramebuffer currentFrameBuffer;
		pipeline->GetVkFramebuffer(currentFrameBuffer, swapchainImgIndex);

		// Get the current extent of the swapchain images
		VkExtent2D currentExtent;
		platformInstance->GetWindowExtent(currentExtent);

		beginRenderPassInfo.clearValueCount = 2;
		beginRenderPassInfo.framebuffer = currentFrameBuffer;
		beginRenderPassInfo.renderArea.extent = currentExtent;
		beginRenderPassInfo.pClearValues = clearValues;
		beginRenderPassInfo.renderPass = vkSettings->renderPass;

		// Create render pass on command buffer
		// ensures that the subpass contents is passed into the main command buffer (for now, at least)
		vkCmdBeginRenderPass(cmdBuffers[currentRenderingFrame], &beginRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// RUN DRAW COMMANDS HERE
		vkCmdBindPipeline(cmdBuffers[currentRenderingFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, triangleShaderPipeline[0]);

		pipeline->DrawVkIndexed(cmdBuffers[currentRenderingFrame]);

		// Finalize the render pass for the command buffer
		vkCmdEndRenderPass(cmdBuffers[currentRenderingFrame]);

		vkCmdClearDepthStencilImage(cmdBuffers[currentRenderingFrame], vkSettings->depthBuffer.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepthStencilVal, 1, &vkSettings->depthViewInfo.subresourceRange);

		// Stops recording of draw commands
		assert(vkEndCommandBuffer(cmdBuffers[currentRenderingFrame]) == VK_SUCCESS);

		// Submit draw commands to the device queue to be drawn
		VkSubmitInfo queueSubmitInfo = {};
		queueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		queueSubmitInfo.commandBufferCount = 1;
		queueSubmitInfo.pCommandBuffers = &cmdBuffers[currentRenderingFrame];
		queueSubmitInfo.pNext = VK_NULL_HANDLE;
		queueSubmitInfo.signalSemaphoreCount = 1;
		queueSubmitInfo.pSignalSemaphores = &vkSettings->renderFinishedSemaphores[currentRenderingFrame];
		queueSubmitInfo.waitSemaphoreCount = 1;
		queueSubmitInfo.pWaitSemaphores = &vkSettings->imageAvailableSemaphores[currentRenderingFrame];
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		queueSubmitInfo.pWaitDstStageMask = &waitStage;

		assert(vkQueueSubmit(vkSettings->queue, 1, &queueSubmitInfo, inFlightFences[currentRenderingFrame]) == VK_SUCCESS);
		
		// Render the image to the window/surface
		
		VkPresentInfoKHR presentationInfo = {};
		presentationInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentationInfo.pNext = VK_NULL_HANDLE;
		presentationInfo.swapchainCount = 1;
		presentationInfo.pSwapchains = &vkSettings->swapchain;
		presentationInfo.waitSemaphoreCount = 1;
		presentationInfo.pWaitSemaphores = &vkSettings->renderFinishedSemaphores[currentRenderingFrame];
		presentationInfo.pImageIndices = &swapchainImgIndex;
		presentationInfo.pResults = &renderingResult;

		assert(vkQueuePresentKHR(vkSettings->queue, &presentationInfo) == VK_SUCCESS);

		// Retrieve the next frame
		currentRenderingFrame = (currentRenderingFrame + 1) % MAX_VULKAN_FRAMES_IN_FLIGHT;
#endif
	}


	
}

void GLSetup::GetWindowDimensions(int& w, int& h)
{
	w = width;
	h = height;
}



float GLSetup::GetFarClippingPlane()
{
	return farClippingPlane;
}

float GLSetup::GetNearClippingPlane()
{
	return nearClippingPlane;
}

void GLSetup::SetFarClippingPlane(float val)
{
	farClippingPlane = val;
}

void GLSetup::SetNearClippingPlane(float val)
{
	nearClippingPlane = val;
}

void GLSetup::GetViewportTextureID(GLuint& textureID, GLuint& renderbufferObjectID)
{
	textureID = pipeline->defaultFrameBufferTextureID;
	renderbufferObjectID = pipeline->renderbuffer;
}

void GLSetup::GetViewportDimensions(int& _width, int& _height)
{	
	if (viewportInFocus)
	{
		ImVec2 windowSize = windowInFocus->Size;
		_width = (int)windowSize.x;
		_height = (int)windowSize.y;
	}
}

void GLSetup::AddMaterialToPipeline(std::string primitiveName, Material* mat)
{
	if (pipeline)
	{
		pipeline->UpdateShaderCache(mat);
		pipeline->LoadShader(primitiveName, mat);
	}

}

void GLSetup::CreateVkPipelineForTriangle()
{
	VkExtent2D windowExtent{};
	windowExtent.width = width;
	windowExtent.height = height;
	VkPipelineBuilder* pipelineBuilder = new VkPipelineBuilder();
	triangleShaderPipeline.resize(1);
	pipelineBuilder->GetTriangleShaderPipeline(windowExtent, &triangleShaderPipeline[0]);
	delete pipelineBuilder;
}

void GLSetup::AddCubemapMaterial(Material* cubeMapMat)
{
	
}

void GLSetup::GetDefaultMeshShader(Shader* defaultShader)
{
	if (pipeline)
		pipeline->GetDefaultShader(defaultShader);
}

void GLSetup::SubmitCubeMapData(std::vector<TextureData*> cubemapData)
{
#if USE_OPENGL
	if (pipeline)
		pipeline->GenerateCubemap(cubemapData);
#elif USE_VULKAN

#endif
}

void GLSetup::ImportMesh(std::string name, Mesh* mesh)
{
	if (pipeline)
	{
		pipeline->Import(name, mesh);
#if USE_OPENGL
		// pass in render data to the renderer in the pipeline
		pipeline->RequestForMeshVertexData(name);
#endif
	}
	
}

void GLSetup::ImportMesh(std::string primitiveName, std::vector<DVertex> vertices, std::vector<uint16_t> indices)
{
	if (pipeline)
	{
		pipeline->Import(primitiveName, vertices, indices);
#if USE_OPENGL
		// pass in render data to the renderer in the pipeline
		pipeline->RequestForMeshVertexData(primitiveName);
#endif
	}
}

BRenderingPipeline* GLSetup::GetPipeline()
{
	return pipeline? pipeline : nullptr;
}

DMat4x4 GLSetup::GetCameraView()
{
	return view;
}

DMat4x4 GLSetup::GetProjection()
{
	return projection;
}

void GLSetup::AddRenderObject(Graphic* ptr)
{
	renderObjs.push_back(ptr);
	printf("There are currently %d objects in the render queue.\n", (int)renderObjs.size());
}

// Remove render object if found in queue
void GLSetup::RemoveRenderObject(Graphic* ptr)
{
	int index = -1;
	int i = 0;
	while (i < renderObjs.size())
	{
		
		if (ptr->id == renderObjs[i]->id && ptr->type == renderObjs[i]->type)
		{
			index = i;
			break;
		}
		i++;
	}

	if (index >= 0)
		renderObjs.erase(renderObjs.begin() + index);
}

void GLSetup::GLFW_ErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void GLSetup::ScrollWheelCallback(int axisVal)
{
	if(fov + axisVal >= fovMinConstraint && fov + axisVal <= fovMaxConstraint)
	fov += axisVal;
}

void GLSetup::AddUIElement(GUI_Base* ptr)
{
	uiElements.push_back(ptr);
	fprintf(stderr, "There are %d ui elements being drawn.", (int)uiElements.size());
}

void GLSetup::RemoveUIElement(GUI_Base* ptr)
{
	int i = 0;
	int index = -1;
	while (i < uiElements.size())
	{
		if (ptr->GetName() == uiElements[i]->GetName() && ptr->id == uiElements[i]->id)
		{
			index = i;
			break;
		}
		i++;
	}
	if (index >= 0)
		uiElements.erase(uiElements.begin() + index);
}

