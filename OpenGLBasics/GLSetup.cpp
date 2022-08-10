#include "glPCH.h"
#include "GLSetup.h"
#include <cassert>
#include "GameLoop.h"
#include "Material.h"
#include "Mesh.h"



extern GLSetup* GGLSPtr = new GLSetup();
extern GameLoop* GGLPtr;

GLSetup::GLSetup()
{
	
	
}

GLSetup::~GLSetup()
{
	// Shutdown and clean GUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
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
	
	// Setup GUI
	IMGUI_CHECKVERSION();
	mainWindowGUIContext = ImGui::CreateContext();	
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

	// Link the GUI to the correct context and rendering frameworks
	ImGui_ImplSDL2_InitForOpenGL(sdlWindow, mainSDLContext);
	ImGui_ImplOpenGL3_Init("#version 330");

	

	// Setup an initial Framebuffer
	if(pipeline)
		pipeline->GenerateDefaultFramebuffer();

	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//// Create an attachment texture
	//glGenTextures(1, &screenTextureID);		
	//glBindTexture(GL_TEXTURE_2D, screenTextureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// Attach it to Framebuffer
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTextureID, 0);
	//// Create a Renderbuffer Object
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	//// Attach it to Framebuffer
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//// Remove bindings
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);


	// Setup main camera
	mainCamera = new Camera(DVector3(0, 1, 0));
	// Set up mouse capture callback for rotating the camera
	GGLPtr->AddMouseCapCallback(std::bind(&Camera::RotateCamera, mainCamera, std::placeholders::_1, std::placeholders::_2));
	GGLPtr->GetMainInputHandle()->leftMouseButton->Subscribe(std::bind(&Camera::OnMouseButtonDown, mainCamera), MOX_RELEASED);

	// Set up mouse scroll wheel callback for changing the fov
	GGLPtr->GetMainInputHandle()->scrollWheel->Subscribe(std::bind(&GLSetup::ScrollWheelCallback, this, std::placeholders::_1));
	// Enable Depth and Stencil test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	// Stencil parameters
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	// Accept fragments that are closer to the camera
	glDepthFunc(GL_LESS);

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
		
		pipeline->LoadCurrentFramebuffer();

		// Use the initial framebuffer to record all render data for this frame
		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// What color to use when clearing the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Black background
		
			// Keep a reference of the 4x4 view and projection matrices each frame
		// in order to pass into the drawing of meshes
		view = mainCamera->GetViewMatrix();
		pipeline->UpdateViewMatrix(view);

		// get projecttion matrix
		//projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
		pipeline->UpdateProjectionMatrix(fov, (float)width, (float)height, nearClippingPlane, farClippingPlane);
		
		// Draw Cubemap
		pipeline->DrawCubeMap();

		//// Draw all primitives to the screen
		//pipeline->RenderPrimitives();
		
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
		GLuint uiCount = (GLuint)uiElements.size();
		
		for (GLuint i = 0; i < uiCount; i++)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			uiElements[i]->Paint();
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
		pipeline->LoadShader(primitiveName, mat->Get());
	}

}

void GLSetup::AddCubemapMaterial(Material* cubeMapMat)
{
	
}

void GLSetup::SubmitCubeMapData(std::vector<TextureData*> cubemapData)
{
	if (pipeline)
		pipeline->GenerateCubemap(cubemapData);
}

void GLSetup::ImportMesh(Mesh* mesh)
{
	if (pipeline)
	{
		pipeline->Import(mesh);
		// pass in render data to the renderer in the pipeline
		pipeline->RequestForMeshVertexData(mesh->GetMeshData()->name);
	}
	
}

void GLSetup::ImportMesh(std::string primitiveName, std::vector<DVertex> vertices, std::vector<uint16_t> indices)
{
	if (pipeline)
	{
		pipeline->Import(primitiveName, vertices, indices);
		// pass in render data to the renderer in the pipeline
		pipeline->RequestForMeshVertexData(primitiveName);
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

